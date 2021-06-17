#!/usr/bin/bash.exe

set -e

export PATH=/bin:/usr/bin:/${MINGW_VERSION}/bin:/c/Program\ Files/Git/cmd:/c/Windows/System32:/c/Program\ Files/7-Zip:/c/Program\ Files\ \(x86\)/Inno\ Setup\ \6:/c/Program\ Files/Appveyor/BuildAgent
echo $PATH
appveyor AddMessage "1. Starting build_appveyor_mingw.sw"
RC_COMPILER_OPT="-DCMAKE_RC_COMPILER=/c/msys64/${MINGW_VERSION}/bin/windres.exe"

WORKDIR=${PWD}
echo BUILD_NO $BUILD_NO
JOBS=$(nproc)


wget http://swdownloads.analog.com/cse/m1k/drivers/dpinst.zip
wget http://swdownloads.analog.com/cse/m1k/drivers/dfu-util.zip
7z x -y "dpinst.zip" -o"/c/dpinst"
7z x -y "dfu-util.zip" -o"/c/dfu-util"

# github release upload tool install
wget https://github.com/tcnksm/ghr/releases/download/v0.13.0/ghr_v0.13.0_windows_amd64.zip
7z x -y ghr_v0.13.0_windows_amd64.zip -o"/c/ghr"
UPLOAD_TOOL=/c/ghr/ghr_v0.13.0_windows_amd64/ghr.exe


CC=/${MINGW_VERSION}/bin/${ARCH}-w64-mingw32-gcc.exe
CXX=/${MINGW_VERSION}/bin/${ARCH}-w64-mingw32-g++.exe
CMAKE_OPTS="\
	-DCMAKE_C_COMPILER:FILEPATH=${CC} \
	-DCMAKE_CXX_COMPILER:FILEPATH=${CXX} \
	-DPKG_CONFIG_EXECUTABLE=/$MINGW_VERSION/bin/pkg-config.exe \
	-DCMAKE_CXX_FLAGS="-fno-omit-frame-pointer" \
	-DCMAKE_PREFIX_PATH=/c/msys64/$MINGW_VERSION/lib/cmake \
	-DCMAKE_BUILD_TYPE=RelWithDebInfo \
	"

SCOPY_CMAKE_OPTS="\
	$RC_COMPILER_OPT \
	-DBREAKPAD_HANDLER=ON \
	-DWITH_DOC=ON \
	-DPYTHON_EXECUTABLE=/$MINGW_VERSION/bin/python3.exe \
	"

#PYTHON_LOCATION=/$MINGW_VERSION/lib/python3.8
#PYTHON_FILES="${PYTHON_LOCATION}/*.py ${PYTHON_LOCATION}/asyncio ${PYTHON_LOCATION}/collections ${PYTHON_LOCATION}/concurrent ${PYTHON_LOCATION}/config-3.* ${PYTHON_LOCATION}/ctypes ${PYTHON_LOCATION}/distutils ${PYTHON_LOCATION}/encodings ${PYTHON_LOCATION}/lib-dynload ${PYTHON_LOCATION}/site-packages"
PYTHON_FILES=/$MINGW_VERSION/lib/python3.*

DLL_DEPS=$(cat ${WORKDIR}/CI/appveyor/mingw_dll_deps)
DLL_DEPS="$DLL_DEPS $PYTHON_FILES"

echo $DLL_DEPS

OLD_PATH=$PATH
DEST_FOLDER=scopy_$ARCH_BIT
BUILD_FOLDER=build_$ARCH_BIT
DEBUG_FOLDER=debug_$ARCH_BIT

appveyor AddMessage "2. Installing msys deps.sw"
cd /c
source ${WORKDIR}/CI/appveyor/install_msys_deps.sh
appveyor AddMessage "3. Installed msys deps.sw"

# Download a 32-bit version of windres.exe
cd ${WORKDIR}
wget http://swdownloads.analog.com/cse/build/windres.exe.gz
gunzip windres.exe.gz


echo "### Building Scopy ..."
appveyor AddMessage "4. Running CMake"
/$MINGW_VERSION/bin/python3.exe --version
mkdir /c/$BUILD_FOLDER
cd /c/$BUILD_FOLDER
cp /tmp/scopy-mingw-build-status /c/projects/scopy
cat /c/projects/scopy/scopy-mingw-build-status
cmake -G 'Unix Makefiles' $SCOPY_CMAKE_OPTS $CMAKE_OPTS /c/projects/scopy
appveyor AddMessage "5. Build configured"

cat /c/$BUILD_FOLDER/buildinfo.html
appveyor PushArtifact /c/$BUILD_FOLDER/buildinfo.html

appveyor AddMessage "6. Starting build"
cd /c/$BUILD_FOLDER/resources
sed -i  's/^\(FILEVERSION .*\)$/\1,'$BUILD_NO'/' properties.rc
cat properties.rc
cd /c/build_$ARCH_BIT && make -j $JOBS
appveyor AddMessage "7. Build finished"


echo "### Deploy the application (copy the dependencies) ..."
appveyor AddMessage "8. Deploying Scopy"
mkdir /c/$DEST_FOLDER
cp /c/$BUILD_FOLDER/Scopy.exe /c/$DEST_FOLDER/
cp /c/$BUILD_FOLDER/qt.conf /c/$DEST_FOLDER/
mkdir /c/$DEST_FOLDER/resources

cp /c/$BUILD_FOLDER/iio-emu/iio-emu.exe /c/$DEST_FOLDER/

# windeployqt was broken in qt version 5.14.2 - it should be fixed in Qt 5.15 - https://bugreports.qt.io/browse/QTBUG-80763
/c/msys64/$MINGW_VERSION/bin/windeployqt.exe --dir /c/$DEST_FOLDER --no-system-d3d-compiler --no-compiler-runtime --no-quick-import --opengl --printsupport /c/$BUILD_FOLDER/Scopy.exe
cp -r /$MINGW_VERSION/share/libsigrokdecode/decoders  /c/$DEST_FOLDER/


#tar -C /c/$DEST_FOLDER --strip-components=3 -xJf /c/scopy-$MINGW_VERSION-build-deps.tar.xz msys64/$MINGW_VERSION/bin
cd /$MINGW_VERSION/bin ;
cp -r $DLL_DEPS /c/$DEST_FOLDER/

echo "### Extracting debug symbols ..."
mkdir /c/scopy_$ARCH_BIT/.debug
#/$MINGW_VERSION/bin/objcopy -v --only-keep-debug /c/$DEST_FOLDER/Scopy.exe /c/$DEST_FOLDER/.debug/Scopy.exe.debug
dump_syms -r /c/$DEST_FOLDER/Scopy.exe > /c/$DEST_FOLDER/Scopy.exe.sym
#/c/msys64/$MINGW_VERSION/bin/strip.exe --strip-debug --strip-unneeded /c/$DEST_FOLDER/Scopy.exe
#/c/msys64/$MINGW_VERSION/bin/strip.exe --strip-debug --strip-unneeded /c/$DEST_FOLDER/*.dll
#/c/msys64/$MINGW_VERSION/bin/objcopy.exe -v --add-gnu-debuglink=/c/$DEST_FOLDER/.debug/Scopy.exe.debug /c/$DEST_FOLDER/Scopy.exe
mkdir /c/$DEBUG_FOLDER
mv /c/$DEST_FOLDER/Scopy.exe.sym /c/$DEBUG_FOLDER
mv /c/$DEST_FOLDER/.debug /c/$DEBUG_FOLDER
/c/cv2pdb/cv2pdb /c/$DEST_FOLDER/Scopy.exe
/c/cv2pdb/cv2pdb /c/$DEST_FOLDER/libm2k.dll
/c/cv2pdb/cv2pdb /c/$DEST_FOLDER/libiio.dll
/c/cv2pdb/cv2pdb /c/$DEST_FOLDER/libgnuradio-m2k.dll
/c/cv2pdb/cv2pdb /c/$DEST_FOLDER/libgnuradio-scopy.dll
cp -R /c/projects/scopy /c/$DEBUG_FOLDER/scopy
mv /c/$DEST_FOLDER/*.pdb /c/$DEBUG_FOLDER

cp -r /c/projects/scopy/drivers /c/$DEST_FOLDER
if [[ $ARCH_BIT == "64" ]]; then
	cp /c/dfu-util/dfu-util-static-amd64.exe /c/$DEST_FOLDER/drivers/dfu-util.exe
	cp /c/dpinst/dpinst_amd64.exe /c/$DEST_FOLDER/drivers/dpinst.exe
else
	cp /c/dfu-util/dfu-util-static.exe /c/$DEST_FOLDER/drivers/dfu-util.exe
	cp /c/dpinst/dpinst.exe /c/$DEST_FOLDER/drivers/dpinst.exe
fi

appveyor AddMessage "9. Scopy succesfully deployed"

echo "### Creating archives ... "
appveyor AddMessage "10. Creating archives"
7z a "/c/scopy-${ARCH_BIT}bit.zip" /c/$DEST_FOLDER
appveyor PushArtifact /c/scopy-${ARCH_BIT}bit.zip
7z a "/c/debug-${ARCH_BIT}bit.zip" /c/$DEBUG_FOLDER
appveyor PushArtifact /c/debug-${ARCH_BIT}bit.zip
appveyor AddMessage "11. Creating installer"
iscc //Qp /c/$BUILD_FOLDER/scopy-$ARCH_BIT.iss
appveyor PushArtifact $DEPLOY_FILE
appveyor AddMessage "12. Job complete"

if [ "$APPVEYOR_REPO_BRANCH" = "master" ]; then
	echo Identified master branch
	if [ -z "$APPVEYOR_PULL_REQUEST_NUMBER" ]; then
		echo Not a pull request
		mkdir /c/to_deploy
		cp /c/scopy-${ARCH_BIT}bit.zip /c/to_deploy
		cp /c/debug-${ARCH_BIT}bit.zip /c/to_deploy
		cp $DEPLOY_FILE /c/to_deploy
		$UPLOAD_TOOL -u $APPVEYOR_ACCOUNT_NAME -r $APPVEYOR_PROJECT_NAME -name "Continuous build" -b "Latest succesful master build " -prerelease -debug -replace continous /c/to_deploy
	fi
fi

