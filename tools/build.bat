@echo off
rem ---------------------------------------------------------------------------
rem  Build Argus on Windows.
rem
rem   build.bat                 configure and build, release, with inference
rem   build.bat debug           debug build
rem   build.bat package         build, then assemble the self-contained dist\
rem   build.bat clean           rebuild from scratch
rem   build.bat no-inference    skip ONNX Runtime and OpenCV entirely
rem
rem  Missing dependencies are downloaded automatically; see fetch-deps.ps1.
rem  Override the search paths with QTDIR, ORT_ROOT or OPENCV_DIR.
rem
rem  Written with labels rather than parenthesised blocks on purpose: batch
rem  expands a whole block before running it, so a path containing ")" — such as
rem  %ProgramFiles(x86)% — closes the block early and the error you get is
rem  "\Microsoft was unexpected at this time".
rem ---------------------------------------------------------------------------
setlocal

set "ROOT=%~dp0.."
set "PRESET=msvc-release"
set "INFERENCE=ON"
set "DO_PACKAGE=0"
set "DO_CLEAN=0"

:parse
if "%~1"=="" goto parsed
if /I "%~1"=="debug"        set "PRESET=msvc-debug"
if /I "%~1"=="package"      set "DO_PACKAGE=1"
if /I "%~1"=="clean"        set "DO_CLEAN=1"
if /I "%~1"=="no-inference" set "INFERENCE=OFF"
shift
goto parse
:parsed

rem --- Visual Studio -----------------------------------------------------------
if defined VCINSTALLDIR goto have_vs

rem Locating Visual Studio is delegated to PowerShell, which is a hard
rem dependency anyway for fetch-deps.ps1. Doing it in batch means quoting a path
rem that contains both spaces and parentheses — %ProgramFiles(x86)% — through
rem `for /f`, which hands the command to a nested cmd and mangles it. The
rem symptom is "'vswhere.exe' is not recognized" while the file plainly exists.
set "VSPATH="
set "VSTMP=%TEMP%\argus_vs.txt"
if exist "%VSTMP%" del "%VSTMP%" >nul 2>&1
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0find-vs.ps1" -OutFile "%VSTMP%" >nul 2>&1
if not exist "%VSTMP%" goto no_vs
set /p VSPATH=<"%VSTMP%"
del "%VSTMP%" >nul 2>&1
if not defined VSPATH goto no_vs
if not exist "%VSPATH%\VC\Auxiliary\Build\vcvars64.bat" goto no_vs

call "%VSPATH%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 goto no_vs
goto have_vs

:no_vs
echo ERROR: no Visual Studio with the C++ toolset was found.
echo        Install "Desktop development with C++", or run this from a
echo        Developer Command Prompt.
exit /b 1

:have_vs

rem --- Qt ------------------------------------------------------------------------
if defined QTDIR goto have_qt
for /d %%v in ("C:\Qt\6.*") do if exist "%%v\msvc2022_64\lib\cmake\Qt6" set "QTDIR=%%v\msvc2022_64"
if defined QTDIR goto have_qt
echo ERROR: Qt 6 not found. Set QTDIR to the kit directory, for example:
echo            set QTDIR=C:\Qt\6.8.2\msvc2022_64
exit /b 1
:have_qt

rem --- optional dependencies -------------------------------------------------------
if not defined ORT_ROOT   set "ORT_ROOT=%ROOT%\third_party\onnxruntime"
if not defined OPENCV_DIR set "OPENCV_DIR=%ROOT%\third_party\opencv\build"

if "%INFERENCE%"=="OFF" goto deps_ready
if exist "%ORT_ROOT%\include\onnxruntime_cxx_api.h" goto deps_ready

echo ONNX Runtime not found - downloading dependencies
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0fetch-deps.ps1"
if errorlevel 1 exit /b 1
if exist "%ORT_ROOT%\include\onnxruntime_cxx_api.h" goto deps_ready

echo ERROR: still no ONNX Runtime at %ORT_ROOT%
echo        Build without it: build.bat no-inference
exit /b 1

:deps_ready

echo Qt        : %QTDIR%
echo preset    : %PRESET%
echo inference : %INFERENCE%
echo.

if "%DO_CLEAN%"=="0" goto configure
if exist "%ROOT%\build\%PRESET%" rmdir /s /q "%ROOT%\build\%PRESET%"

:configure
cmake -S "%ROOT%" --preset %PRESET% ^
      -DCMAKE_PREFIX_PATH="%QTDIR%" ^
      -DARGUS_WITH_INFERENCE=%INFERENCE% ^
      -DARGUS_ORT_ROOT="%ORT_ROOT%" ^
      -DARGUS_OPENCV_DIR="%OPENCV_DIR%"
if errorlevel 1 exit /b 1

if "%DO_PACKAGE%"=="1" goto build_install
cmake --build --preset %PRESET%
if errorlevel 1 exit /b 1
echo.
echo Binaries: %ROOT%\build\%PRESET%\bin
echo Add %QTDIR%\bin to PATH to run them, or use: build.bat package
goto done

:build_install
cmake --build --preset %PRESET% --target install
if errorlevel 1 exit /b 1
echo.
echo Package: %ROOT%\dist

:done
endlocal
