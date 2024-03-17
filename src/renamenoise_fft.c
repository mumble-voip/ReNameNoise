/*Copyright (c) 2003-2004, Mark Borgerding
  Lots of modifications by Jean-Marc Valin
  Copyright (c) 2005-2007, Xiph.Org Foundation
  Copyright (c) 2008,      Xiph.Org Foundation, CSIRO

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.*/

/* This code is originally from Mark Borgerding's KISS-FFT but has been
   heavily modified to better suit Opus */

#ifndef SKIP_CONFIG_H
#  ifdef HAVE_CONFIG_H
#    include "config.h"
#  endif
#endif

#include "_renamenoise_fft_guts.h"
#define CUSTOM_MODES

/* The guts header contains all the multiplication and addition macros that are defined for
   complex numbers.  It also declares the kf_ internal functions.
*/

static void kf_bfly2(
                     renamenoise_fft_cpx * Fout,
                     int m,
                     int N
                    )
{
   renamenoise_fft_cpx * Fout2;
   int i;
   (void)m;
#ifdef CUSTOM_MODES
   if (m==1)
   {
      renamenoise_assert(m==1);
      for (i=0;i<N;i++)
      {
         renamenoise_fft_cpx t;
         Fout2 = Fout + 1;
         t = *Fout2;
         RENAMENOISE_C_SUB( *Fout2 ,  *Fout , t );
         RENAMENOISE_C_ADDTO( *Fout ,  t );
         Fout += 2;
      }
   } else
#endif
   {
      renamenoise_val16 tw;
      tw = RENAMENOISE_QCONST16(0.7071067812f, 15);
      /* We know that m==4 here because the radix-2 is just after a radix-4 */
      renamenoise_assert(m==4);
      for (i=0;i<N;i++)
      {
         renamenoise_fft_cpx t;
         Fout2 = Fout + 4;
         t = Fout2[0];
         RENAMENOISE_C_SUB( Fout2[0] ,  Fout[0] , t );
         RENAMENOISE_C_ADDTO( Fout[0] ,  t );

         t.r = RENAMENOISE_S_MUL(RENAMENOISE_ADD32_ovflw(Fout2[1].r, Fout2[1].i), tw);
         t.i = RENAMENOISE_S_MUL(RENAMENOISE_SUB32_ovflw(Fout2[1].i, Fout2[1].r), tw);
         RENAMENOISE_C_SUB( Fout2[1] ,  Fout[1] , t );
         RENAMENOISE_C_ADDTO( Fout[1] ,  t );

         t.r = Fout2[2].i;
         t.i = -Fout2[2].r;
         RENAMENOISE_C_SUB( Fout2[2] ,  Fout[2] , t );
         RENAMENOISE_C_ADDTO( Fout[2] ,  t );

         t.r = RENAMENOISE_S_MUL(RENAMENOISE_SUB32_ovflw(Fout2[3].i, Fout2[3].r), tw);
         t.i = RENAMENOISE_S_MUL(RENAMENOISE_NEG32_ovflw(RENAMENOISE_ADD32_ovflw(Fout2[3].i, Fout2[3].r)), tw);
         RENAMENOISE_C_SUB( Fout2[3] ,  Fout[3] , t );
         RENAMENOISE_C_ADDTO( Fout[3] ,  t );
         Fout += 8;
      }
   }
}

static void kf_bfly4(
                     renamenoise_fft_cpx * Fout,
                     const size_t fstride,
                     const renamenoise_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   int i;

   if (m==1)
   {
      /* Degenerate case where all the twiddles are 1. */
      for (i=0;i<N;i++)
      {
         renamenoise_fft_cpx scratch0, scratch1;

         RENAMENOISE_C_SUB( scratch0 , *Fout, Fout[2] );
         RENAMENOISE_C_ADDTO(*Fout, Fout[2]);
         RENAMENOISE_C_ADD( scratch1 , Fout[1] , Fout[3] );
         RENAMENOISE_C_SUB( Fout[2], *Fout, scratch1 );
         RENAMENOISE_C_ADDTO( *Fout , scratch1 );
         RENAMENOISE_C_SUB( scratch1 , Fout[1] , Fout[3] );

         Fout[1].r = RENAMENOISE_ADD32_ovflw(scratch0.r, scratch1.i);
         Fout[1].i = RENAMENOISE_SUB32_ovflw(scratch0.i, scratch1.r);
         Fout[3].r = RENAMENOISE_SUB32_ovflw(scratch0.r, scratch1.i);
         Fout[3].i = RENAMENOISE_ADD32_ovflw(scratch0.i, scratch1.r);
         Fout+=4;
      }
   } else {
      int j;
      renamenoise_fft_cpx scratch[6];
      const renamenoise_twiddle_cpx *tw1,*tw2,*tw3;
      const int m2=2*m;
      const int m3=3*m;
      renamenoise_fft_cpx * Fout_beg = Fout;
      for (i=0;i<N;i++)
      {
         Fout = Fout_beg + i*mm;
         tw3 = tw2 = tw1 = st->twiddles;
         /* m is guaranteed to be a multiple of 4. */
         for (j=0;j<m;j++)
         {
            RENAMENOISE_C_MUL(scratch[0],Fout[m] , *tw1 );
            RENAMENOISE_C_MUL(scratch[1],Fout[m2] , *tw2 );
            RENAMENOISE_C_MUL(scratch[2],Fout[m3] , *tw3 );

            RENAMENOISE_C_SUB( scratch[5] , *Fout, scratch[1] );
            RENAMENOISE_C_ADDTO(*Fout, scratch[1]);
            RENAMENOISE_C_ADD( scratch[3] , scratch[0] , scratch[2] );
            RENAMENOISE_C_SUB( scratch[4] , scratch[0] , scratch[2] );
            RENAMENOISE_C_SUB( Fout[m2], *Fout, scratch[3] );
            tw1 += fstride;
            tw2 += fstride*2;
            tw3 += fstride*3;
            RENAMENOISE_C_ADDTO( *Fout , scratch[3] );

            Fout[m].r = RENAMENOISE_ADD32_ovflw(scratch[5].r, scratch[4].i);
            Fout[m].i = RENAMENOISE_SUB32_ovflw(scratch[5].i, scratch[4].r);
            Fout[m3].r = RENAMENOISE_SUB32_ovflw(scratch[5].r, scratch[4].i);
            Fout[m3].i = RENAMENOISE_ADD32_ovflw(scratch[5].i, scratch[4].r);
            ++Fout;
         }
      }
   }
}


#ifndef RADIX_TWO_ONLY

static void kf_bfly3(
                     renamenoise_fft_cpx * Fout,
                     const size_t fstride,
                     const renamenoise_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   int i;
   size_t k;
   const size_t m2 = 2*m;
   const renamenoise_twiddle_cpx *tw1,*tw2;
   renamenoise_fft_cpx scratch[5];
   renamenoise_twiddle_cpx epi3;

   renamenoise_fft_cpx * Fout_beg = Fout;
   epi3 = st->twiddles[fstride*m];
   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      tw1=tw2=st->twiddles;
      /* For non-custom modes, m is guaranteed to be a multiple of 4. */
      k=m;
      do {

         RENAMENOISE_C_MUL(scratch[1],Fout[m] , *tw1);
         RENAMENOISE_C_MUL(scratch[2],Fout[m2] , *tw2);

         RENAMENOISE_C_ADD(scratch[3],scratch[1],scratch[2]);
         RENAMENOISE_C_SUB(scratch[0],scratch[1],scratch[2]);
         tw1 += fstride;
         tw2 += fstride*2;

         Fout[m].r = RENAMENOISE_SUB32_ovflw(Fout->r, RENAMENOISE_HALF_OF(scratch[3].r));
         Fout[m].i = RENAMENOISE_SUB32_ovflw(Fout->i, RENAMENOISE_HALF_OF(scratch[3].i));

         RENAMENOISE_C_MULBYSCALAR( scratch[0] , epi3.i );

         RENAMENOISE_C_ADDTO(*Fout,scratch[3]);

         Fout[m2].r = RENAMENOISE_ADD32_ovflw(Fout[m].r, scratch[0].i);
         Fout[m2].i = RENAMENOISE_SUB32_ovflw(Fout[m].i, scratch[0].r);

         Fout[m].r = RENAMENOISE_SUB32_ovflw(Fout[m].r, scratch[0].i);
         Fout[m].i = RENAMENOISE_ADD32_ovflw(Fout[m].i, scratch[0].r);

         ++Fout;
      } while(--k);
   }
}


#ifndef OVERRIDE_kf_bfly5
static void kf_bfly5(
                     renamenoise_fft_cpx * Fout,
                     const size_t fstride,
                     const renamenoise_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   renamenoise_fft_cpx *Fout0,*Fout1,*Fout2,*Fout3,*Fout4;
   int i, u;
   renamenoise_fft_cpx scratch[13];
   const renamenoise_twiddle_cpx *tw;
   renamenoise_twiddle_cpx ya,yb;
   renamenoise_fft_cpx * Fout_beg = Fout;

   ya = st->twiddles[fstride*m];
   yb = st->twiddles[fstride*2*m];
   tw=st->twiddles;

   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      Fout0=Fout;
      Fout1=Fout0+m;
      Fout2=Fout0+2*m;
      Fout3=Fout0+3*m;
      Fout4=Fout0+4*m;

      /* For non-custom modes, m is guaranteed to be a multiple of 4. */
      for ( u=0; u<m; ++u ) {
         scratch[0] = *Fout0;

         RENAMENOISE_C_MUL(scratch[1] ,*Fout1, tw[u*fstride]);
         RENAMENOISE_C_MUL(scratch[2] ,*Fout2, tw[2*u*fstride]);
         RENAMENOISE_C_MUL(scratch[3] ,*Fout3, tw[3*u*fstride]);
         RENAMENOISE_C_MUL(scratch[4] ,*Fout4, tw[4*u*fstride]);

         RENAMENOISE_C_ADD( scratch[7],scratch[1],scratch[4]);
         RENAMENOISE_C_SUB( scratch[10],scratch[1],scratch[4]);
         RENAMENOISE_C_ADD( scratch[8],scratch[2],scratch[3]);
         RENAMENOISE_C_SUB( scratch[9],scratch[2],scratch[3]);

         Fout0->r = RENAMENOISE_ADD32_ovflw(Fout0->r, RENAMENOISE_ADD32_ovflw(scratch[7].r, scratch[8].r));
         Fout0->i = RENAMENOISE_ADD32_ovflw(Fout0->i, RENAMENOISE_ADD32_ovflw(scratch[7].i, scratch[8].i));

         scratch[5].r = RENAMENOISE_ADD32_ovflw(scratch[0].r, RENAMENOISE_ADD32_ovflw(RENAMENOISE_S_MUL(scratch[7].r,ya.r), RENAMENOISE_S_MUL(scratch[8].r,yb.r)));
         scratch[5].i = RENAMENOISE_ADD32_ovflw(scratch[0].i, RENAMENOISE_ADD32_ovflw(RENAMENOISE_S_MUL(scratch[7].i,ya.r), RENAMENOISE_S_MUL(scratch[8].i,yb.r)));

         scratch[6].r =  RENAMENOISE_ADD32_ovflw(RENAMENOISE_S_MUL(scratch[10].i,ya.i), RENAMENOISE_S_MUL(scratch[9].i,yb.i));
         scratch[6].i = RENAMENOISE_NEG32_ovflw(RENAMENOISE_ADD32_ovflw(RENAMENOISE_S_MUL(scratch[10].r,ya.i), RENAMENOISE_S_MUL(scratch[9].r,yb.i)));

         RENAMENOISE_C_SUB(*Fout1,scratch[5],scratch[6]);
         RENAMENOISE_C_ADD(*Fout4,scratch[5],scratch[6]);

         scratch[11].r = RENAMENOISE_ADD32_ovflw(scratch[0].r, RENAMENOISE_ADD32_ovflw(RENAMENOISE_S_MUL(scratch[7].r,yb.r), RENAMENOISE_S_MUL(scratch[8].r,ya.r)));
         scratch[11].i = RENAMENOISE_ADD32_ovflw(scratch[0].i, RENAMENOISE_ADD32_ovflw(RENAMENOISE_S_MUL(scratch[7].i,yb.r), RENAMENOISE_S_MUL(scratch[8].i,ya.r)));
         scratch[12].r = RENAMENOISE_SUB32_ovflw(RENAMENOISE_S_MUL(scratch[9].i,ya.i), RENAMENOISE_S_MUL(scratch[10].i,yb.i));
         scratch[12].i = RENAMENOISE_SUB32_ovflw(RENAMENOISE_S_MUL(scratch[10].r,yb.i), RENAMENOISE_S_MUL(scratch[9].r,ya.i));

         RENAMENOISE_C_ADD(*Fout2,scratch[11],scratch[12]);
         RENAMENOISE_C_SUB(*Fout3,scratch[11],scratch[12]);

         ++Fout0;++Fout1;++Fout2;++Fout3;++Fout4;
      }
   }
}
#endif /* OVERRIDE_kf_bfly5 */


#endif


#ifdef CUSTOM_MODES

static
void compute_bitrev_table(
         int Fout,
         renamenoise_int16 *f,
         const size_t fstride,
         int in_stride,
         renamenoise_int16 * factors,
         const renamenoise_fft_state *st
            )
{
   const int p=*factors++; /* the radix  */
   const int m=*factors++; /* stage's fft length/p */

    /*printf ("fft %d %d %d %d %d %d\n", p*m, m, p, s2, fstride*in_stride, N);*/
   if (m==1)
   {
      int j;
      for (j=0;j<p;j++)
      {
         *f = Fout+j;
         f += fstride*in_stride;
      }
   } else {
      int j;
      for (j=0;j<p;j++)
      {
         compute_bitrev_table( Fout , f, fstride*p, in_stride, factors,st);
         f += fstride*in_stride;
         Fout += m;
      }
   }
}

/*  facbuf is populated by p1,m1,p2,m2, ...
    where
    p[i] * m[i] = m[i-1]
    m0 = n                  */
static
int kf_factor(int n,renamenoise_int16 * facbuf)
{
    int p=4;
    int i;
    int stages=0;
    int nbak = n;

    /*factor out powers of 4, powers of 2, then any remaining primes */
    do {
        while (n % p) {
            switch (p) {
                case 4: p = 2; break;
                case 2: p = 3; break;
                default: p += 2; break;
            }
            if (p>32000 || (renamenoise_int32)p*(renamenoise_int32)p > n)
                p = n;          /* no more factors, skip to end */
        }
        n /= p;
#ifdef RADIX_TWO_ONLY
        if (p!=2 && p != 4)
#else
        if (p>5)
#endif
        {
           return 0;
        }
        facbuf[2*stages] = p;
        if (p==2 && stages > 1)
        {
           facbuf[2*stages] = 4;
           facbuf[2] = 2;
        }
        stages++;
    } while (n > 1);
    n = nbak;
    /* Reverse the order to get the radix 4 at the end, so we can use the
       fast degenerate case. It turns out that reversing the order also
       improves the noise behaviour. */
    for (i=0;i<stages/2;i++)
    {
       int tmp;
       tmp = facbuf[2*i];
       facbuf[2*i] = facbuf[2*(stages-i-1)];
       facbuf[2*(stages-i-1)] = tmp;
    }
    for (i=0;i<stages;i++)
    {
        n /= facbuf[2*i];
        facbuf[2*i+1] = n;
    }
    return 1;
}

static void compute_twiddles(renamenoise_twiddle_cpx *twiddles, int nfft)
{
   int i;
   for (i=0;i<nfft;++i) {
      const double pi=3.14159265358979323846264338327;
      double phase = ( -2*pi /nfft ) * i;
      renamenoise_kf_cexp(twiddles+i, phase );
   }
}

int opus_fft_alloc_arch_c(renamenoise_fft_state *st) {
   (void)st;
   return 0;
}

/*
 *
 * Allocates all necessary storage space for the fft and ifft.
 * The return value is a contiguous block of memory.  As such,
 * It can be freed with free().
 * */
renamenoise_fft_state *renamenoise_fft_alloc_twiddles(int nfft,void * mem,size_t * lenmem,
                                        const renamenoise_fft_state *base, int arch)
{
    renamenoise_fft_state *st=NULL;
    size_t memneeded = sizeof(struct renamenoise_fft_state); /* twiddle factors*/

    if ( lenmem==NULL ) {
        st = ( renamenoise_fft_state*)RENAMENOISE_FFT_MALLOC( memneeded );
    }else{
        if (mem != NULL && *lenmem >= memneeded)
            st = (renamenoise_fft_state*)mem;
        *lenmem = memneeded;
    }
    if (st) {
        renamenoise_int16 *bitrev;
        renamenoise_twiddle_cpx *twiddles;

        st->nfft=nfft;
        st->scale = 1.f/nfft;
        if (base != NULL)
        {
           st->twiddles = base->twiddles;
           st->shift = 0;
           while (st->shift < 32 && nfft<<st->shift != base->nfft)
              st->shift++;
           if (st->shift>=32)
              goto fail;
        } else {
           st->twiddles = twiddles = (renamenoise_twiddle_cpx*)RENAMENOISE_FFT_MALLOC(sizeof(renamenoise_twiddle_cpx)*nfft);
           compute_twiddles(twiddles, nfft);
           st->shift = -1;
        }
        if (!kf_factor(nfft,st->factors))
        {
           goto fail;
        }

        /* bitrev */
        st->bitrev = bitrev = (renamenoise_int16*)RENAMENOISE_FFT_MALLOC(sizeof(renamenoise_int16)*nfft);
        if (st->bitrev==NULL)
            goto fail;
        compute_bitrev_table(0, bitrev, 1,1, st->factors,st);

        /* Initialize architecture specific fft parameters */
        if (opus_fft_alloc_arch(st, arch))
            goto fail;
    }
    return st;
fail:
    opus_fft_free(st, arch);
    return NULL;
}

renamenoise_fft_state *renamenoise_fft_alloc(int nfft,void * mem,size_t * lenmem, int arch)
{
   return renamenoise_fft_alloc_twiddles(nfft, mem, lenmem, NULL, arch);
}

void opus_fft_free_arch_c(renamenoise_fft_state *st) {
   (void)st;
}

void opus_fft_free(const renamenoise_fft_state *cfg, int arch)
{
   if (cfg)
   {
      opus_fft_free_arch((renamenoise_fft_state *)cfg, arch);
      renamenoise_free2((renamenoise_int16*)cfg->bitrev);
      if (cfg->shift < 0)
         renamenoise_free2((renamenoise_twiddle_cpx*)cfg->twiddles);
      renamenoise_free2((renamenoise_fft_state*)cfg);
   }
}

#endif /* CUSTOM_MODES */

void renamenoise_fft_impl(const renamenoise_fft_state *st,renamenoise_fft_cpx *fout)
{
    int m2, m;
    int p;
    int L;
    int fstride[RENAMENOISE_MAXFACTORS];
    int i;
    int shift;

    /* st->shift can be -1 */
    shift = st->shift>0 ? st->shift : 0;

    fstride[0] = 1;
    L=0;
    do {
       p = st->factors[2*L];
       m = st->factors[2*L+1];
       fstride[L+1] = fstride[L]*p;
       L++;
    } while(m!=1);
    m = st->factors[2*L-1];
    for (i=L-1;i>=0;i--)
    {
       if (i!=0)
          m2 = st->factors[2*i-1];
       else
          m2 = 1;
       switch (st->factors[2*i])
       {
       case 2:
          kf_bfly2(fout, m, fstride[i]);
          break;
       case 4:
          kf_bfly4(fout,fstride[i]<<shift,st,m, fstride[i], m2);
          break;
 #ifndef RADIX_TWO_ONLY
       case 3:
          kf_bfly3(fout,fstride[i]<<shift,st,m, fstride[i], m2);
          break;
       case 5:
          kf_bfly5(fout,fstride[i]<<shift,st,m, fstride[i], m2);
          break;
 #endif
       }
       m = m2;
    }
}

void renamenoise_fft_c(const renamenoise_fft_state *st,const renamenoise_fft_cpx *fin,renamenoise_fft_cpx *fout)
{
   int i;
   renamenoise_val16 scale;
   scale = st->scale;

   renamenoise_assert2 (fin != fout, "In-place FFT not supported");
   /* Bit-reverse the input */
   for (i=0;i<st->nfft;i++)
   {
      renamenoise_fft_cpx x = fin[i];
      fout[st->bitrev[i]].r = RENAMENOISE_SHR32(RENAMENOISE_MULT16_32_Q16(scale, x.r), scale_shift);
      fout[st->bitrev[i]].i = RENAMENOISE_SHR32(RENAMENOISE_MULT16_32_Q16(scale, x.i), scale_shift);
   }
   renamenoise_fft_impl(st, fout);
}


void renamenoise_ifft_c(const renamenoise_fft_state *st,const renamenoise_fft_cpx *fin,renamenoise_fft_cpx *fout)
{
   int i;
   renamenoise_assert2 (fin != fout, "In-place FFT not supported");
   /* Bit-reverse the input */
   for (i=0;i<st->nfft;i++)
      fout[st->bitrev[i]] = fin[i];
   for (i=0;i<st->nfft;i++)
      fout[i].i = -fout[i].i;
   renamenoise_fft_impl(st, fout);
   for (i=0;i<st->nfft;i++)
      fout[i].i = -fout[i].i;
}
