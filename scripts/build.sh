#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu" ]]; then
	cd build/
	make -f Makefile
elif [[ "$OSTYPE" == "msys" ]]; then
	cd build/
	mingw32-make.exe -f Makefile
fi
