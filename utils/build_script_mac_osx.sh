set -x #echo on

export PROJECT_ROOT="/Users/vshevchyk/projects/purk"
export QT_PATH="/Users/vshevchyk/Qt/5.5"
export LOCAL_BOOST_LIBS_PATH="/usr/local/opt/boost/lib"



cd "$PROJECT_ROOT"
if [ $? -ne 0 ]; then
    echo "Failed to cd purk"
    exit $?
fi

# git pull
# if [ $? -ne 0 ]; then
#     echo "Failed to git pull"
#     exit $?
# fi

#rm -rf build; mkdir -p build/release; cd build/release;
cd build/release;

cmake -D BUILD_GUI=TRUE -D CMAKE_PREFIX_PATH="$QT_PATH/clang_64" -D CMAKE_BUILD_TYPE=Release ../..
if [ $? -ne 0 ]; then
    echo "Failed to cmake"
    exit $?
fi

make -j Purk
if [ $? -ne 0 ]; then
    echo "Failed to make Purk"
    exit $?
fi

make -j connectivity_tool
if [ $? -ne 0 ]; then
    echo "Failed to make connectivity_tool"
    exit $?
fi

cd src/
if [ $? -ne 0 ]; then
    echo "Failed to cd src"
    exit $?
fi


# copy boost files
echo "Copying boost files...."
mkdir -p Purk.app/Contents/Frameworks/boost_libs
cp -R "$LOCAL_BOOST_LIBS_PATH/" Purk.app/Contents/Frameworks/boost_libs/
if [ $? -ne 0 ]; then
    echo "Failed to cp workaround to MacOS"
    exit 1
fi


# fix boost links
echo "Fixing boost library links...."
source ../../../utils/fix_boost_libs.sh
update_links_in_boost_binary @executable_path/../Frameworks/boost_libs Purk.app/Contents/MacOS/Purk
update_links_in_boost_libs @executable_path/../Frameworks/boost_libs Purk.app/Contents/Frameworks/boost_libs



$QT_PATH/clang_64/bin/macdeployqt Purk.app
if [ $? -ne 0 ]; then
    echo "Failed to macdeployqt Purk.app"
    exit $?
fi

cp -R ../../../src/gui/qt-daemon/html Purk.app/Contents/MacOS
if [ $? -ne 0 ]; then
    echo "Failed to cp html to MacOS"
    exit $?
fi

cp ../../../src/gui/qt-daemon/app.icns Purk.app/Contents/Resources
if [ $? -ne 0 ]; then
    echo "Failed to cp app.icns to resources"
    exit $?
fi


read version_str <<< $(DYLD_LIBRARY_PATH=$LOCAL_BOOST_LIBS_PATH ./connectivity_tool --version | awk '/^Purk / { print $2 }')
echo "Version built: $version_str"



echo "############### Prepearing archive... ################"
mkdir package_folder
if [ $? -ne 0 ]; then
    echo "Failed to zip app"
    exit 1
fi

ln -s /Applications package_folder/Applications 
if [ $? -ne 0 ]; then
    echo "Failed to copy applications link"
    exit 1
fi

mv Purk.app package_folder
if [ $? -ne 0 ]; then
    echo "Failed to top app package"
    exit 1
fi


hdiutil create -format UDZO -srcfolder package_folder -volname Purk "Purk-macos-x64-$version_str.dmg"
if [ $? -ne 0 ]; then
    echo "Failed to create dmg"
    exit 1
fi


#zip -r -y "purk-macos-x64-v0.2.0.zip" Purk.app
#if [ $? -ne 0 ]; then
#    echo "Failed to zip app"
#    exit $?
# fi

cd ../..
echo "Build success"

