#!/usr/bin/python

#
#  Copyright (c) 2024, The Mumble Developers
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
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers import GRU
from keras.models import load_model
from keras import backend as K
import sys
import re
import numpy as np

def printVector(f, ft, vector, name):
    v = np.reshape(vector, (-1));
    #print('static const float ', name, '[', len(v), '] = \n', file=f)
    f.write('static const renamenoise_rnn_weight {}[{}] = {{\n   '.format(name, len(v)))
    for i in range(0, len(v)):
        f.write('{}'.format(min(127, int(round(256*v[i])))))
        ft.write('{}'.format(min(127, int(round(256*v[i])))))
        if (i!=len(v)-1):
            f.write(',')
        else:
            break;
        ft.write(" ")
        if (i%8==7):
            f.write("\n   ")
        else:
            f.write(" ")
    #print(v, file=f)
    f.write('\n};\n\n')
    ft.write("\n")
    return;

def printLayer(f, ft, layer):
    weights = layer.get_weights()
    activation = re.search('function (.*) at', str(layer.activation)).group(1).upper()
    if len(weights) > 2:
        ft.write('{} {} '.format(weights[0].shape[0], weights[0].shape[1]/3))
    else:
        ft.write('{} {} '.format(weights[0].shape[0], weights[0].shape[1]))
    if activation == 'SIGMOID':
        ft.write('1\n')
    elif activation == 'RELU':
        ft.write('2\n')
    else:
        ft.write('0\n')
    name = "renamenoise_" + layer.name
    printVector(f, ft, weights[0], name + '_weights')
    if len(weights) > 2:
        printVector(f, ft, weights[1], name + '_recurrent_weights')
    printVector(f, ft, weights[-1], name + '_bias')
    if len(weights) > 2:
        f.write('static const ReNameNoiseGRULayer {} = {{\n   {}_bias,\n   {}_weights,\n   {}_recurrent_weights,\n   {}, {}, ACTIVATION_{}\n}};\n\n'
                .format(name, name, name, name, weights[0].shape[0], weights[0].shape[1]/3, activation))
    else:
        f.write('static const ReNameNoiseDenseLayer {} = {{\n   {}_bias,\n   {}_weights,\n   {}, {}, ACTIVATION_{}\n}};\n\n'
                .format(name, name, name, weights[0].shape[0], weights[0].shape[1], activation))

def structLayer(f, layer):
    weights = layer.get_weights()
    name = "renamenoise_" + layer.name
    if len(weights) > 2:
        f.write('    {},\n'.format(weights[0].shape[1]/3))
    else:
        f.write('    {},\n'.format(weights[0].shape[1]))
    f.write('    &{},\n'.format(name))


def foo(c, name):
    return None

def mean_squared_sqrt_error(y_true, y_pred):
    return K.mean(K.square(K.sqrt(y_pred) - K.sqrt(y_true)), axis=-1)


model = load_model(sys.argv[1], custom_objects={'msse': mean_squared_sqrt_error, 'mean_squared_sqrt_error': mean_squared_sqrt_error, 'my_crossentropy': mean_squared_sqrt_error, 'mycost': mean_squared_sqrt_error, 'WeightClip': foo})

weights = model.get_weights()

f = open(sys.argv[2], 'w')
ft = open(sys.argv[3], 'w')

f.write('/*This file is automatically generated from a Keras model*/\n\n')
f.write('#ifdef HAVE_CONFIG_H\n#include "config.h"\n#endif\n\n#include "rnn.h"\n#include "rnn_data.h"\n\n')
ft.write('renamenoise-nu model file version 1\n')

layer_list = []
for i, layer in enumerate(model.layers):
    if len(layer.get_weights()) > 0:
        printLayer(f, ft, layer)
    if len(layer.get_weights()) > 2:
        layer_list.append("renamenoise_" + layer.name)

f.write('const struct ReNameNoiseModel renamenoise_model_{} = {{\n'.format(sys.argv[4]))
for i, layer in enumerate(model.layers):
    if len(layer.get_weights()) > 0:
        structLayer(f, layer)
f.write('};\n')

#hf.write('struct ReNameNoiseRNNState {\n')
#for i, name in enumerate(layer_list):
#    hf.write('  float {}_state[{}_SIZE];\n'.format(name, name.upper())) 
#hf.write('};\n')

f.close()
