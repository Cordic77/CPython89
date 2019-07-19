#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <io.h>

#define CMD_SIZE 500

#define CPYTHON_GIT_REPOSITORY

/* This file creates the getbuildinfo.o object, by first
   invoking subwcrev.exe (if found), and then invoking cl.exe.
   As a side effect, it might generate PCBuild\getbuildinfo2.c
   also. If this isn't a subversion checkout, or subwcrev isn't
   found, it compiles ..\\Modules\\getbuildinfo.c instead.

   Currently, subwcrev.exe is found from the registry entries
   of TortoiseSVN.

   No attempt is made to place getbuildinfo.o into the proper
   binary directory. This isn't necessary, as this tool is
   invoked as a pre-link step for pythoncore, so that overwrites
   any previous getbuildinfo.o.

   However, if a second argument is provided, this will be used
   as a temporary directory where any getbuildinfo2.c and
   getbuildinfo.o files are put.  This is useful if multiple
   configurations are being built in parallel, to avoid them
   trampling each other's files.

*/

int make_buildinfo2(const char *tmppath)
{
    struct _stat st;
    HKEY hTortoise;
    char command[CMD_SIZE+1];
    DWORD type, size;
    if (_stat(".svn", &st) < 0)
        return 0;
    /* Allow suppression of subwcrev.exe invocation if a no_subwcrev file is present. */
    if (_stat("no_subwcrev", &st) == 0)
        return 0;
    if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\TortoiseSVN", &hTortoise) != ERROR_SUCCESS &&
        RegOpenKey(HKEY_CURRENT_USER, "Software\\TortoiseSVN", &hTortoise) != ERROR_SUCCESS)
        /* Tortoise not installed */
        return 0;
    command[0] = '"';  /* quote the path to the executable */
    size = sizeof(command) - 1;
    if (RegQueryValueEx(hTortoise, "Directory", 0, &type, command+1, &size) != ERROR_SUCCESS ||
        type != REG_SZ)
    {
        /* Registry corrupted */
        CloseHandle(hTortoise);
        return 0;
    }
    CloseHandle(hTortoise);
    strcat_s(command, CMD_SIZE, "bin\\subwcrev.exe");
    if (_stat(command+1, &st) < 0)
        /* subwcrev.exe not part of the release */
        return 0;
    strcat_s(command, CMD_SIZE, "\" ..\\..\\cpython ..\\..\\cpython\\Modules\\getbuildinfo.c \"");
    strcat_s(command, CMD_SIZE, tmppath); /* quoted tmppath */
    strcat_s(command, CMD_SIZE, "getbuildinfo2.c\"");

    puts(command); fflush(stdout);
    if (system(command) < 0)
        return 0;
    return 1;
}

const char DELIMS[] = { " \n" };

#if defined(CPYTHON_GIT_REPOSITORY)
int get_git_info(char * gitbranch, char * gitversion, char * gittag, int size)
{
    int result = 0;
    char filename[CMD_SIZE];
    char cmdline[CMD_SIZE];

    strcpy_s(filename, CMD_SIZE, "tmpXXXXXX");
    if (_mktemp_s(filename, CMD_SIZE) == 0) {
        size_t length;
        int rc;

        // GITBRANCH:
        if (gitbranch)
        {
            gitbranch[0] = '\0';
            strcpy_s(cmdline, CMD_SIZE, "git -C ..\\..\\cpython name-rev --name-only HEAD > ");
            strcat_s(cmdline, CMD_SIZE, filename);
            rc = system(cmdline);
            if (rc == 0) {
                FILE * fp;

                if (fopen_s(&fp, filename, "r") == 0) {
                    char * cp = fgets(cmdline, CMD_SIZE, fp);
                    if (cp) {
                        strcpy_s(gitbranch, size, cp);
                        length = strlen(gitbranch);
                        if (length > 0 && gitbranch[length-1] == '\n')
                            gitbranch[length-1] = '\0';
                        ++result;
                    }
                    fclose(fp);
                }
            }
        }

        // GITVERSION:
        if (gitversion)
        {
            gitversion[0] = '\0';
            strcpy_s(cmdline, CMD_SIZE, "git -C ..\\..\\cpython rev-parse --short HEAD > ");
            strcat_s(cmdline, CMD_SIZE, filename);
            rc = system(cmdline);
            if (rc == 0) {
                FILE * fp;

                if (fopen_s(&fp, filename, "r") == 0) {
                    char * cp = fgets(cmdline, CMD_SIZE, fp);
                    if (cp) {
                        strcpy_s(gitversion, size, cp);
                        length = strlen(gitversion);
                        if (length > 0 && gitversion[length-1] == '\n')
                            gitversion[length-1] = '\0';
                        ++result;
                    }
                    fclose(fp);
                }
            }
        }

        // GITTAG:
        if (gittag)
        {
            gittag[0] = '\0';
            strcpy_s(cmdline, CMD_SIZE, "git -C ..\\..\\cpython describe --all --always --dirty > ");
            strcat_s(cmdline, CMD_SIZE, filename);
            rc = system(cmdline);
            if (rc == 0) {
                FILE * fp;

                if (fopen_s(&fp, filename, "r") == 0) {
                    char * cp = fgets(cmdline, CMD_SIZE, fp);
                    if (cp) {
                        strcpy_s(gittag, size, cp);
                        length = strlen(gittag);
                        if (length > 0 && gittag[length-1] == '\n')
                            gittag[length-1] = '\0';
                        ++result;
                    }
                    fclose(fp);
                }
            }
        }

        _unlink(filename);
    }
    return (result > 0);
}
#else
int get_mercurial_info(char * hgbranch, char * hgtag, char * hgrev, int size)
{
    int result = 0;
    char filename[CMD_SIZE];
    char cmdline[CMD_SIZE];

    strcpy_s(filename, CMD_SIZE, "tmpXXXXXX");
    if (_mktemp_s(filename, CMD_SIZE) == 0) {
        int rc;

        strcpy_s(cmdline, CMD_SIZE, "hg id -bit > ");
        strcat_s(cmdline, CMD_SIZE, filename);
        rc = system(cmdline);
        if (rc == 0) {
            FILE * fp;
            
            if (fopen_s(&fp, filename, "r") == 0) {
                char * cp = fgets(cmdline, CMD_SIZE, fp);

                if (cp) {
                    char * context = NULL;
                    char * tp = strtok_s(cp, DELIMS, &context);
                    if (tp) {
                        strcpy_s(hgrev, size, tp);
                        tp = strtok_s(NULL, DELIMS, &context);
                        if (tp) {
                            strcpy_s(hgbranch, size, tp);
                            tp = strtok_s(NULL, DELIMS, &context);
                            if (tp) {
                                strcpy_s(hgtag, size, tp);
                                result = 1;
                            }
                        }
                    }
                }
                fclose(fp);
            }
        }
        _unlink(filename);
    }
    return result;
}
#endif

int main(int argc, char*argv[])
{
    FILE * fp;
    char * architecture = "x86";
    char vcvarsall[CMD_SIZE] = "";
    char command[CMD_SIZE] = "cl.exe -c -D_WIN32 -DUSE_DL_EXPORT -D_WINDOWS -D_WINDLL ";
    char tmppath[CMD_SIZE] = "";
    int do_unlink, result;
    char *tmpdir = NULL;
    if (argc <= 2 || argc > 4) {
        fprintf(stderr, "make_buildinfo $(ConfigurationName) [arch] [tmpdir]\n");
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "Release") == 0) {
        strcat_s(command, CMD_SIZE, "-MD ");
    }
    else if (strcmp(argv[1], "Debug") == 0) {
        strcat_s(command, CMD_SIZE, "-D_DEBUG -MDd ");
    }
    else if (strcmp(argv[1], "ReleaseItanium") == 0) {
        strcat_s(command, CMD_SIZE, "-MD /USECL:MS_ITANIUM ");
    }
    else if (strcmp(argv[1], "ReleaseAMD64") == 0) {
        strcat_s(command, CMD_SIZE, "-MD ");
        strcat_s(command, CMD_SIZE, "-MD /USECL:MS_OPTERON ");
    }
    else {
        fprintf(stderr, "unsupported configuration %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (argc > 2) {
        architecture = argv[2];
    }
    if (strcmp(architecture, "amd64") == 0) {
        strcat_s(command, sizeof(command), "-D_WIN64 ");
    }
    if (argc > 3) {
        size_t length;
        tmpdir = argv[3];
        strcat_s(tmppath, _countof(tmppath), tmpdir);
        /* Hack fix for bad command line:  If the command is issued like this:
         * $(SolutionDir)make_buildinfo.exe" Debug "$(IntDir)"
         * we will get a trailing quote because IntDir ends with a backslash that then
         * escapes the final ".  To simplify the life for developers, catch that problem
         * here by cutting it off.
         * The proper command line, btw is:
         * $(SolutionDir)make_buildinfo.exe" Debug "$(IntDir)\"
         * Hooray for command line parsing on windows.
         */
        length = strlen(tmppath);
        if (length > 0)
        {
          if (tmppath[length-1] == '"')
            tmppath[--length] = '\0';
          if (tmppath[length-1] != '\\')
            strcat_s(tmppath, _countof(tmppath), "\\");
        }
    }

    /* Try to invoke ‘vcvarsall.bat’ { */
    {
        HKEY hVisualStudio;
        #if _MSC_VER >= 1910    // VS2017?
            #error "Not supported!"
        #elif _MSC_VER >= 1900  // VS2015?
        //  #define VSVER "14.0"
        #elif _MSC_VER >= 1800  // VS2013?
        //  #define VSVER "12.0"
        #elif _MSC_VER >= 1700  // VS2012?
        //  #define VSVER "11.0"
        #elif _MSC_VER >= 1600  // VS2010?
        //  #define VSVER "10.0"
        #elif _MSC_VER >= 1500  // VS2008?
            #define VSVER "9.0"
        #elif _MSC_VER >= 1400  // VS2005?
        //  #define VSVER "8.0"
        #elif _MSC_VER >= 1310  // VS2003?
        //  #define VSVER "7.1"
        #elif _MSC_VER >= 1300  // VS2002?
        //  #define VSVER "7.0"
        #elif _MSC_VER >= 1200  // VS98?
        //  #define VSVER "6.0"
        #elif _MSC_VER >= 1100  // VS97?
        //  #define VSVER "5.0"
        #endif
        #if !defined(VSVER)
            #error "Unsupported Visual Studio version."
        #endif

        if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\WOW6432Node\\Microsoft\\VisualStudio\\" VSVER, &hVisualStudio) == ERROR_SUCCESS)
        {
            DWORD type, size;

            if (RegQueryValueEx(hVisualStudio, "InstallDir", 0, &type, vcvarsall, &size) == ERROR_SUCCESS &&
                type == REG_SZ)
            {
                strcat_s(vcvarsall, sizeof(vcvarsall), "..\\..\\VC\\vcvarsall.bat");
            }
            else
              vcvarsall[0] = '\0';

            CloseHandle(hVisualStudio);
        }
    }

    if ((do_unlink = make_buildinfo2(tmppath))) {
        strcat_s(command, CMD_SIZE, "\"");
        strcat_s(command, CMD_SIZE, tmppath);
        strcat_s(command, CMD_SIZE, "getbuildinfo2.c\" -DSUBWCREV ");
    }
    else {
        #ifndef CPYTHON_GIT_REPOSITORY
        char hgtag[CMD_SIZE];
        char hgbranch[CMD_SIZE];
        char hgrev[CMD_SIZE];

        if (get_mercurial_info(hgbranch, hgtag, hgrev, CMD_SIZE)) {
            strcat_s(command, CMD_SIZE, "-DHGBRANCH=\\\"");
            strcat_s(command, CMD_SIZE, hgbranch);
            strcat_s(command, CMD_SIZE, "\\\"");

            strcat_s(command, CMD_SIZE, " -DHGTAG=\\\"");
            strcat_s(command, CMD_SIZE, hgtag);
            strcat_s(command, CMD_SIZE, "\\\"");

            strcat_s(command, CMD_SIZE, " -DHGVERSION=\\\"");
            strcat_s(command, CMD_SIZE, hgrev);
            strcat_s(command, CMD_SIZE, "\\\" ");
        }
        #else
        char gitbranch[CMD_SIZE];
        char gitversion[CMD_SIZE];
        char gittag[CMD_SIZE];

        if (get_git_info(gitbranch, gitversion, gittag, CMD_SIZE)) {
            // Switch over to git happened with ‘v3.6.1rc1’
            if (gittag[6] >= '4' ||
                gittag[6] == '3' && (gittag[8] >= '7' ||
                                    (gittag[8] == '6' && gittag[10] == '1'))) {
                strcat_s(command, CMD_SIZE, "-DGITVERSION=\\\"");
                strcat_s(command, CMD_SIZE, gitversion);
                strcat_s(command, CMD_SIZE, "\\\"");

                strcat_s(command, CMD_SIZE, " -DGITTAG=\\\"");
                strcat_s(command, CMD_SIZE, gittag);
                strcat_s(command, CMD_SIZE, "\\\"");

                strcat_s(command, CMD_SIZE, " -DGITBRANCH=\\\"");
                strcat_s(command, CMD_SIZE, gitbranch);
                strcat_s(command, CMD_SIZE, "\\\" ");
            } else {
                strcat_s(command, CMD_SIZE, "-DHGBRANCH=\\\"");
                strcat_s(command, CMD_SIZE, gitbranch);
                strcat_s(command, CMD_SIZE, "\\\"");

                strcat_s(command, CMD_SIZE, " -DHGTAG=\\\"");
                strcat_s(command, CMD_SIZE, gittag);
                strcat_s(command, CMD_SIZE, "\\\"");

                strcat_s(command, CMD_SIZE, " -DHGVERSION=\\\"");
                strcat_s(command, CMD_SIZE, gitversion);
                strcat_s(command, CMD_SIZE, "\\\" ");
            }
        }
        #endif
        strcat_s(command, CMD_SIZE, "-Dinline=__inline ..\\..\\cpython\\Modules\\getbuildinfo.c");
    }
    strcat_s(command, CMD_SIZE, " -Fo\"");
    strcat_s(command, CMD_SIZE, tmppath);
    strcat_s(command, CMD_SIZE, "getbuildinfo.o\" -I..\\Compat\\MSVCRT -I..\\..\\externals\\cpython89-deps\\c99compat\\include -I..\\..\\cpython\\Include -I..\\..\\cpython\\PC");
    puts(command); fflush(stdout);
/*  result = system(command);*/  //PROBLEM: cl.exe environment isn't set correctly!
    {
        result = fopen_s(&fp, "make_buildinfo.bat", "w");
        if (result == 0) {
            result = fprintf(fp, "@CALL \"%s\" %s\n@", vcvarsall, architecture);
            if (result > 0) {
                strcat_s(command, sizeof(command), "\n");
                result = fputs(command, fp);
            }
            fclose(fp);
        }
    }
    if (result >= 0) {
//      puts("system(make_buildinfo.bat)"); fflush(stdout);
        result = system("make_buildinfo.bat");
    }

    if (do_unlink) {
        command[0] = '\0';
        strcat_s(command, CMD_SIZE, "\"");
        strcat_s(command, CMD_SIZE, tmppath);
        strcat_s(command, CMD_SIZE, "getbuildinfo2.c\"");
        _unlink(command);
    }
    if (result < 0)
        return EXIT_FAILURE;
    return 0;
}
