# -*- coding: utf-8 -*-
from keras.layers import Input,Conv2D,MaxPooling2D,ZeroPadding2D
from keras.layers import Flatten,BatchNormalization,Permute,TimeDistributed,Dense,Bidirectional,GRU
from keras.models import Model

rnnunit  = 256
from keras import backend as K

from keras.layers import Lambda
from keras.optimizers import SGD

def ctc_lambda_func(args):
    y_pred, labels, input_length, label_length = args
    y_pred = y_pred[:, 2:, :]
    return K.ctc_batch_cost(labels, y_pred, input_length, label_length)


def get_model(height,nclass):
    
    input = Input(shape=(height,None,1),name='the_input')
    m = Conv2D(64,kernel_size=(3,3),activation='relu',padding='same',name='conv1')(input)
    m = MaxPooling2D(pool_size=(2,2),strides=(2,2),name='pool1')(m)
    m = Conv2D(128,kernel_size=(3,3),activation='relu',padding='same',name='conv2')(m)
    m = MaxPooling2D(pool_size=(2,2),strides=(2,2),name='pool2')(m)
    m = Conv2D(256,kernel_size=(3,3),activation='relu',padding='same',name='conv3')(m)
    m = Conv2D(256,kernel_size=(3,3),activation='relu',padding='same',name='conv4')(m)

    m = ZeroPadding2D(padding=(0,1))(m)
    m = MaxPooling2D(pool_size=(2,2),strides=(2,1),padding='valid',name='pool3')(m)

    m = Conv2D(512,kernel_size=(3,3),activation='relu',padding='same',name='conv5')(m)
    m = BatchNormalization(axis=1)(m)
    m = Conv2D(512,kernel_size=(3,3),activation='relu',padding='same',name='conv6')(m)
    m = BatchNormalization(axis=1)(m)
    m = ZeroPadding2D(padding=(0,1))(m)
    m = MaxPooling2D(pool_size=(2,2),strides=(2,1),padding='valid',name='pool4')(m)
    m = Conv2D(512,kernel_size=(2,2),activation='relu',padding='valid',name='conv7')(m)

    m = Permute((2,1,3),name='permute')(m)
    m = TimeDistributed(Flatten(),name='timedistrib')(m)

    m = Bidirectional(GRU(rnnunit,return_sequences=True),name='blstm1')(m)
    m = Dense(rnnunit,name='blstm1_out',activation='linear')(m)
    m = Bidirectional(GRU(rnnunit,return_sequences=True),name='blstm2')(m)
    y_pred = Dense(nclass,name='blstm2_out',activation='softmax')(m)

    basemodel = Model(inputs=input,outputs=y_pred)

    labels = Input(name='the_labels', shape=[None,], dtype='float32')
    input_length = Input(name='input_length', shape=[1], dtype='int64')
    label_length = Input(name='label_length', shape=[1], dtype='int64')
    loss_out = Lambda(ctc_lambda_func, output_shape=(1,), name='ctc')([y_pred, labels, input_length, label_length])
    model = Model(inputs=[input, labels, input_length, label_length], outputs=[loss_out])
    sgd = SGD(lr=0.001, decay=1e-6, momentum=0.9, nesterov=True, clipnorm=5)
    #model.compile(loss={'ctc': lambda y_true, y_pred: y_pred}, optimizer='adadelta')
    model.compile(loss={'ctc': lambda y_true, y_pred: y_pred}, optimizer=sgd)
    model.summary()
    return model,basemodel

