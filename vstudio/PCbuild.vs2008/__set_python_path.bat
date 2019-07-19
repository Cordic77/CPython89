SET PYTHONHOME=
: Resolve absolute path from relative path and/or file name?
: https://stackoverflow.com/questions/1645843/resolve-absolute-path-from-relative-path-and-or-file-name#answer-10018990
FOR /F %%i IN ("..\..\cpython\Lib") DO SET CPYTHON_LIB=%%~fi
SET PYTHONPATH=%CPYTHON_LIB%;%PYTHONPATH%
