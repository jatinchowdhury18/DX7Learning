import numpy as np

num_params = 22 * 6 + 23
num_examples = 4

params_matrix = np.random.rand(num_examples, num_params)
np.savetxt('params.csv', params_matrix, delimiter=',')
