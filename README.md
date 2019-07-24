This is the ‘CPython89’ project, a series of patches designed to re-establish ANSI X3.159-1989 compatibility for the ‘CPython’ reference immplementation of the Python programming language.

As such I don't claim any copyright on the source code in this repository. In fact, most of its contents are included via (git) submodule references, as follows:

Project | Description
--- | ---
./cpython                     | The Python programming language
./externals/_github/openssl   | TLS/SSL and crypto library
./externals/_github/tcltk/tcl | Tcl/Tk – The Tcl Core
./externals/_github/tcltk/tk  | Tcl/Tk – The Tk Widget Toolkit For Tcl
./externals/_github/xz        | Free general-purpose data compression software with high compression ratio
./externals/_github/zlib      | A massively spiffy yet delicately unobtrusive compression library

As of version v3.5.0 the ‘CPython’ source code requires a ISO/IEC 9899:1999 compatible compiler for successful compilation.

For people who don't have the option – or do not want – to upgrade to a C99 compatible compiler, this decision might be unfortunate. To this end, this project aims to provide adapted source code files (along with upgraded VS9 project files) to allow for compilation with Microsoft's Visual Studio 2008 IDE.

The achievement of this goal requires several changes to the Python's source code files, all of which can be found under the ./vstudio directory, which – with the exception of the ‘PCbuild.vs2008’ project folder – replicates CPython's source tree structure.

While the code transition back to C89 has been attempted with much care, some words of caution are in order: the CRT library shipped with later Visual C++ releases is (usually) call interface compatible. However, the semantics of several functions might be enhanced. As a prominent example, ‘strftime’ comes to mind. If invoked with an unrecognized formatting code, the ‘invalid parameter handler’ is invoked (which usually abort()s the programs execution).

While it might be possible to allow for these API changes, and provide a compatibility layer for VC9, this is not (currently) done. Therefore, not all of Python's test suite will successfully run to completion.

In light of the last paragraph, the resulting binaries should not be viewed as ‘production-ready’ – in any case, Python scripts to be executed under (any) self-compiled Python interpreter should always be tested with extra care.

On a more positive note, anyone with experience in compiling Python 3.4 (or earlier) should feel right at home. Just open the following solution file,

./PCbuild.vs2008/pcbuild.VS2008.sln

change the selection of active Projects in the ‘Batch build’ dialog, and you're set to go¹. One last caveat applies, though: only the ‘Debug’ and ‘Release’ configuration types have been configured (and tested).  Profile-guided optimization (PGO) builds really only make sense with newer – and officially supported – versions of Visual Studio.

¹ **Windows SDK 6.0A** users beware: due to a bug, the following file defines a NTDDI_LONGHORN, <br />
&nbsp;&nbsp;but no NTDDI_VISTA constant:

* "%ProgramFiles(x86)%\Microsoft SDKs\Windows\v6.0A\Include\sdkddkver.h"

&nbsp;&nbsp;For successful compilation, a ‘#define NTDDI_VISTA 0x06000000’ line needs to be added!
