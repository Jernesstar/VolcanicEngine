@echo off

pushd %~dp0\..\

set arg=%1
if "%arg%"=="make" set res=true
if "%arg%"=="vs" set res=true

if NOT defined res (
    echo Target '%arg%' is not supported
    exit 2
)

set target=""
set print_target=""

if "%arg%"=="make" (
    set "target=gmake2"
    set "print_target=GNU Make"
)
if "%arg%"=="vs" (
    set "target=vs2022"
    set "print_target=Visual Studio 2022"
)

echo ----- Generating files for %print_target% -----
vendor\premake\bin\Windows\premake5.exe %target%
echo --------------------------------

popd
Pause