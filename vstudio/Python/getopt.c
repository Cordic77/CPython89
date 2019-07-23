/*---------------------------------------------------------------------------*
 * <RCS keywords>
 *
 * C++ Library
 *
 * Copyright 1992-1994, David Gottner
 *
 *                    All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice, this permission notice and
 * the following disclaimer notice appear unmodified in all copies.
 *
 * I DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL I
 * BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA, OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Nevertheless, I would like to know about bugs in this library or
 * suggestions for improvment.  Send bug reports and feedback to
 * davegottner@delphi.com.
 *---------------------------------------------------------------------------*/

/* Modified to support --help and --version, as well as /? on Windows
 * by Georg Brandl. */

#include <Python.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "internal/pygetopt.h"

#ifdef __cplusplus
extern "C" {
#endif

int _PyOS_opterr = 1;          /* generate error messages */
int _PyOS_optind = 1;          /* index into argv array   */
wchar_t *_PyOS_optarg = NULL;     /* optional argument       */

static wchar_t *opt_ptr = L"";

void _PyOS_ResetGetOpt(void)
{
    _PyOS_opterr = 1;
    _PyOS_optind = 1;
    _PyOS_optarg = NULL;
    opt_ptr = L"";
}

int _PyOS_GetOpt(int argc, wchar_t **argv, wchar_t *optstring,
                 const _PyOS_LongOption *longopts, int *longindex)
{
    wchar_t *ptr;
    wchar_t option;

    if (*opt_ptr == '\0') {

        if (_PyOS_optind >= argc)
            return -1;
#ifdef MS_WINDOWS
        else if (wcscmp(argv[_PyOS_optind], L"/?") == 0) {
            ++_PyOS_optind;
            return 'h';
        }
#endif

        else if (argv[_PyOS_optind][0] != L'-' ||
                 argv[_PyOS_optind][1] == L'\0' /* lone dash */ )
            return -1;

        else if (wcscmp(argv[_PyOS_optind], L"--") == 0) {
            ++_PyOS_optind;
            return -1;
        }

        else if (wcscmp(argv[_PyOS_optind], L"--help") == 0) {
            ++_PyOS_optind;
            return 'h';
        }

        else if (wcscmp(argv[_PyOS_optind], L"--version") == 0) {
            ++_PyOS_optind;
            return 'V';
        }

        opt_ptr = &argv[_PyOS_optind++][1];
    }

    if ((option = *opt_ptr++) == L'\0')
        return -1;

    if (option == L'-') {
        // Parse long option.
        if (*opt_ptr == L'\0') {
            fprintf(stderr, "expected long option\n");
            return -1;
        }
        *longindex = 0;
      { const _PyOS_LongOption *opt;  /*C89 -- mixed declarations and code*/
        for (opt = &longopts[*longindex]; opt->name; opt = &longopts[++(*longindex)]) {
            if (!wcscmp(opt->name, opt_ptr))
                break;
        }
        if (!opt->name) {
            fprintf(stderr, "unknown option %ls\n", argv[_PyOS_optind - 1]);
            return '_';
        }
        opt_ptr = L"";
        if (!opt->has_arg) {
            return opt->val;
        }
        if (_PyOS_optind >= argc) {
            fprintf(stderr, "Argument expected for the %ls options\n",
                    argv[_PyOS_optind - 1]);
            return '_';
        }
        _PyOS_optarg = argv[_PyOS_optind++];
        return opt->val;
      }
    }

    if (option == 'J') {
        if (_PyOS_opterr)
            fprintf(stderr, "-J is reserved for Jython\n");
        return '_';
    }

    if ((ptr = wcschr(optstring, option)) == NULL) {
        if (_PyOS_opterr)
            fprintf(stderr, "Unknown option: -%c\n", (char)option);
        return '_';
    }

    if (*(ptr + 1) == L':') {
        if (*opt_ptr != L'\0') {
            _PyOS_optarg  = opt_ptr;
            opt_ptr = L"";
        }

        else {
            if (_PyOS_optind >= argc) {
                if (_PyOS_opterr)
                    fprintf(stderr,
                        "Argument expected for the -%c option\n", (char)option);
                return '_';
            }

            _PyOS_optarg = argv[_PyOS_optind++];
        }
    }

    return option;
}

#ifdef __cplusplus
}
#endif
