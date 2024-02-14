@echo off
pushd %~dp0\..\

if exist Makefile (
    MinGW32-make

    if $? == 0 ( @REM No Error occured from the build process
        echo Compiled project successfully
    )
    
) else (
    echo Makefile not found. Be sure to run run_premake.bat before calling build.bat
)

popd
Pause