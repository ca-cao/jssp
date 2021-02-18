import numpy as np
import sys

# programa para crear una instancia aleatoria de tamano dado 
njobs,nmaq = int(sys.argv[1]),int(sys.argv[2])

seq = np.arange(nmaq)
print("{} {}".format(njobs,nmaq))
for i in range(njobs):
    np.random.shuffle(seq) 
    for m in seq:
        time = np.random.randint(1,100)
        print("{} {}\t".format(m,time),end="")
    print("")


