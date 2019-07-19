@ECHO OFF
SETLOCAL
CALL __set_python_path.bat

SET TARGETDIR=%1
IF "%TARGETDIR%" == "amd64" (
  SET "PLATFORM_NAME=x64"
) ELSE (
  SET "PLATFORM_NAME=Win32"
)
CD "%TARGETDIR%"
IF EXIST python.exe (
  SET EXESUFFIX=
  SET "INTDIR=..\%PLATFORM_NAME%-temp-Release"
) ELSE (
  SET EXESUFFIX=_d
  SET "INTDIR=..\%PLATFORM_NAME%-temp-Debug"
)
SET "INTDIR=%INTDIR%\_freeze_importlib"
ECHO ON

_freeze_importlib%EXESUFFIX%.exe ..\..\..\cpython\Lib\importlib\_bootstrap.py "%INTDIR%\importlib.g.h"

@ECHO OFF
fc "..\..\..\cpython\Python\importlib.h" "%INTDIR%\importlib.g.h" >NUL
IF ERRORLEVEL 1 (
  msg "%username%" Python\importlib.h is different from %INTDIR%\_freeze_importlib\importlib.g.h - please update importlib.h and rebuild!
  EXIT /B 1
)
