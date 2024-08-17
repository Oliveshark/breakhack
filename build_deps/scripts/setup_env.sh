#!/bin/sh

BUILD_DIR=build

mkdir -p $BUILD_DIR/debug
mkdir -p $BUILD_DIR/release

cd $BUILD_DIR/debug
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ../..
cd -

cd $BUILD_DIR/release
cmake -DCMAKE_BUILD_TYPE=Release -GNinja ../..
cd -
