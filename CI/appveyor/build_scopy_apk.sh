#!/bin/bash
set -xe
source ./android_toolchain.sh $1 $2

ARTIFACT_LOCATION=$GITHUB_WORKSPACE

build_scopy() {

	git clone $REPO_URL
	pushd scopy
	git submodule update --init --recursive iio-emu

	git fetch origin $BRANCH
	git checkout FETCH_HEAD

	rm -rf build*

	cp $BUILD_STATUS_FILE .
	cp $BUILD_ROOT/android_cmake.sh .
	cp $SCRIPT_HOME_DIR/android_deploy_qt.sh .

	./android_cmake.sh .

	cd $BUILDDIR
	make -j$JOBS
	cd ..

	./android_deploy_qt.sh
	cd $BUILDDIR
	make apk
	make aab
	cd ..
	
	popd
}

move_artifact() {
	pushd scopy
	sudo cp ./$BUILDDIR/android-build/build/outputs/apk/debug/android-build-debug.apk $ARTIFACT_LOCATION/
	sudo cp ./$BUILDDIR/android-build/build/outputs/bundle/debug/android-build-debug.aab $ARTIFACT_LOCATION/
	sudo cp ./$BUILDDIR/android-build/build/outputs/bundle/release/android-build-release.aab $ARTIFACT_LOCATION/
	pushd $ARTIFACT_LOCATION
	sudo chmod 644 ./android-build-debug.apk
	sudo chmod 644 ./android-build-debug.aab
	sudo chmod 644 ./android-build-release.aab
	ls -la $ARTIFACT_LOCATION
	popd

	popd
}

build_scopy
move_artifact
