#!/bin/bash

cd build || exit 1
rm -rf *.png
make -j4


