#!/bin/bash

clear
mkdir -p build
find build/ -mindepth 1 -maxdepth 1 -not -path build/_deps -exec rm -rf {} \;
cd build
cmake ../
make -j4 || exit 1
echo "Build done at $(date)"
