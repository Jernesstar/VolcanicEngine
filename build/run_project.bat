@echo off
pushd %~dp0\..\

if exist .\bin\Sandbox.exe (
    .\bin\Sandbox.exe %*
) else (
    echo Cannot find Sandbox.exe. Make sure to call build.bat before calling run_project.bat
)

popd