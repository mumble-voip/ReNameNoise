#ifndef RNN_DATA_H
#define RNN_DATA_H

#include "rnn.h"

struct ReNameNoiseModel {
  int input_dense_size;
  const ReNameNoiseDenseLayer *input_dense;

  int vad_gru_size;
  const ReNameNoiseGRULayer *vad_gru;

  int noise_gru_size;
  const ReNameNoiseGRULayer *noise_gru;

  int denoise_gru_size;
  const ReNameNoiseGRULayer *denoise_gru;

  int denoise_output_size;
  const ReNameNoiseDenseLayer *denoise_output;

  int vad_output_size;
  const ReNameNoiseDenseLayer *vad_output;
};

struct RNNState {
  const ReNameNoiseModel *model;
  float *vad_gru_state;
  float *noise_gru_state;
  float *denoise_gru_state;
};


#endif
