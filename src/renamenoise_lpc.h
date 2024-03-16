/* Copyright (c) 2009-2010 Xiph.Org Foundation
   Written by Jean-Marc Valin */
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

#ifndef RENAMENOISE_LPC_H
#define RENAMENOISE_LPC_H

#include "arch.h"
#include "common.h"

#define RENAMENOISE_LPC_ORDER 24

void _renamenoise_lpc(renamenoise_val16 *_lpc, const renamenoise_val32 *ac, int p);

void renamenoise_fir(
         const renamenoise_val16 *x,
         const renamenoise_val16 *num,
         renamenoise_val16 *y,
         int N,
         int ord);

void renamenoise_iir(const renamenoise_val32 *x,
         const renamenoise_val16 *den,
         renamenoise_val32 *y,
         int N,
         int ord,
         renamenoise_val16 *mem);

int _renamenoise_autocorr(const renamenoise_val16 *x, renamenoise_val32 *ac,
         const renamenoise_val16 *window, int overlap, int lag, int n);

#endif /* RENAMENOISE_LPC_H */
