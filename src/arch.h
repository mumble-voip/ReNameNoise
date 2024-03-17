/* Copyright (c) 2003-2008 Jean-Marc Valin
   Copyright (c) 2007-2008 CSIRO
   Copyright (c) 2007-2009 Xiph.Org Foundation
   Written by Jean-Marc Valin */
/**
   @file arch.h
   @brief Various architecture definitions for CELT
*/
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ARCH_H
#define ARCH_H

#include "renamenoise_types.h"
#include "common.h"

# if !defined(__GNUC_PREREQ)
#  if defined(__GNUC__)&&defined(__GNUC_MINOR__)
#   define __GNUC_PREREQ(_maj,_min) \
 ((__GNUC__<<16)+__GNUC_MINOR__>=((_maj)<<16)+(_min))
#  else
#   define __GNUC_PREREQ(_maj,_min) 0
#  endif
# endif

#define RENAMENOISE_SIG_SCALE 32768.f

#define renamenoise_fatal(str) _renamenoise_fatal(str, __FILE__, __LINE__);
#ifdef ENABLE_ASSERTIONS
#include <stdio.h>
#include <stdlib.h>
#ifdef __GNUC__
__attribute__((noreturn))
#endif
static RENAMENOISE_INLINE void _renamenoise_fatal(const char *str, const char *file, int line)
{
   fprintf (stderr, "Fatal (internal) error in %s, line %d: %s\n", file, line, str);
   abort();
}
#define renamenoise_assert(cond) {if (!(cond)) {renamenoise_fatal("assertion failed: " #cond);}}
#define renamenoise_assert2(cond, message) {if (!(cond)) {renamenoise_fatal("assertion failed: " #cond "\n" message);}}
#else
#define renamenoise_assert(cond)
#define renamenoise_assert2(cond, message)
#endif

#define RENAMENOISE_IMUL32(a,b) ((a)*(b))

#define RENAMENOISE_MIN16(a,b) ((a) < (b) ? (a) : (b))   /**< Minimum 16-bit value.   */
#define RENAMENOISE_MAX16(a,b) ((a) > (b) ? (a) : (b))   /**< Maximum 16-bit value.   */
#define RENAMENOISE_MIN32(a,b) ((a) < (b) ? (a) : (b))   /**< Minimum 32-bit value.   */
#define RENAMENOISE_MAX32(a,b) ((a) > (b) ? (a) : (b))   /**< Maximum 32-bit value.   */
#define RENAMENOISE_IMIN(a,b) ((a) < (b) ? (a) : (b))   /**< Minimum int value.   */
#define RENAMENOISE_IMAX(a,b) ((a) > (b) ? (a) : (b))   /**< Maximum int value.   */
#define RENAMENOISE_UADD32(a,b) ((a)+(b))
#define RENAMENOISE_USUB32(a,b) ((a)-(b))

/* Set this if renamenoise_int64 is a native type of the CPU. */
/* Assume that all LP64 architectures have fast 64-bit types; also x86_64
   (which can be ILP32 for x32) and Win64 (which is LLP64). */
#if defined(__x86_64__) || defined(__LP64__) || defined(_WIN64)
#define RENAMENOISE_FAST_INT64 1
#else
#define RENAMENOISE_FAST_INT64 0
#endif

#define PRINT_MIPS(file)

typedef float renamenoise_val16;
typedef float renamenoise_val32;
typedef float renamenoise_val64;

typedef float renamenoise_sig;
typedef float renamenoise_norm;
typedef float renamenoise_ener;

#ifdef RENAMENOISE_FLOAT_APPROX
/* This code should reliably detect NaN/inf even when -ffast-math is used.
   Assumes IEEE 754 format. */
static RENAMENOISE_INLINE int renamenoise_isnan(float x)
{
   union {float f; renamenoise_uint32 i;} in;
   in.f = x;
   return ((in.i>>23)&0xFF)==0xFF && (in.i&0x007FFFFF)!=0;
}
#else
#ifdef __FAST_MATH__
#error Cannot build renamenoise with -ffast-math unless RENAMENOISE_FLOAT_APPROX is defined. This could result in crashes on extreme (e.g. NaN) input
#endif
#define renamenoise_isnan(x) ((x)!=(x))
#endif

#define RENAMENOISE_Q15ONE 1.0f

#define RENAMENOISE_NORM_SCALING 1.f

#define RENAMENOISE_EPSILON 1e-15f
#define RENAMENOISE_VERY_SMALL 1e-30f
#define RENAMENOISE_VERY_LARGE16 1e15f
#define RENAMENOISE_Q15_ONE ((renamenoise_val16)1.f)

/* This appears to be the same speed as C99's fabsf() but it's more portable. */
#define RENAMENOISE_ABS16(x) ((float)fabs(x))
#define RENAMENOISE_ABS32(x) ((float)fabs(x))

#define RENAMENOISE_QCONST16(x,bits) (x)
#define RENAMENOISE_QCONST32(x,bits) (x)

#define RENAMENOISE_NEG16(x) (-(x))
#define RENAMENOISE_NEG32(x) (-(x))
#define RENAMENOISE_NEG32_ovflw(x) (-(x))
#define RENAMENOISE_EXTRACT16(x) (x)
#define RENAMENOISE_EXTEND32(x) (x)
#define RENAMENOISE_SHR16(a,shift) (a)
#define RENAMENOISE_SHL16(a,shift) (a)
#define RENAMENOISE_SHR32(a,shift) (a)
#define RENAMENOISE_SHL32(a,shift) (a)
#define RENAMENOISE_PSHR32(a,shift) (a)
#define RENAMENOISE_VSHR32(a,shift) (a)

#define RENAMENOISE_PSHR(a,shift)   (a)
#define RENAMENOISE_SHR(a,shift)    (a)
#define RENAMENOISE_SHL(a,shift)    (a)
#define SATURATE(x,a)   (x)
#define SATURATE16(x)   (x)

#define ROUND16(a,shift)  (a)
#define SROUND16(a,shift) (a)
#define HALF16(x)       (.5f*(x))
#define HALF32(x)       (.5f*(x))

#define ADD16(a,b) ((a)+(b))
#define SUB16(a,b) ((a)-(b))
#define ADD32(a,b) ((a)+(b))
#define SUB32(a,b) ((a)-(b))
#define ADD32_ovflw(a,b) ((a)+(b))
#define SUB32_ovflw(a,b) ((a)-(b))
#define MULT16_16_16(a,b)     ((a)*(b))
#define MULT16_16(a,b)     ((renamenoise_val32)(a)*(renamenoise_val32)(b))
#define MAC16_16(c,a,b)     ((c)+(renamenoise_val32)(a)*(renamenoise_val32)(b))

#define MULT16_32_Q15(a,b)     ((a)*(b))
#define MULT16_32_Q16(a,b)     ((a)*(b))

#define MULT32_32_Q31(a,b)     ((a)*(b))

#define MAC16_32_Q15(c,a,b)     ((c)+(a)*(b))
#define MAC16_32_Q16(c,a,b)     ((c)+(a)*(b))

#define MULT16_16_Q11_32(a,b)     ((a)*(b))
#define MULT16_16_Q11(a,b)     ((a)*(b))
#define MULT16_16_Q13(a,b)     ((a)*(b))
#define MULT16_16_Q14(a,b)     ((a)*(b))
#define MULT16_16_Q15(a,b)     ((a)*(b))
#define MULT16_16_P15(a,b)     ((a)*(b))
#define MULT16_16_P13(a,b)     ((a)*(b))
#define MULT16_16_P14(a,b)     ((a)*(b))
#define MULT16_32_P16(a,b)     ((a)*(b))

#define DIV32_16(a,b)     (((renamenoise_val32)(a))/(renamenoise_val16)(b))
#define DIV32(a,b)     (((renamenoise_val32)(a))/(renamenoise_val32)(b))

#define SCALEIN(a)      ((a)*RENAMENOISE_SIG_SCALE)
#define SCALEOUT(a)     ((a)*(1/RENAMENOISE_SIG_SCALE))

#define SIG2WORD16(x) (x)

#ifndef GLOBAL_STACK_SIZE
#define GLOBAL_STACK_SIZE 120000
#endif

#endif /* ARCH_H */
