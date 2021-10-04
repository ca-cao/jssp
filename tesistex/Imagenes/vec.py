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

ns = 100
space = np.zeros((ns,2))
space[:,0] = lopt[:,0].min()-1+((lopt[:,0].max()+2-lopt[:,0].min())*np.random.rand(ns))
space[:,1] = lopt[:,1].min()-1+((lopt[:,1].max()+2-lopt[:,1].min())*np.random.rand(ns))

mask = []
for j,x in enumerate(space):
    for i in space:
        if np.linalg.norm(x-i)<1 and any(j!=i):
            mask+=[j]
            break

print(mask)

x = np.linspace(space[:,0].min(),space[:,0].max(),100)
y = np.linspace(space[:,1].min(),space[:,1].max(),100)

X,Y = np.meshgrid(x,y)

# funcion manosa
def f(x,y):
    return (x-lopt[-1,0])**2+(y-lopt[-1,1])**2

Z = f(X,Y)
plt.contourf(X,Y,Z,levels=np.linspace(0,Z.max(),70))
# muy ingenuo
for k,x in enumerate(space):
    for i in lopt:
        if np.linalg.norm(x-i)>2.5 and not(k in mask):
            plt.plot(x[0],x[1],"k*",alpha=.1)

plt.plot(lopt[:,0],lopt[:,1])
plt.plot(lopt[:,0],lopt[:,1],"o")
plt.axis("off")

plt.show()
