import tensorflow as tf
from tensorflow.keras import models,layers,losses
import numpy as np
from scipy.io import loadmat, wavfile
import scipy.signal as signal
import os
import matplotlib.pyplot as plt

# load data
# data = loadmat('xdata.mat')
# X = data['xData'][:20,:3000]
# X = data['xData']

path = 'Data/Samples20k'
N_samples = 5000 # 20000

samples = []
for n in range(N_samples):
    fs, x = wavfile.read('{}/sample_{}.wav'.format(path, n)) # read file
    x = x / 2**15                           # normalize
    x = np.concatenate((x[:,0], x[:,1]))    # concatenate L+R channels
    samples.append(x)

X = np.asarray(samples)

# load params
params = (np.loadtxt('Data/params20k.csv', delimiter=',')).astype(np.float)

# set up x and y
N = np.shape(X)[0]
N_train = int(N*0.95)
N_test = N - N_train

# train set
train_X = X[:N_train,:]
train_y = params[:N_train,:]

# test set
test_X = X[N_train:,:]
test_y = params[N_train:N,:]

print(np.shape(X))
print(np.shape(train_X))
print(np.shape(train_y))
print(np.shape(test_X))
print(np.shape(test_y))

# create tensorflow model
model = models.Sequential()
model.add(layers.InputLayer(input_shape=((X.shape[1],))))
model.add(layers.Dense(8*4096, activation='relu'))
# model.add(layers.Dense(8192, activation='relu'))
model.add(layers.Dense(4096, activation='relu'))
# model.add(layers.Dense(2048, activation='relu'))
model.add(layers.Dense(1024, activation='relu'))
model.add(layers.Dense(256, activation='relu'))
model.add(layers.Dense(155, activation='sigmoid'))

''' POSSIBLE NEURAL NETWORKS TO USE '''
################################################################################################
def rnn_model(input_dim, units, recur_layers, dropout_rate, output_dim=155):
''' This RNN uses GRUs '''
    input_data = layers.Input(name='input', shape = (input_dim,))  #shape=(None, input_dim))
    
    for i in range(recur_layers):
        if i == 0:
            gru = GRU(units, activation='relu', 
                    return_sequences=True, implementation=2, name='gru'+str(i))(input_data)
            bn_rnn = BatchNormalization()(gru)
            dropout_rnn = Dropout(dropout_rate)(bn_rnn)
        else:
            gru = GRU(units, activation='relu', 
                    return_sequences=True, implementation=2, name='gru'+str(i))(dropout_rnn)
            bn_rnn = BatchNormalization()(gru)
            dropout_rnn = Dropout(dropout_rate)(bn_rnn)
            
    time_dense = TimeDistributed(Dense(output_dim))(dropout_rnn)
    # add sigmoid activation layer
    y_pred = Activation('sigmoid', name='sigmoid')(time_dense)
    
    model = models.Model(inputs=input_data, outputs=y_pred)
    print(model.summary())
    return model

# model with 5 hidden layers
model_1 = rnn_model(input_dim=X.shape[1], units=200, dropout_rate=0.8, recur_layers=4)


###################################################################################
def cnn_rnn_model(input_dim, filters, kernel_size, conv_stride, conv_border_mode, units, output_dim=155):
''' CNN + RNN for spectrogram '''
    input_data = Input(name='input', shape = (input_dim,))
    
    # convolutional layer
    conv_1d = Conv1D(filters, kernel_size, strides=conv_stride, padding=conv_border_mode, activation='relu',name='conv1d')(input_data)
    # batch normalization
    bn_cnn = BatchNormalization(name='bn_conv_1d')(conv_1d)
    # recurrent layer
    simp_rnn = SimpleRNN(units, activation='relu', return_sequences=True, implementation=2, name='rnn')(bn_cnn)
    # batch normalization
    bn_rnn = BatchNormalization()(simp_rnn)
    # TimeDistributed(Dense(output_dim)) layer
    time_dense = TimeDistributed(Dense(output_dim))(bn_rnn)
    
    # sigmoid activation layer
    y_pred = Activation('sigmoid', name='sigmoid')(time_dense)

    model = models.Model(inputs=input_data, outputs=y_pred)
    #model.output_length = lambda x: cnn_output_length(x, kernel_size, conv_border_mode, conv_stride)
    print(model.summary())
    return model

model_2 = cnn_rnn_model(input_dim=X.shape[1],
                        filters=200,
                        kernel_size=11, 
                        conv_stride=2,
                        conv_border_mode='valid',
                        units=200)
###############################################################################


# compile and run model
# using Mean Squared Error as loss function for now 
# training on parameters for now
model.compile(optimizer='adam', loss='mean_squared_error', metrics=['accuracy'])

# train model
history = model.fit(train_X, train_y, epochs=50, validation_data=(test_X, test_y))

# plot losses
train_loss = history.history['loss']
test_loss =  history.history['val_loss']
plt.plot(train_loss)
plt.plot(test_loss)
plt.savefig('Loss_mse.png')

# Spectrogram loss function
def spec_loss(x_wav, y_wav, fs):
    nseg = 2048
    _,_,Z_x = signal.stft(x_wav, fs=fs, nperseg=nseg, nfft=nseg*2, axis=0)
    _,_,Z_y = signal.stft(y_wav, fs=fs, nperseg=nseg, nfft=nseg*2, axis=0)

    loss = np.mean(np.abs(Z_x - Z_y), axis=None)
    return loss

def test_predictions(predictions, x_dir):
    # get number of example and number of params
    num_examples = predictions.shape[0]
    num_params = predictions.shape[1]

    # save output params to file
    np.savetxt('params_out.csv', predictions, delimiter=',')
    # run ParamsToAudio to get output audio files
    os.system('./ParamsToAudio --outdir=TestOutputData --paramfile=params_out.csv > /dev/null 2>&1')

    # Load files to compare
    y_dir = 'TestOutputData'

    total_loss = 0
    for n in range(num_examples):
        fs, x_wav = wavfile.read('{}/sample_{}.wav'.format(x_dir, n))
        fs, y_wav = wavfile.read('{}/sample_{}.wav'.format(y_dir, n))

        total_loss += spec_loss(x_wav, y_wav, fs)

    return total_loss  

# predict output params
predictions = np.random.random_sample(np.shape(test_y))
spectrogram_loss = test_predictions(predictions, path)
print('Random loss: {}'.format(spectrogram_loss))

predictions = model.predict(test_X)
spectrogram_loss = test_predictions(predictions, path)
print('Model loss: {}'.format(spectrogram_loss))
