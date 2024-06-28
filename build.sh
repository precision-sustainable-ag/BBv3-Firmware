#!/bin/bash

CUR_DIR_TEMP=$(pwd)
if [ -d "./build" ]; then
	cd build && rm -r *
else
	mkdir build && cd build
fi
cmake ../
make
cd "$CUR_DIR_TEMP"
