/***
* imminitrin.h substitute: maps hardware lock elision (HLE) calls to
                           simple interlocked exchange instructions.
*******************************************************************************/

#pragma once

#if !defined(_M_IX86) && !defined(_M_X64)
#error This header is specific to X86 and X64 targets
#endif

#ifndef _INCLUDED_IMM
#define _INCLUDED_IMM

#include <intrin.h>  /*VS9COMPAT: interlocked compare and exchange */

extern long _InterlockedExchange_HLEAcquire(long volatile *Target, long Value);
#define _InterlockedExchange_HLEAcquire(Target, Value) _InterlockedExchange(Target, Value)

extern long _InterlockedExchange_HLERelease(long volatile *Target, long Value);
#define _InterlockedExchange_HLERelease(Target, Value) _InterlockedExchange(Target, Value)

#if defined(_M_X64)
extern __int64 _InterlockedExchange64_HLEAcquire(__int64 volatile *Target, __int64 Value);
#define _InterlockedExchange64_HLEAcquire(Target, Value) _InterlockedExchange64(Target, Value)

extern __int64 _InterlockedExchange64_HLERelease(__int64 volatile *Target, __int64 Value);
#define _InterlockedExchange64_HLERelease(Target, Value) _InterlockedExchange64(Target, Value)
#endif  /* defined (_M_X64) */

extern long _InterlockedCompareExchange_HLEAcquire(long volatile *Destination, long Exchange, long Comparand);
#define _InterlockedCompareExchange_HLEAcquire(Destination, Exchange, Comparand) _InterlockedCompareExchange(Destination, Exchange, Comparand)

extern long _InterlockedCompareExchange_HLERelease(long volatile *Destination, long Exchange, long Comparand);
#define _InterlockedCompareExchange_HLERelease(Destination, Exchange, Comparand) _InterlockedCompareExchange(Destination, Exchange, Comparand)

#if defined(_M_X64)
extern __int64 _InterlockedCompareExchange64_HLEAcquire(__int64 volatile *Destination, __int64 Exchange, __int64 Comparand);
#define _InterlockedCompareExchange64_HLEAcquire(Destination, Exchange, Comparand) _InterlockedCompareExchange64(Destination, Exchange, Comparand)

extern __int64 _InterlockedCompareExchange64_HLERelease(__int64 volatile *Destination, __int64 Exchange, __int64 Comparand);
#define _InterlockedCompareExchange64_HLERelease(Destination, Exchange, Comparand) _InterlockedCompareExchange64(Destination, Exchange, Comparand)
#endif  /* defined (_M_X64) */

#endif  /* _INCLUDED_IMM */
