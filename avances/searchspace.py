import numpy as np
import random

x = np.random.rand(10)
y = np.random.rand(10)
z = np.zeros(10)

# nodos
for i in range(10):
    print(x[i]," ",y[i]," ",z[i])

# aristas

random.shuffle(x)
random.shuffle(y)
for i in range(10):
    print(x[i]," ",y[i]," ",z[i])

