import numpy as np
from scipy.io import wavfile
import scipy.signal as signal

def loss_func(des_wav, test_wav):
    # Read files
    _, x = wavfile.read(des_wav)
    _, y = wavfile.read(test_wav)

    # Normalize
    xL = x[:,0] / 2**15; xR = x[:,1] / 2**15
    yL = y[:,0] / 2**15; yR = y[:,1] / 2**15
    
    # Compute STFT
    N = 1024 # window length
    _,_,xLSTFT = signal.stft(xL, nperseg=N)
    _,_,xRSTFT = signal.stft(xR, nperseg=N)
    _,_,yLSTFT = signal.stft(yL, nperseg=N)
    _,_,yRSTFT = signal.stft(yR, nperseg=N)

    # Compute L1 loss
    lossL = np.sum(np.abs(xLSTFT - yLSTFT))
    lossR = np.sum(np.abs(xRSTFT - yRSTFT))

    return lossL + lossR
