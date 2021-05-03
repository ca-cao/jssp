import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl

nl = 10
np.random.seed(3)
delta = np.zeros((nl,2))
delta[:,0] = 1-2*(np.random.rand(nl))
delta[:,1] = (1-2*(np.random.rand(nl)<.7))*np.sqrt(1- delta[:,0]**2)
lopt = np.cumsum(delta,0)

for x in lopt:
    # numero de vecinos 3-5
    nv = int(np.random.randint(4,7))
    # angulos
    theta = np.zeros(nv)+2*np.pi/(nv)
    theta[0] = 2*np.pi*np.random.rand()
    theta = np.cumsum(theta,0)
    # puntos
    same=(x == lopt[-1]).all()
    for t in theta:
        if same:
            plt.plot([x[0],x[0]+.4*np.cos(t)],[x[1],x[1]+.4*np.sin(t)],"k--",alpha=.7)
        else:
            plt.plot([x[0],x[0]+.4*np.cos(t)],[x[1],x[1]+.4*np.sin(t)],"k-",alpha=.7)
    if same:
        plt.plot(x[0]+.4*np.cos(theta),x[1]+.4*np.sin(theta),"k+")
    else:
        plt.plot(x[0]+.4*np.cos(theta),x[1]+.4*np.sin(theta),"k*")


plt.plot(lopt[:,0],lopt[:,1])
plt.plot(lopt[:,0],lopt[:,1],"o")
plt.axis("off")
plt.show()
