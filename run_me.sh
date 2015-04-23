#!/bin/bash

CUR_PATH=$(pwd)
mkdir build
cd build
cmake ../
make -j5
cp -rf lib* ../
cp -rf main ../
cd $CUR_PATH
rm -rf build
