@ECHO OFF
SETLOCAL
CALL __set_python_path.bat
CD amd64
IF EXIST python.exe (
  .\python.exe %*
) ELSE (
  .\python_d.exe %*
)
