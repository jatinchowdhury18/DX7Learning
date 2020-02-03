import tensorflow as tf
from tensorflow.keras import models,layers,losses
import numpy as np
from scipy.io import loadmat

# load data
data = loadmat('xdata.mat')
X = data['xData'][:20,:3000]

# set up x and y
N = np.shape(X)[0]
N_train = int(N*0.8)
N_test = N - N_train
train_X = X[:N_train,:]

train_y = []
path = 'SamplesDir'
for n in range(N_train):
    train_y.append('{}/sample_{}.wav'.format(path,n))

test_X = X[N_train:,:]
test_y = []
path = 'TestDir'
for n in range(N_train, N):
    test_y.append('{}/sample_{}.wav'.format(path,n))

# Loss function
class DX7Error(losses.Loss):
    def call(self, y_true, y_pred):
        print(y_true)
        print(y_pred)
        return 0

# create tensorflow model
model = models.Sequential()
model.add(layers.Dense(2048, input_shape=((3000,)), activation='relu'))
model.add(layers.Dense(1024, activation='relu'))
model.add(layers.Dense(256, activation='relu'))
model.add(layers.Dense(155, activation='sigmoid'))

# compile and run model
model.compile(optimizer='adam', loss=DX7Error, metrics=['accuracy'])

history = model.fit(train_X, train_y, epochs=10, validation_data=(test_X, test_y))
