import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
from random import randint
from matplotlib import cm
import sys

plan=np.loadtxt(sys.argv[1])


maq = int(plan.shape[0]/3)
jobs = plan.shape[1]
op=np.floor(plan[:maq]/maq)


colors = []
delta = 0xFFFFFF//jobs
rainbow = cm.get_cmap('tab10',jobs)
for i in range(jobs):
    colors.append(rainbow((1/(jobs-1))*i))
    

cmap = mpl.colors.ListedColormap(colors)
bounds = [i for i in range(jobs)]
norm = mpl.colors.BoundaryNorm(bounds, cmap.N)

fig,ax = plt.subplots()

if len(sys.argv)>2:
    rc = np.loadtxt(sys.argv[2])
    ax.plot(rc[:,1],rc[:,0]+.3,"k-",alpha=.7) 

#ax.set_xticks(np.arange(np.max(plan)))
for i in range(maq):
    bb = [(start,end-start) for start, end in zip(plan[maq+i],plan[2*maq+i])]    
    col = [colors[int(x)] for x in op[i]] 
    ax.broken_barh(bb, (i, .6), facecolors = col)

#ax.grid(True)
#ax.xaxis.grid() # vertical lines
#ax.set_yticklabels(["Mejorar la \nfunción de\n fitness","Cambiar la \nrepresentación\n (nodos)","Plantear nueva\n vecindad","Redacción\n de tesis"])
ax.set_yticks(maq -.7-np.arange(maq))
#ax.set_xticks(np.arange(6)*4)
#ax.set_xticklabels(["Febrero","Marzo","Abril","Mayo","Junio","Julio"])
ax.figure.colorbar(mpl.cm.ScalarMappable(cmap=cmap, norm=norm),ax=ax)
ax.set_title(sys.argv[1])
ax.set_xlabel("Tiempo")
ax.set_ylabel("Máquina")
plt.show()
