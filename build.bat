@echo off

REM Check if debug flag is passed
set DEBUG_FLAG=
if "%1"=="debug" (
    set DEBUG_FLAG=-DDEBUG_MODE
    echo Building Stacklands Clone with SDL3 ^(DEBUG MODE^)...
) else (
    echo Building Stacklands Clone with SDL3...
)

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

g++ -g -std=c++17 %DEBUG_FLAG% main.cpp src/card.cpp src/board.cpp src/game.cpp src/color_manager.cpp src/design_manager.cpp -o build/main.exe -Iinclude -Llib -lSDL3 -lopengl32 -lglu32
if %errorlevel% equ 0 (
    echo Build successful!
    echo Copying SDL3.dll to build folder...
    copy bin\SDL3.dll build\ >nul 2>&1
    echo Ready to run! Use run.bat or build\main.exe
) else (
    echo Build failed!
    exit /b 1
)
