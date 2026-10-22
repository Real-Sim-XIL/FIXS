@echo off
REM Build + run the #174 simulator-free guard (no CarMaker, no Carla, no server).
REM
REM Every check below is "neq 0", never "if errorlevel 1". A failed assert() in
REM these programs does not exit 3: the CRT ends the process with 0xC0000409, which
REM cmd reports as -1073740791, and "if errorlevel 1" means ">= 1" -- so it never
REM fired. Measured: replay_core.exe failed its own transcript assertion and this
REM script carried on to the next step, and would have printed ALL ... PASSED.
setlocal
cd /d "%~dp0"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" ( echo ERROR: vswhere not found; install VS 2022. & exit /b 1 )
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -property installationPath`) do set "VSPATH=%%i"
call "%VSPATH%\VC\Auxiliary\Build\vcvars64.bat" >nul

set "CLIB=..\..\CommonLib"
set "INC=/I%CLIB% /I%CLIB%\yaml-cpp\include /I%CLIB%\yaml-cpp\build\include"
set "YAML=%CLIB%\yaml-cpp\build\Release\yaml-cpp.lib"

echo === smoke_interface (interface only, SDK-free) ===
cl /std:c++17 /EHsc /MD /nologo /W3 /DWIN32 /D_CRT_SECURE_NO_WARNINGS smoke_interface.cpp /Fe:smoke_interface.exe
if %ERRORLEVEL% neq 0 ( echo SMOKE BUILD_FAILED & exit /b 1 )
.\smoke_interface.exe
if %ERRORLEVEL% neq 0 ( echo SMOKE RUN_FAILED & exit /b 1 )

echo.
echo === replay_core (VirEnvCore + mock, SDK-free) ===
if not exist "%YAML%" ( echo REPLAY BUILD_FAILED: %YAML% not built - run scripts\dispatch\1_external_libraries.bat & exit /b 1 )
cl /std:c++17 /EHsc /MD /nologo /W3 /DWIN32 /D_CRT_SECURE_NO_WARNINGS %INC% replay_core.cpp "%CLIB%\VirEnvCore.cpp" "%CLIB%\MsgHelper.cpp" "%CLIB%\SocketHelper.cpp" "%CLIB%\ConfigHelper.cpp" /Fe:replay_core.exe /link "%YAML%" ws2_32.lib
if %ERRORLEVEL% neq 0 ( echo REPLAY BUILD_FAILED & exit /b 1 )
.\replay_core.exe
if %ERRORLEVEL% neq 0 ( echo REPLAY RUN_FAILED & exit /b 1 )

echo.
echo === blueprint_pick (the C++ blueprint choice, for test_blueprint_parity) ===
cl /std:c++17 /EHsc /MD /nologo /W4 /I%CLIB% blueprint_pick.cpp /Fe:blueprint_pick.exe
if %ERRORLEVEL% neq 0 ( echo BLUEPRINT BUILD_FAILED & exit /b 1 )
.\blueprint_pick.exe >nul
if %ERRORLEVEL% neq 0 ( echo BLUEPRINT RUN_FAILED & exit /b 1 )

echo.
echo === view_dump (the C++ ViewSetup parse, for test_view_setup_parity) ===
cl /std:c++17 /EHsc /MD /nologo /W3 /DWIN32 /D_CRT_SECURE_NO_WARNINGS %INC% view_dump.cpp "%CLIB%\ConfigHelper.cpp" /Fe:view_dump.exe /link "%YAML%" ws2_32.lib
if %ERRORLEVEL% neq 0 ( echo VIEWDUMP BUILD_FAILED & exit /b 1 )

echo.
echo ALL VIRENVCORE GUARD TESTS PASSED
endlocal
