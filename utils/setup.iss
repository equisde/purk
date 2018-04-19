; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Purk"
;#define MyAppVersion "0.2.0.31"
#define MyAppPublisher "Purk Project"
#define MyAppURL "https://purkproject.com"
#define MyAppExeName "qt-purk.exe"
;#define BinariesPath "C:\jenkins\workdir\builds\purk-win-x64-v0.2.0.31(5d85ebf)"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{8D8E3254-0AE8-4EB5-A8EB-60FB031C5348}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes
ArchitecturesInstallIn64BitMode=x64
WizardImageFile=../resources/bg.bmp
WizardSmallImageFile=../resources/icon.bmp
;SetupIconFile=../resources/app.ico


[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Registry]
Root: HKCR; Subkey: ".purk"; ValueType: string; ValueName: ""; ValueData: "PurkWalletDataFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".purk.keys"; ValueType: string; ValueName: ""; ValueData: "PurkWalletDataKeysFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "PurkWalletDataFile"; ValueType: string; ValueName: ""; ValueData: "Purk Wallet's Data File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "PurkWalletDataKeysFile"; ValueType: string; ValueName: ""; ValueData: "Purk Wallet's Keys File"; Flags: uninsdeletekey

Root: HKCR; Subkey: "PurkWalletDataFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\qt-purk.exe,0"
Root: HKCR; Subkey: "PurkWalletDataKeysFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\qt-purk.exe,0"


[Files]
Source: "{#BinariesPath}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs ignoreversion overwritereadonly replacesameversion
Source: "..\src\gui\qt-daemon\html\*"; DestDir: "{app}\html"; Flags: ignoreversion recursesubdirs
Source: "{#BinariesPath}\vcredist_x64.exe"; DestDir: {tmp}; Flags: deleteafterinstall
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon


[Run]
Filename: {tmp}\vcredist_x64.exe; Parameters: "/install /quiet /norestart";  StatusMsg: Installing VC++ 2013 Redistributables...
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

