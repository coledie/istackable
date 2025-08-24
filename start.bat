@echo off
echo Building and starting Stacklands Clone...
call build.bat
if %errorlevel% equ 0 (
    echo Starting game...
    .\main.exe
) else (
    echo Build failed, cannot start game.
    pause
)
