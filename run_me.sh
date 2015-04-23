#!/bin/bash

CUR_PATH=$(pwd)
rm -rf CMakeCache.txt
rm -rf CMakeFiles
mkdir build
cd build
cmake ../
make -j5
cp -rf lib* ../
cp -rf main ../
cd $CUR_PATH
rm -rf build
