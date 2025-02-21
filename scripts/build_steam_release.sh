#!/bin/bash

# Abort on error
set -e

if [ ! -d lib/steamworks_c_wrapper/sdk ]; then
    echo "Steam SDK not present"
fi

if [ -z $STEAM_USR ]; then
    echo "error: \$STEAM_USR not set"
    exit 1
fi

if [ -z $STEAM_PWD ]; then
    echo "error: \$STEAM_PWD not set"
    exit 1
fi

if ! command -v steamcmd 2>&1 > /dev/null; then
    echo "error: steamcmd is not installed"
    exit 1
fi
if ! command -v unzip 2>&1 > /dev/null; then
    echo "error: unzip is not installed"
    exit 1
fi
if ! command -v cmake 2>&1 > /dev/null; then
    echo "error: cmake is not installed"
    exit 1
fi
if ! command -v cpack 2>&1 > /dev/null; then
    echo "error: cpack is not installed"
    exit 1
fi

STEAM_BUILD_DIR=build/steam-release

BUILD_DIR_LINUX=$STEAM_BUILD_DIR/linux
BUILD_DIR_WINDOWS=$STEAM_BUILD_DIR/windows


STEAM_CONTENT_DIR=$STEAM_BUILD_DIR/content
STEAM_CONTENT_DIR_WINDOWS=$STEAM_CONTENT_DIR/windows
STEAM_CONTENT_DIR_LINUX=$STEAM_CONTENT_DIR/linux

# Setup the depot dirs
mkdir -p $STEAM_CONTENT_DIR_LINUX
mkdir -p $STEAM_CONTENT_DIR_WINDOWS

# Build and package for linux
mkdir -p $BUILD_DIR_LINUX
cmake -B $BUILD_DIR_LINUX \
    -DCMAKE_BUILD_TYPE=Release \
    -GNinja
cmake --build $BUILD_DIR_LINUX
rm -rf $BUILD_DIR_LINUX/package/*
cpack --config $BUILD_DIR_LINUX/CPackConfig.cmake \
    -DCPACK_PACKAGE_DIRECTORY=$STEAM_CONTENT_DIR_LINUX \
    -G ZIP
rm -rf $STEAM_CONTENT_DIR_LINUX/*
unzip $BUILD_DIR_LINUX/package/*.zip -d $STEAM_CONTENT_DIR_LINUX/

# Build and package for windows
mkdir -p $BUILD_DIR_WINDOWS
cmake -B $BUILD_DIR_WINDOWS \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=build_deps/toolchains/mingw-w64-i686.cmake \
    -DSDLMIXER_VENDORED=ON \
    -DSDLTTF_VENDORED=ON \
    -GNinja
cmake --build $BUILD_DIR_WINDOWS
rm -rf $BUILD_DIR_WINDOWS/package/*
cpack --config $BUILD_DIR_WINDOWS/CPackConfig.cmake \
    -DCPACK_PACKAGE_DIRECTORY=$STEAM_CONTENT_DIR_WINDOWS \
    -G ZIP
rm -rf $STEAM_CONTENT_DIR_WINDOWS/*
unzip $BUILD_DIR_WINDOWS/package/*.zip -d $STEAM_CONTENT_DIR_WINDOWS/

# Setup steamworks depot build
cat << EOT >> $STEAM_BUILD_DIR/app_build.vdf
"AppBuild"
{
	"AppID" "931040" // your AppID
	"Desc" "Vagrant provider:Docker, Ubuntu 24.04" // internal description for this build
	"verbose" "0" // spew more build details in console
	"preview" "1" // make this a preview build only, nothing is uploaded

	"ContentRoot" "content\" // root content folder, relative to location of this file
	"BuildOutput" "output\" // build output folder for build logs and build cache files

	"Depots"
	{
		"931041" // DepotID (Windows)
		{
			"FileMapping"
			{
				"LocalPath" "windows/*" // all files from contentroot folder
				"DepotPath" "." // mapped into the root of the depot
				"recursive" "1" // include all subfolders
			}
		}
		"931042" // DepotID (Linux)
		{
			"FileMapping"
			{
				"LocalPath" "linux/*" // all files from contentroot folder
				"DepotPath" "." // mapped into the root of the depot
				"recursive" "1" // include all subfolders
			}
		}
	}
}
EOT

# Running the steam command build
steamcmd \
    +login $STEAM_USR $STEAM_PWD \
    +run_app_build $(pwd)/$STEAM_BUILD_DIR/app_build.vdf \
    +quit
