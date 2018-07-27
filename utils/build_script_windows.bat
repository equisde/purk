SET QT_PREFIX_PATH=C:\Qt\5.5\5.5\msvc2013_64
SET INNOSETUP_PATH=C:\Program Files (x86)\Inno Setup 5\ISCC.exe
SET QT_BINARIES_PATH=C:\home\projects\binaries\qt-daemon
SET ACHIVE_NAME_PREFIX=purk-win-x64-
SET BUILDS_PATH=C:\home\deploy\purk
SET SOURCES_PATH=C:\home\purk
set BOOST_ROOT=C:\boost\boost_1_56_0
set BOOST_LIBRARYDIR=C:\boost\boost_1_56_0\lib64-msvc-12.0
set EXTRA_FILES_PATH=C:\home\deploy\purk\extra_files
set SSL_PATH_0_9_8=C:\home\libs

cd %SOURCES_PATH%

::git pull

::IF %ERRORLEVEL% NEQ 0 (
::  goto error
::)
::echo on

@echo "---------------- BUILDING CONSOLE APPLICATIONS ----------------"
@echo "---------------------------------------------------------------"

setLocal 

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

IF "%1"=="skip_build" GOTO skip_build

::rmdir build /s /q
::mkdir build
cd build
cmake -D CMAKE_PREFIX_PATH="%QT_PREFIX_PATH%" -D BUILD_GUI=TRUE -D STATIC=FALSE -G "Visual Studio 12 Win64" ..
IF %ERRORLEVEL% NEQ 0 (
  goto error
)


msbuild version.vcxproj /p:SubSystem="CONSOLE,5.02"  /p:Configuration=Release /t:Build
IF %ERRORLEVEL% NEQ 0 (
  goto error
)

msbuild src/daemon.vcxproj /p:SubSystem="CONSOLE,5.02"  /p:Configuration=Release /t:Build
IF %ERRORLEVEL% NEQ 0 (
  goto error
)

msbuild src/simplewallet.vcxproj /p:SubSystem="CONSOLE,5.02"  /p:Configuration=Release /t:Build
IF %ERRORLEVEL% NEQ 0 (
  goto error
)

msbuild src/Purk.vcxproj /p:SubSystem="WINDOWS,5.02" /p:Configuration=Release /t:Build

IF %ERRORLEVEL% NEQ 0 (
  goto error
)

:skip_build

cd %SOURCES_PATH%\build\src\Release

@echo "Version: "

set cmd=simplewallet.exe --version
FOR /F "tokens=3" %%a IN ('%cmd%') DO set version=%%a  
set version=%version:~0,-2%
echo '%version%'




mkdir bunch
copy /Y Purk.exe bunch
copy /Y purkd.exe bunch
copy /Y simplewallet.exe bunch

copy /Y %SSL_PATH_0_9_8%\libeay32.dll bunch
copy /Y %SSL_PATH_0_9_8%\ssleay32.dll bunch

%QT_PREFIX_PATH%\bin\windeployqt.exe bunch\Purk.exe


cd bunch

zip -r %BUILDS_PATH%\builds\%ACHIVE_NAME_PREFIX%%version%.zip *.*
IF %ERRORLEVEL% NEQ 0 (
  goto error
)


@echo "Add html"


cd %SOURCES_PATH%\src\gui\qt-daemon\
zip -r %BUILDS_PATH%\builds\%ACHIVE_NAME_PREFIX%%version%.zip html
IF %ERRORLEVEL% NEQ 0 (
  goto error
)

@echo "Other stuff"

cd %ETC_BINARIES_PATH%
zip -r %BUILDS_PATH%\builds\%ACHIVE_NAME_PREFIX%%version%.zip *.*
IF %ERRORLEVEL% NEQ 0 (
  goto error
)


cd %SOURCES_PATH%\build
IF %ERRORLEVEL% NEQ 0 (
  goto error
)


@echo "---------------------------------------------------------------"
@echo "-------------------Building installer--------------------------"
@echo "---------------------------------------------------------------"

mkdir installer_src

unzip %BUILDS_PATH%\builds\%ACHIVE_NAME_PREFIX%%version%.zip -d installer_src
IF %ERRORLEVEL% NEQ 0 (
  goto error
)


"%INNOSETUP_PATH%"  /dBinariesPath=../build/installer_src /DMyAppVersion=%version% /o%BUILDS_PATH%\builds\ /f%ACHIVE_NAME_PREFIX%%version%-installer ..\utils\setup.iss 
IF %ERRORLEVEL% NEQ 0 (
  goto error
)


@echo "---------------------------------------------------------------"
@echo "---------------------------------------------------------------"



goto success

:error
echo "BUILD FAILED"
exit /B %ERRORLEVEL%

:success
echo "BUILD SUCCESS"

pause



