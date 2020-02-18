import tensorflow as tf
from tensorflow.keras import models,layers,losses
import numpy as np
from scipy.io import loadmat
import os

# load data
data = loadmat('xdata.mat')
#X = data['xData'][:20,:3000]
X = data['xData']

# load params
params = (np.loadtxt('params.csv', delimiter=',')).astype(np.float)

# set up x and y
N = np.shape(X)[0]
N_train = int(N*0.8)
N_test = N - N_train

# train set
train_X = X[:N_train,:]
train_y = params[:N_train,:]

# test set
test_X = X[N_train:,:]
test_y = params[N_train:,:]

# Loss function
class DX7Error(losses.Loss):
    def call(self, y_true, y_pred):
        print(y_true)
        print(y_pred)
        return 0

# create tensorflow model
model = models.Sequential()
model.add(layers.Dense(2048, input_shape=((X.shape[1],)), activation='relu'))
model.add(layers.Dense(1024, activation='relu'))
model.add(layers.Dense(256, activation='relu'))
model.add(layers.Dense(155, activation='sigmoid'))

# compile and run model
# using Mean Squared Error as loss function for now 
# training on parameters for now
model.compile(optimizer='adam', loss='mse', metrics=['accuracy'])

# train model
history = model.fit(train_X, train_y, epochs=10, validation_data=(test_X, test_y))

# test model with test samples
test_data = loadmat('xtestdata.mat')
#X = test_data['xTestData'][:20,:3000]
X = test_data['xTestData']

# predict output params
predictions = model.predict(X)

# print(predictions)
# print(type(predictions))

# get number of example and number of params
# num_examples = predictions.shape[0]
# num_params = predictions.shape[1]

# save output params to file
np.savetxt('params_out.csv', predictions, delimiter=',')
# run ParamsToAudio to get output audio files
os.system('./ParamsToAudio --outdir=TestOutputData --paramfile=params_out.csv')

# train_y = []
# path = 'SamplesDir'
# for n in range(N_train):
#     train_y.append('{}/sample_{}.wav'.format(path,n))

# test_y = []
# path = 'TestDir'
# for n in range(N_train, N):
#     test_y.append('{}/sample_{}.wav'.format(path,n))