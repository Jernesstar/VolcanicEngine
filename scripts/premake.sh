#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu" ]]; then
	vendor/premake/bin/Linux/premake5.exe gmake2
elif [[ "$OSTYPE" == "msys" ]]; then
	vendor/premake/bin/Windows/premake5.exe gmake2 $1
fi
