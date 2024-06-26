#!/usr/bin/python

#
#  Copyright (c) 2017, Jean-Marc Valin
#
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  - Redistributions of source code must retain the above copyright
#  notice, this list of conditions and the following disclaimer.
#
#  - Redistributions in binary form must reproduce the above copyright
#  notice, this list of conditions and the following disclaimer in the
#  documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from __future__ import print_function

from keras.models import Sequential
from keras.models import Model
from keras.layers import Input
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers import GRU
from keras.layers import SimpleRNN
from keras.layers import Dropout
from keras import losses
import h5py

from keras import backend as K
import numpy as np

print('Build model...')
main_input = Input(shape=(None, 22), name='main_input')
#x = Dense(44, activation='relu')(main_input)
#x = GRU(44, dropout=0.0, recurrent_dropout=0.0, activation='tanh', recurrent_activation='sigmoid', return_sequences=True)(x)
x=main_input
x = GRU(128, activation='tanh', recurrent_activation='sigmoid', return_sequences=True)(x)
#x = GRU(128, return_sequences=True)(x)
#x = GRU(22, activation='relu', return_sequences=True)(x)
x = Dense(22, activation='sigmoid')(x)
#x = Dense(22, activation='softplus')(x)
model = Model(inputs=main_input, outputs=x)

batch_size = 32

print('Loading data...')
with h5py.File('denoise_data.h5', 'r') as hf:
    all_data = hf['denoise_data'][:]
print('done.')

window_size = 500

nb_sequences = len(all_data)//window_size
print(nb_sequences, ' sequences')
x_train = all_data[:nb_sequences*window_size, :-22]
x_train = np.reshape(x_train, (nb_sequences, window_size, 22))

y_train = np.copy(all_data[:nb_sequences*window_size, -22:])
y_train = np.reshape(y_train, (nb_sequences, window_size, 22))

#y_train = -20*np.log10(np.add(y_train, .03));

all_data = 0;
x_train = x_train.astype('float32')
y_train = y_train.astype('float32')

print(len(x_train), 'train sequences. x shape =', x_train.shape, 'y shape = ', y_train.shape)

# try using different optimizers and different optimizer configs
model.compile(loss='mean_squared_error',
              optimizer='adam',
              metrics=['binary_accuracy'])

print('Train...')
model.fit(x_train, y_train,
          batch_size=batch_size,
          epochs=200,
          validation_data=(x_train, y_train))
model.save("newweights.hdf5")
