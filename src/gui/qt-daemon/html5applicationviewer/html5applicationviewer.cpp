// Copyright (c) 2012-2013 The Boolberry developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Copyright (c) 2017-2018 The Purk Project Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "html5applicationviewer.h"

#include <QCoreApplication>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLinearLayout>
#include <QGraphicsWebView>
#include <QWebFrame>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QClipboard>
#include <QString>
#include <QStyleFactory>
#include "warnings.h"
#include "net/http_client.h"

#include <QThread>
#include "worker.h"
#include "transferingwindow.h"
#include "jsonbuilder.h"
#include "simplewallet/simplewallet.h"

class Html5ApplicationViewerPrivate : public QGraphicsView
{
  Q_OBJECT
public:
  Html5ApplicationViewerPrivate(QWidget *parent = 0);

  void resizeEvent(QResizeEvent *event);
  static QString adjustPath(const QString &path);

  public slots:
  void quit();
  void closeEvent(QCloseEvent *event);

  private slots:
  void addToJavaScript();

signals:
  void quitRequested();
  void update_daemon_state(const QString str);
  void update_wallet_status(const QString str);
  void update_wallet_info(const QString str);
  void money_transfer(const QString str);
  void show_wallet();
  void hide_wallet();
  void switch_view(const QString str);
  void set_recent_transfers(const QString str);
  void handle_internal_callback(const QString str, const QString callback_name);

public:
  QGraphicsWebView *m_webView;
#ifdef TOUCH_OPTIMIZED_NAVIGATION
  NavigationController *m_controller;
#endif // TOUCH_OPTIMIZED_NAVIGATION
};

void Html5ApplicationViewerPrivate::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

Html5ApplicationViewerPrivate::Html5ApplicationViewerPrivate(QWidget *parent)
: QGraphicsView(parent)
{
  QGraphicsScene *scene = new QGraphicsScene;
  setScene(scene);
  setFrameShape(QFrame::NoFrame);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  m_webView = new QGraphicsWebView;
  m_webView->setStyle(QStyleFactory::create("Fusion"));
  //m_webView->setAcceptTouchEvents(true);
  //m_webView->setAcceptHoverEvents(false);
  m_webView->setAcceptTouchEvents(false);
  m_webView->setAcceptHoverEvents(true);
  //setAttribute(Qt::WA_AcceptTouchEvents, true);
  scene->addItem(m_webView);
  scene->setActiveWindow(m_webView);
  auto settings = m_webView->settings();
  settings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,true);
  settings->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);

  // QList<QSslError> expectedSslErrors;
  // expectedSslErrors.append(QSslError::SelfSignedCertificate);
  // expectedSslErrors.append(QSslError::CertificateUntrusted);
  // reply->ignoreSslErrors(expectedSslErrors);

#ifdef TOUCH_OPTIMIZED_NAVIGATION
  m_controller = new NavigationController(parent, m_webView);
#endif // TOUCH_OPTIMIZED_NAVIGATION
  connect(m_webView->page()->mainFrame(),
    SIGNAL(javaScriptWindowObjectCleared()), SLOT(addToJavaScript()));
}

void Html5ApplicationViewerPrivate::resizeEvent(QResizeEvent *event)
{
  m_webView->resize(event->size());
}

QString Html5ApplicationViewerPrivate::adjustPath(const QString &path)
{
#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
  if (!QDir::isAbsolutePath(path))
    return QCoreApplication::applicationDirPath()
    + QLatin1String("/../Resources/") + path;
#else
  const QString pathInInstallDir = QCoreApplication::applicationDirPath()
    + QLatin1String("/../") + path;
  if (pathInInstallDir.contains(QLatin1String("opt"))
    && pathInInstallDir.contains(QLatin1String("bin"))
    && QFileInfo(pathInInstallDir).exists()) {
    return pathInInstallDir;
  }
#endif
#endif
  return QFileInfo(path).absoluteFilePath();
}

void Html5ApplicationViewerPrivate::quit()
{
  emit quitRequested();
}

void Html5ApplicationViewerPrivate::addToJavaScript()
{
  m_webView->page()->mainFrame()->addToJavaScriptWindowObject("Qt_parent", QGraphicsView::parent());
  m_webView->page()->mainFrame()->addToJavaScriptWindowObject("Qt", this);
}

Html5ApplicationViewer::Html5ApplicationViewer(QWidget *parent): QWidget(parent),
m_d(new Html5ApplicationViewerPrivate(this)),
m_quit_requested(false),
m_deinitialize_done(false),
m_backend_stopped(false),
m_worker{nullptr},
m_rpc_thread{nullptr},
m_transfering_window{nullptr},
m_keys_path{nullptr},
m_pass{nullptr},
m_timer{nullptr},
m_isDialog{false},
m_backend{nullptr}
{
  //connect(m_d, SIGNAL(quitRequested()), SLOT(close()));

  m_request_uri_threads_count = 0;

  connect(m_d, SIGNAL(quitRequested()), this, SLOT(on_request_quit()));

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_d);
  layout->setMargin(0);
  setLayout(layout);
}

bool Html5ApplicationViewer::init_config()
{

  epee::serialization::load_t_from_json_file(m_config, m_backend->get_config_folder() + "/" + GUI_CONFIG_FILENAME);
  if (!m_config.wallets_last_used_dir.size())
  {
    m_config.wallets_last_used_dir = tools::get_default_user_dir();
    tools::create_directories_if_necessary(m_config.wallets_last_used_dir);
  }
  return true;
}

bool Html5ApplicationViewer::store_config()
{
  epee::serialization::store_t_to_json_file(m_config, m_backend->get_config_folder() + "/" + GUI_CONFIG_FILENAME);
  return true;
}

void Html5ApplicationViewer::stop_rpc()
{
   if (m_worker != nullptr) {
    m_worker->stop();
    delete m_worker;
    m_worker = nullptr;
  }

  if (m_rpc_thread != nullptr) {
    m_rpc_thread->quit();
//    delete m_rpc_thread;
    m_rpc_thread = nullptr;
  }

  if (m_timer != nullptr) {
    m_timer->stop();
    delete m_timer;
    m_timer = nullptr;
  }
}

Html5ApplicationViewer::~Html5ApplicationViewer()
{
  stop_rpc();

  if (m_transfering_window != nullptr) {
    delete m_transfering_window;
  }

  while (m_request_uri_threads_count > 0)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  store_config();
  delete m_d;
}

void Html5ApplicationViewer::stop_backend()
{
  if (m_backend != nullptr) {
    m_backend->stop();

    delete m_backend;
  }
}

void Html5ApplicationViewer::closeEvent(QCloseEvent *event)
{
  if (!m_deinitialize_done)
  {
    on_request_quit();
    event->ignore();
    hide();
  }
  else
  {
    event->accept();
  }
}

void Html5ApplicationViewer::changeEvent(QEvent *e)
{
  switch (e->type())
  {
  case QEvent::WindowStateChange:
  {
    if (this->windowState() & Qt::WindowMinimized)
    {
      if (m_trayIcon)
      {
        QTimer::singleShot(250, this, SLOT(hide()));
        m_trayIcon->showMessage("Purk app is minimized to tray",
        "You can restore it with double-click or context menu");
      }
    } else {
      showNormal();
    }

    break;
  }
  default:
    break;
  }

  QWidget::changeEvent(e);
}

void Html5ApplicationViewer::initTrayIcon(const std::string& htmlPath)
{
  if (!QSystemTrayIcon::isSystemTrayAvailable())
    return;

  m_restoreAction = std::unique_ptr<QAction>(new QAction(tr("&Restore"), this));
  connect(m_restoreAction.get(), SIGNAL(triggered()), this, SLOT(showNormal()));

  m_quitAction = std::unique_ptr<QAction>(new QAction(tr("&Quit"), this));
  connect(m_quitAction.get(), SIGNAL(triggered()), this, SLOT(on_request_quit()));

  m_trayIconMenu = std::unique_ptr<QMenu>(new QMenu(this));
  m_trayIconMenu->addAction(m_restoreAction.get());
  m_trayIconMenu->addSeparator();
  m_trayIconMenu->addAction(m_quitAction.get());

  m_trayIcon = std::unique_ptr<QSystemTrayIcon>(new QSystemTrayIcon(this));
  m_trayIcon->setContextMenu(m_trayIconMenu.get());
#ifdef WIN32
	std::string iconPath(htmlPath + "/app16.png"); // windows tray icon size is 16x16
#else
	std::string iconPath(htmlPath + "/app22.png"); // X11 tray icon size is 22x22
#endif
  m_trayIcon->setIcon(QIcon(iconPath.c_str()));
  m_trayIcon->setToolTip("Purk");
  connect(m_trayIcon.get(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
  m_trayIcon->show();
}

void Html5ApplicationViewer::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  if (reason == QSystemTrayIcon::ActivationReason::DoubleClick
  || reason == QSystemTrayIcon::ActivationReason::Trigger)
  {
    showNormal();
    activateWindow();
  }
}

bool Html5ApplicationViewer::removeDir(const QString & dirName)
{
  bool result = true;
  QDir dir(dirName);

  if (dir.exists()) {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
    if (info.isDir()) {
      result = removeDir(info.absoluteFilePath());
    }
    else {
      result = QFile::remove(info.absoluteFilePath());
    }

    if (!result) {
      return result;
    }
  }

  result = QDir().rmdir(dirName);
}

return result;
}

void Html5ApplicationViewer::loadFile(const QString &fileName)
{
  m_d->m_webView->setUrl(QUrl::fromLocalFile(Html5ApplicationViewerPrivate::adjustPath(fileName)));
}

void Html5ApplicationViewer::loadUrl(const QUrl &url)
{
  m_d->m_webView->setUrl(url);
}
PUSH_WARNINGS
DISABLE_VS_WARNINGS(4065)
void Html5ApplicationViewer::setOrientation(ScreenOrientation orientation)
{
  Qt::WidgetAttribute attribute;
  switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
  case ScreenOrientationLockPortrait:
    attribute = static_cast<Qt::WidgetAttribute>(128);
    break;
  case ScreenOrientationLockLandscape:
    attribute = static_cast<Qt::WidgetAttribute>(129);
    break;
  default:
  case ScreenOrientationAuto:
    attribute = static_cast<Qt::WidgetAttribute>(130);
    break;
#elif QT_VERSION < 0x050000
  case ScreenOrientationLockPortrait:
    attribute = Qt::WA_LockPortraitOrientation;
    break;
  case ScreenOrientationLockLandscape:
    attribute = Qt::WA_LockLandscapeOrientation;
    break;
  default:
  case ScreenOrientationAuto:
    attribute = Qt::WA_AutoOrientation;
    break;
#else
  default:
    attribute = Qt::WidgetAttribute();
#endif
  };
  setAttribute(attribute, true);
}
POP_WARNINGS

void Html5ApplicationViewer::showExpanded()
{
#if defined(Q_WS_MAEMO_5)
  showMaximized();
#else
  show();
#endif
    this->setMouseTracking(true);
    this->setMinimumWidth(1000);
    this->setMinimumHeight(650);
    //this->setFixedSize(800, 600);
    m_d->m_webView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    m_d->m_webView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
}

QGraphicsWebView *Html5ApplicationViewer::webView() const
{
  return m_d->m_webView;
}

bool Html5ApplicationViewer::on_request_quit()
{
  hide();
  m_quit_requested = true;
  if (m_backend_stopped)
  {
    bool r = QMetaObject::invokeMethod(this,
      "do_close",
      Qt::QueuedConnection);
  }
  else
    m_backend->send_stop_signal();
  return true;
}

bool Html5ApplicationViewer::do_close()
{
  this->close();
  return true;
}

void Html5ApplicationViewer::startTransfering()
{
  currency::set_accepted(true);
  currency::set_show_ping(false);

  m_isDialog = false;
}

void Html5ApplicationViewer::cancelTransfering()
{
  currency::set_accepted(false);
  currency::set_show_ping(false);

  m_isDialog = false;
}

void Html5ApplicationViewer::confirm_transfer()
{
  if (currency::show_ping() && !m_isDialog) {
    if (m_transfering_window != nullptr) {
      delete m_transfering_window;
    }
    m_transfering_window = new TransferingWindow(this);

    connect(m_transfering_window, SIGNAL(startTransfering()), this, SLOT(startTransfering()));
    connect(m_transfering_window, SIGNAL(cancelTransfering()), this, SLOT(cancelTransfering()));
    QString lAmount = QString::number(currency::amount());
    QString lAddress = QString::fromStdString(currency::address());
    m_transfering_window->initializeDevice(lAmount, lAddress);

    m_isDialog = true;
  }
}

bool Html5ApplicationViewer::on_backend_stopped()
{
  m_backend_stopped = true;
  m_deinitialize_done = true;
  if (m_quit_requested)
  {
    bool r = QMetaObject::invokeMethod(this,
      "do_close",
      Qt::QueuedConnection);
  }
  return true;
}

bool Html5ApplicationViewer::update_daemon_status(const view::daemon_status_info& info)
{
  //m_d->update_daemon_state(info);
  std::string json_str;
  epee::serialization::store_t_to_json(info, json_str);
  m_d->update_daemon_state(json_str.c_str());
  return true;
}

QString Html5ApplicationViewer::request_aliases()
{
  QString res = "{}";
  view::alias_set al_set;
  if (m_backend->get_aliases(al_set))
  {
    std::string json_str;
    epee::serialization::store_t_to_json(al_set, json_str);

    res = json_str.c_str();
  }
  return res;
}

bool Html5ApplicationViewer::show_msg_box(const std::string& message)
{
  QMessageBox::information(m_d, "Error", message.c_str(), QMessageBox::Ok);
  return true;
}

bool Html5ApplicationViewer::start_backend(int argc, char* argv[])
{
  m_backend = new daemon_backend();
  m_backend->set_app(this);
  return m_backend->start(argc, argv, this);
}

void Html5ApplicationViewer::start_wallet_rpc()
{
  int argc = 7;
  char *argv[7];
  argv[0] = "Purk";
  argv[1] = "--rpc-bind-port";
  argv[2] = "8081";
  argv[3] = "--wallet-file";
  argv[4] = m_keys_path;
  argv[5] = "--password";
  argv[6] = m_pass;

  stop_rpc();

  m_worker = new Worker(argc, argv);
  m_rpc_thread = new QThread();

  if (m_timer == nullptr) {
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(confirm_transfer()));
  }
  m_timer->start(1000);

  m_worker->moveToThread(m_rpc_thread);
  connect(m_rpc_thread, SIGNAL(started()), m_worker, SLOT(mainLoop()));
  // connect(m_worker, SIGNAL(finished()), m_rpc_thread, SLOT(quit()), Qt::DirectConnection);

  m_rpc_thread->start();
}

void Html5ApplicationViewer::set_credentials(char *keys_path, char *pass)
{
  if (m_keys_path != nullptr) {
    delete m_keys_path;
  }
  m_keys_path = keys_path;

  if (m_pass != nullptr) {
    delete m_pass;
  }
  m_pass = pass;
}

bool Html5ApplicationViewer::update_wallet_status(const view::wallet_status_info& wsi)
{
  std::string json_str;
  epee::serialization::store_t_to_json(wsi, json_str);
  m_d->update_wallet_status(json_str.c_str());
  return true;
}

bool Html5ApplicationViewer::update_wallet_info(const view::wallet_info& wsi)
{
  std::string json_str;
  epee::serialization::store_t_to_json(wsi, json_str);
  m_d->update_wallet_info(json_str.c_str());
  return true;
}

bool Html5ApplicationViewer::money_transfer(const view::transfer_event_info& tei)
{
  std::string json_str;
  epee::serialization::store_t_to_json(tei, json_str);
  m_d->money_transfer(json_str.c_str());
  if (!m_trayIcon)
    return true;
  if (!tei.ti.is_income)
    return true;
  double amount = double(tei.ti.amount) * (1e-6);
  if (tei.ti.height == 0) // unconfirmed trx
  {
    QString msg;
    msg.setNum(amount, 'f', 6);
    msg.append(" PURK is received");
    m_trayIcon->showMessage("Income transfer (unconfirmed)", msg);
  }
  else // confirmed trx
  {
    QString msg;
    msg.setNum(amount, 'f', 6);
    msg.append(" PURK is confirmed");
    m_trayIcon->showMessage("Income transfer confirmed", msg);
  }
  return true;
}


bool Html5ApplicationViewer::hide_wallet()
{
  m_d->hide_wallet();
  return true;
}
bool Html5ApplicationViewer::show_wallet()
{
  m_d->show_wallet();
  return true;
}

void Html5ApplicationViewer::close_wallet()
{
  stop_rpc();

  m_backend->close_wallet();
}

void Html5ApplicationViewer::add_address(const QString& name, const QString& address,
	const QString& alias, const QString& paymentId)
{
	gui_config::addressbook_entry row({ name.toStdString(),
		address.toStdString(), alias.toStdString(), paymentId.toStdString() });
	m_config.address_book.entries.push_back(row);
}

void Html5ApplicationViewer::delete_address(const QString& name, const QString& address,
	const QString& alias, const QString& paymentId)
{
	gui_config::addressbook_entry row({ name.toStdString(),
		address.toStdString(), alias.toStdString(), paymentId.toStdString() });
	m_config.address_book.entries.erase(
		std::remove(m_config.address_book.entries.begin(),
		m_config.address_book.entries.end(), row),
		m_config.address_book.entries.end()
		);
}

QString Html5ApplicationViewer::get_addressbook()
{
	std::string json_str;
	epee::serialization::store_t_to_json(m_config.address_book, json_str);
	return json_str.c_str();
}

bool Html5ApplicationViewer::switch_view(int view_no)
{
  view::switch_view_info swi = AUTO_VAL_INIT(swi);
  swi.view = view_no;
  std::string json_str;
  epee::serialization::store_t_to_json(swi, json_str);
  m_d->switch_view(json_str.c_str());
  return true;
}

bool Html5ApplicationViewer::set_recent_transfers(const view::transfers_array& ta)
{
  std::string json_str;
  epee::serialization::store_t_to_json(ta, json_str);
  m_d->set_recent_transfers(json_str.c_str());
  return true;
}

bool Html5ApplicationViewer::set_html_path(const std::string& path)
{
  initTrayIcon(path);
  loadFile(QLatin1String((path + "/index.html").c_str()));
  return true;
}

QString Html5ApplicationViewer::get_version()
{
  return PROJECT_VERSION_LONG;
}

bool is_uri_allowed(const QString& uri)
{
  //TODO: add some code later here
  return true;
}

QString Html5ApplicationViewer::request_uri(const QString& url_str, const QString& params, const QString& callbackname)
{

  ++m_request_uri_threads_count;
  std::thread([url_str, params, callbackname, this](){

    view::request_uri_params prms;
    if (!epee::serialization::load_t_from_json(prms, params.toStdString()))
    {
      show_msg_box("Internal error: failed to load request_uri params");
      m_d->handle_internal_callback("", callbackname);
      --m_request_uri_threads_count;
      return;
    }
    QNetworkAccessManager NAManager;
    QUrl url(url_str);
    QNetworkRequest request(url);
    QNetworkReply *reply = NAManager.get(request);
    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    QByteArray res = reply->readAll();
    m_d->handle_internal_callback(res, callbackname);
    --m_request_uri_threads_count;
  }).detach();

  return "";
}

QString Html5ApplicationViewer::transfer(const QString& json_transfer_object)
{
    qDebug() << json_transfer_object;
  view::transfer_params tp = AUTO_VAL_INIT(tp);
  view::transfer_response tr = AUTO_VAL_INIT(tr);
  tr.success = false;
  if (!epee::serialization::load_t_from_json(tp, json_transfer_object.toStdString()))
  {
    show_msg_box("Internal error: failed to load transfer params");
    return epee::serialization::store_t_to_json(tr).c_str();
  }

  if (!tp.destinations.size())
  {
    show_msg_box("Internal error: empty destinations");
    return epee::serialization::store_t_to_json(tr).c_str();
  }

  currency::transaction res_tx = AUTO_VAL_INIT(res_tx);

  if (!m_backend->transfer(tp, res_tx))
  {
    return epee::serialization::store_t_to_json(tr).c_str();
  }
  tr.success = true;
  tr.tx_hash = string_tools::pod_to_hex(currency::get_transaction_hash(res_tx));
  tr.tx_blob_size = currency::get_object_blobsize(res_tx);

  return epee::serialization::store_t_to_json(tr).c_str();
}

void Html5ApplicationViewer::message_box(const QString& msg)
{
  show_msg_box(msg.toStdString());
}

void Html5ApplicationViewer::generate_wallet()
{
	QFileDialog dialogFile(this);
	std::string default_file = (tools::get_current_username() + "_wallet.purk").c_str();
	QString path = dialogFile.getSaveFileName(this, tr("Wallet file to store"),
		(m_config.wallets_last_used_dir + "/" + default_file).c_str(),
		tr("Purk wallet (*.purk *.purk.keys);; All files (*.*)"));

	if (!path.length())
		return;

	m_config.wallets_last_used_dir = boost::filesystem::path(path.toStdString()).parent_path().string();

	//read password
	bool ok;
	QString pass = QInputDialog::getText(this, tr("Enter wallet password"),
		tr("Password:"), QLineEdit::Password,
		QString(), &ok);

	if (!ok)
		return;

	std::string seed; //not used yet
	m_backend->generate_wallet(path.toStdString(), pass.toStdString(), seed);

    if (seed != "") {
        QMessageBox msgBox(this);
        msgBox.setText("Keep this phrase safe. You will need it to recover your wallet:");
        msgBox.setInformativeText(QString::fromStdString(seed));
        msgBox.exec();
    }
}

QString Html5ApplicationViewer::get_password()
{
  bool ok;
  QString pass = QInputDialog::getText(this, tr("Enter wallet password"),
    tr("Password:"), QLineEdit::Password,
    QString(), &ok);

  if (!ok)
    return QString();
  return pass;
}

QString Html5ApplicationViewer::get_seed_text()
{
  bool ok;
  QString seed_text = QInputDialog::getMultiLineText(this, tr("Enter seed words"),
    tr("Enter seed words"), QString(), &ok);

  if (!ok)
    return QString();

  return seed_text;
}

bool Html5ApplicationViewer::restore_wallet(const QString& restore_text, const QString& password, const QString& path)
{
  if (!path.length())
  {
    show_msg_box("Empty wallet path");
    return false;
  }

  m_config.wallets_last_used_dir = boost::filesystem::path(path.toStdString()).parent_path().string();

  m_backend->set_restore_wallet_from_zero(true);

  return m_backend->restore_wallet(path.toStdString(), restore_text.toStdString(), password.toStdString());
}

void Html5ApplicationViewer::reconnect()
{
  stop_backend();

  int argc = 1;
  char *argv[1];
  argv[0] = "Purk";
  start_backend(argc, argv);
}

void Html5ApplicationViewer::resync_blockcahin()
{
  QMessageBox msgBox(QMessageBox::NoIcon,
    "Warning!",
    "Warning! All blockchain data will be deleted and resynced from 0. This will take some time. Do you want to proceed?",
    QMessageBox::Yes | QMessageBox::No,
    this);
  msgBox.setDefaultButton(QMessageBox::Yes);
  msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

  if (msgBox.exec() == QMessageBox::Yes) {
    QString blockchain_foler = QString::fromStdString(m_backend->get_config_folder());
    delete m_backend;

    removeDir(blockchain_foler);

    int argc = 1;
    char *argv[1];
    argv[0] = "Purk";
    start_backend(argc, argv);
  }
}

void Html5ApplicationViewer::place_to_clipboard(const QString& data)
{
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(data);
}

QString Html5ApplicationViewer::browse_wallet(bool existing)
{
	if (existing)
	{
		return QFileDialog::getOpenFileName(this, tr("Open wallet File"),
			m_config.wallets_last_used_dir.c_str(),
			tr("Purk wallet (*.purk *.purk.keys);; All files (*.*)"));
	}
	QFileDialog dialogFile(this);
	std::string default_file = (tools::get_current_username() + "_wallet.purk").c_str();
	return dialogFile.getSaveFileName(this, tr("Wallet file to store"),
		(m_config.wallets_last_used_dir + "/" + default_file).c_str(),
		tr("Purk wallet (*.purk *.purk.keys);; All files (*.*)"));
}

void Html5ApplicationViewer::open_wallet()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open wallet File"),
		m_config.wallets_last_used_dir.c_str(),
		tr("Purk wallet (*.purk *.purk.keys);; All files (*.*)"));
	if (!path.length())
		return;

	m_config.wallets_last_used_dir = boost::filesystem::path(path.toStdString()).parent_path().string();


	//read password
	bool ok;
	QString pass = QInputDialog::getText(this, tr("Enter wallet password"),
		tr("Password:"), QLineEdit::Password,
		QString(), &ok);
	if (!ok)
		return;

	m_backend->open_wallet(path.toStdString(), pass.toStdString());
}

#include "html5applicationviewer.moc"
