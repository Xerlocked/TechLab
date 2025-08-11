@echo off
echo ==========================
echo Are you sure you want to delete the .bin file?
echo 1. .obj.bin
echo 2. .fbx.bin
echo 3. All .bin files
echo ==========================
set /p choice="Choose (Input 1 or 2 or 3): "

if "%choice%"=="1" (
    echo .obj.bin files delete...
    del /s /q "%~dp0*.obj.bin"
    echo Complete
) else if "%choice%"=="2" (
    echo .fbx.bin files delete...
    del /s /q "%~dp0*.fbx.bin"
    echo Complete
) else if "%choice%"=="3" (
    echo all .bin files delete...
    del /s /q "%~dp0*.bin"
    echo Complete
) else (
    echo wrong, exit
)

pause