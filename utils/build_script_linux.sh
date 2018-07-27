#!/bin/bash

QT_PREFIX_PATH="$HOME/Qt5.5.1/5.5/gcc_64"
QT_BINARIES_PATH="$HOME/purk_binaries"
PROJECT_ROOT="$HOME/purk"


cd "$PROJECT_ROOT"
prj_root=$(pwd)

#git pull
#if [ $? -ne 0 ]; then
 #   echo "Failed to pull"
  #  exit $?
#fi

echo "---------------- BUILDING PROJECT ----------------"
echo "--------------------------------------------------"

echo "Backupping wallet files(on the off-chance)"
cp -v build/release/src/*.bin build/release/src/*.bin.keys build/release/src/*.purk build/release/src/*.purk.keys build/release/src/*.purk.address.txt build/release/src/*.bin.address.txt ..

echo "Building...." 
rm -rf build; mkdir -p build/release; 
cd build/release; 
cmake -D STATIC=true -D BUILD_GUI=TRUE -D CMAKE_PREFIX_PATH="$QT_PREFIX_PATH" -D CMAKE_BUILD_TYPE=Release ../..
if [ $? -ne 0 ]; then
    echo "Failed to run cmake"
    exit 1
fi

make -j daemon;
if [ $? -ne 0 ]; then
    echo "Failed to make!"
    exit 1
fi
make -j Purk;
if [ $? -ne 0 ]; then
    echo "Failed to make!"
    exit 1
fi
make -j simplewallet;
if [ $? -ne 0 ]; then
    echo "Failed to make!"
    exit 1
fi
make -j simpleminer;
if [ $? -ne 0 ]; then
    echo "Failed to make!"
    exit 1
fi
make -j connectivity_tool;
if [ $? -ne 0 ]; then
    echo "Failed to make!"
    exit 1
fi

read version_str <<< $(./src/purkd --version | awk '/^Purk / { print $2 }')
echo $version_str


mkdir -p purk;

cp -Rv ../../src/gui/qt-daemon/html ./purk
cp -Rv ../../utils/Purk.sh ./purk
chmod 777 ./purk/Purk.sh

mkdir ./purk/lib
cp $QT_PREFIX_PATH/lib/libicudata.so.56 ./purk/lib
cp $QT_PREFIX_PATH/lib/libicui18n.so.56 ./purk/lib
cp $QT_PREFIX_PATH/lib/libicuuc.so.56 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Core.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5DBus.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Gui.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Network.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5OpenGL.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Positioning.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5PrintSupport.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Qml.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Quick.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Sensors.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Sql.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5Widgets.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5WebEngine.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5WebEngineCore.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5WebEngineWidgets.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5WebChannel.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5XcbQpa.so.5 ./purk/lib
cp $QT_PREFIX_PATH/lib/libQt5QuickWidgets.so.5 ./purk/lib
cp $QT_PREFIX_PATH/libexec/QtWebEngineProcess ./purk
cp $QT_PREFIX_PATH/resources/qtwebengine_resources.pak ./purk
cp $QT_PREFIX_PATH/resources/qtwebengine_resources_100p.pak ./purk
cp $QT_PREFIX_PATH/resources/qtwebengine_resources_200p.pak ./purk
cp $QT_PREFIX_PATH/resources/icudtl.dat ./purk


cp -Rv src/purkd src/Purk src/simplewallet src/simpleminer src/connectivity_tool ./purk


tar -cjvf purk-linux-x64-$version_str.tar.bz2 purk
if [ $? -ne 0 ]; then
    echo "Failed to pack"
    exit 1
fi



echo "Build success"

exit 0
