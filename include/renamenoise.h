/* Copyright (c) 2018 Gregor Richards
 * Copyright (c) 2017 Mozilla */
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
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef RENAMENOISE_H
#define RENAMENOISE_H 1

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RENAMENOISE_EXPORT
# if defined(WIN32)
#  if defined(RENAMENOISE_BUILD) && defined(DLL_EXPORT)
#   define RENAMENOISE_EXPORT __declspec(dllexport)
#  else
#   define RENAMENOISE_EXPORT
#  endif
# elif defined(__GNUC__) && defined(RENAMENOISE_BUILD)
#  define RENAMENOISE_EXPORT __attribute__ ((visibility ("default")))
# else
#  define RENAMENOISE_EXPORT
# endif
#endif

typedef struct ReNameNoiseDenoiseState ReNameNoiseDenoiseState;
typedef struct RNNModel RNNModel;

/**
 * Return the size of ReNameNoiseDenoiseState
 */
RENAMENOISE_EXPORT int rnnoise_get_size();

/**
 * Return the number of samples processed by rnnoise_process_frame at a time
 */
RENAMENOISE_EXPORT int rnnoise_get_frame_size();

/**
 * Initializes a pre-allocated ReNameNoiseDenoiseState
 *
 * If model is NULL the default model is used.
 *
 * See: rnnoise_create() and rnnoise_model_from_file()
 */
RENAMENOISE_EXPORT int rnnoise_init(ReNameNoiseDenoiseState *st, RNNModel *model);

/**
 * Allocate and initialize a ReNameNoiseDenoiseState
 *
 * If model is NULL the default model is used.
 *
 * The returned pointer MUST be freed with rnnoise_destroy().
 */
RENAMENOISE_EXPORT ReNameNoiseDenoiseState *rnnoise_create(RNNModel *model);

/**
 * Free a ReNameNoiseDenoiseState produced by rnnoise_create.
 *
 * The optional custom model must be freed by rnnoise_model_free() after.
 */
RENAMENOISE_EXPORT void rnnoise_destroy(ReNameNoiseDenoiseState *st);

/**
 * Denoise a frame of samples
 *
 * in and out must be at least rnnoise_get_frame_size() large.
 */
RENAMENOISE_EXPORT float rnnoise_process_frame(ReNameNoiseDenoiseState *st, float *out, const float *in);

/**
 * Load a model from a file
 *
 * It must be deallocated with rnnoise_model_free()
 */
RENAMENOISE_EXPORT RNNModel *rnnoise_model_from_file(FILE *f);

/**
 * Free a custom model
 *
 * It must be called after all the ReNameNoiseDenoiseStates referring to it are freed.
 */
RENAMENOISE_EXPORT void rnnoise_model_free(RNNModel *model);

#ifdef __cplusplus
}
#endif

#endif
