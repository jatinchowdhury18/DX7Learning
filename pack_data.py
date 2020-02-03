import numpy as np
from scipy.io import wavfile, savemat
import matplotlib.pyplot as plt

path = 'SamplesDir'
N_samples = 500

samples = []
for n in range(N_samples):
    fs, x = wavfile.read('{}/sample_{}.wav'.format(path, n)) # read file
    x = x / 2**15                           # normalize
    x = np.concatenate((x[:,0], x[:,1]))    # concatenate L+R channels
    samples.append(x)

# save to .mat file
dict = { 'xData': np.asarray(samples) }
savemat('xdata.mat', dict)
