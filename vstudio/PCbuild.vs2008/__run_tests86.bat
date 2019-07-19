SETLOCAL
CALL __set_python_path.bat
CD Win32
IF EXIST python.exe ( SET "TESTPYTHON=python.exe" ) ELSE ( SET "TESTPYTHON=python_d.exe" )
SET "TESTPYTHON=..\vstudio\PCbuild.vs2008\Win32\%TESTPYTHON%"
CD ..\..\..\cpython
%TESTPYTHON% Tools\scripts\run_tests.py
