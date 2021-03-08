import numpy as np
import sys
import matplotlib.pyplot as plt

results = []
for i in sys.argv[1:]:
    results+=[np.loadtxt(i)]
    print(i)

x = np.arange(80)+1
names = ["Mejores conocidos","ILS con vecindad extendida","ILS","ILS con fitness"]

for l in range(2):
    plt.grid(True)
    for i in results:
        plt.plot(x[40*l:40*l+40],i[40*l:40*l+40],"o")
    plt.legend(names)
    plt.savefig("best"+str(l+1)+".png")
    plt.show()
