#!/bin/sh

mkdir -p _build/debug
mkdir -p _build/release

cd _build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
cd -

cd _build/release
cmake -DCMAKE_BUILD_TYPE=Release ../..
cd -
