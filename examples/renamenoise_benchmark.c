/*
   Copyright (c) 2024, The Mumble Developers

   All rights reserved.

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

#include "../src/rnn_data.h"
#include "renamenoise.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

#define RENAMENOISE_FRAME_SIZE_SHIFT 2
#define RENAMENOISE_FRAME_SIZE (120 << RENAMENOISE_FRAME_SIZE_SHIFT)
#define RENAMENOISE_PITCH_MIN_PERIOD 60
#define RENAMENOISE_PITCH_MAX_PERIOD 768
#define RENAMENOISE_PITCH_FRAME_SIZE 960
#define RENAMENOISE_PITCH_BUF_SIZE (RENAMENOISE_PITCH_MAX_PERIOD + RENAMENOISE_PITCH_FRAME_SIZE)
#define RENAMENOISE_NB_BANDS 22
#define RENAMENOISE_CEPS_MEM 8

float NOISE_EXPECTED_RESULT[48] = {0.000000, 0.000000, 0.000000, 0.000000, 1.060423, 3.027331, 0.000000, 0.000000, 2.720507, 0.614394,
								   0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 2.829938, 0.000000, 0.000000, 0.000000, 0.000000,
								   0.000000, 0.000000, 1.026295, 0.000000, 0.000000, 0.000000, 0.000000, 1.563959, 0.000000, 5.238552,
								   3.068555, 0.000000, 0.000000, 1.012800, 0.000000, 0.000000, 0.000000, 1.289652, 0.000000, 0.000000,
								   0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000};

struct ReNameNoiseDenoiseState {
	float analysis_mem[RENAMENOISE_FRAME_SIZE];
	float cepstral_mem[RENAMENOISE_CEPS_MEM][RENAMENOISE_NB_BANDS];
	int memid;
	float synthesis_mem[RENAMENOISE_FRAME_SIZE];
	float pitch_buf[RENAMENOISE_PITCH_BUF_SIZE];
	float pitch_enh_buf[RENAMENOISE_PITCH_BUF_SIZE];
	float last_gain;
	int last_period;
	float mem_hp_x[2];
	float lastg[RENAMENOISE_NB_BANDS];
	ReNameNoiseRNNState rnn;
};

double test(ReNameNoiseRNNState *rnn) {
	float noise_input[RENAMENOISE_MAX_NEURONS * 3] = {0};

	for (int i = 0; i < RENAMENOISE_MAX_NEURONS * 3; i++) {
		noise_input[i] = ((i % 11) * 0.1f) + 0.5f;
	}

	clock_t begin = clock();

	for (int i = 0; i < 100000; i++) {
		renamenoise_compute_gru(rnn->model->noise_gru, rnn->noise_gru_state, noise_input);
	}

	clock_t end = clock();

	double time_spent_clocks = (double) (end - begin);

	for (int i = 0; i < rnn->model->noise_gru->nb_neurons; i++) {
		if (fabs(rnn->noise_gru_state[i] - NOISE_EXPECTED_RESULT[i]) > 0.000001f) {
			printf("Result %d is not the expected value! %f != %f\n", i, rnn->noise_gru_state[i], NOISE_EXPECTED_RESULT[i]);
			exit(1);
		}
	}

	return time_spent_clocks;
}

int main() {
	ReNameNoiseDenoiseState *st;
	st = renamenoise_create(NULL);

	double total = 0.0;
	double count = 0.0;
	for (int i = 0; i < 5; i++) {
		double time_spent_clocks = test(&st->rnn);
		total += time_spent_clocks;
		count += 1;

		double time_spent_seconds = time_spent_clocks / CLOCKS_PER_SEC;
		printf("Time spent: %f clocks = %f seconds\n", time_spent_clocks, time_spent_seconds);
	}

	double time_spent_clocks = total / count;
	double time_spent_seconds = time_spent_clocks / CLOCKS_PER_SEC;
	printf("Time spent average: %f clocks = %f seconds\n", time_spent_clocks, time_spent_seconds);

	renamenoise_destroy(st);
	return 0;
}
