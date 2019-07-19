#ifndef M_COMPAT_H
#define M_COMPAT_H

/* Disable ‘deprecated’ warnings: */
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#if !defined(MODULES_PICKLE)
  #define _INC_WINDOWS              /* Prevent inclusion of windows.h in winsock.h */
    #include <sdkddkver.h>
    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_IX86)
    #define _X86_
    #endif

    #if !defined(_68K_) && !defined(_MPPC_) && !defined(_X86_) && !defined(_IA64_) && !defined(_AMD64_) && defined(_M_AMD64)
    #define _AMD64_
    #endif
    #include <Windef.h>
    #include <winbase.h>
    #undef Yield                    /* WinBase.h: pointless (empty) compatibility macro */
    #include <winsock2.h>           /* (BSD) Sockets */
    #define _WINSOCKAPI_            /* Prevent inclusion of winsock.h in windows.h */
  #undef _INC_WINDOWS

  #include <stdarg.h>               /* Variadic functions support: va_list, va_start(), va_arg(), va_end() */
  #ifndef va_copy                   /* va_copy() [introduced with VS2013] */
  #define va_copy(dst, src) ((dst) = (src))
  #endif

  #include <stdbool.h>              /* _Bool, false, true */

  /* Visual Studio 2008 Standard Types:
     https://msdn.microsoft.com/en-us/library/323b6b3k(v=vs.90).aspx
  */
  #include <crtdefs.h>              /* size_t */

  #include "pyconfig.h"
  #undef HAVE_ERF
  #undef HAVE_ERFC

  /*===  C99 support?  ===*/
  #define ANSI_C89 1
  #if defined(__STDC__)
    #if defined(__STDC_VERSION__)
      #define ISO_C90 1
      #if __STDC_VERSION__ >= 201710L   /* C17/C18 */ /* ISO/IEC 9899:2018: fixes 54 defects in C11 */
        #define ISO_C17 1
      #elif __STDC_VERSION__ >= 201112L /* C11 */     /* ISO/IEC 9899:2011 */
        #define ISO_C11 1
      #elif __STDC_VERSION__ >= 199901L /* C99 */     /* ISO/IEC 9899:1999 */
        #define ISO_C99 1
      #elif __STDC_VERSION__ >= 199409L /* C94/C95 */ /* ISO/IEC 9899/AMD1:1995, Normative Addendum 1 (NA1) */
        #define ISO_C94 1
      #endif
    #endif
  #elif defined(_MSC_VER)
    #if _MSC_VER >= 1800  /* >= VS2013? */
       #define ISO_C99 1
    #endif
  #endif
  #ifndef ISO_C90
    #define ISO_C90 1 /* Today, essentially alle compilers are C90 compatible. */
  #endif
  #ifndef ISO_C94
    #define ISO_C94 0
  #endif
  #ifndef ISO_C99
    #define ISO_C99 0
  #endif
  #ifndef ISO_C11
    #define ISO_C11 0
  #endif
  #ifndef ISO_C17
    #define ISO_C17 0
  #endif

  /*===  C99 keywords  ===*/
  #if defined(__STDC_VERSION__)
    #if __STDC_VERSION__ < 199901L
      #define inline  /* The ‘inline’ keyword was added with C99! */
    #endif
  #elif !defined(__cplusplus) && !defined(inline)
    #if defined(_MSC_VER)
      #if _MSC_VER >= 1500 /* MSVC 9 or newer */
        #define inline __inline
      #else
        #define inline
      #endif
    #elif defined(__GNUC__)
      #if __GNUC__ >= 3 /* GCC 3 or newer */
        #define inline __inline
      #else
        #define inline
      #endif
    #else /* Unknown or ancient */
      #define inline
    #endif
  #endif
#endif

#endif
