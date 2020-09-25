import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
from random import randint
import sys

plan=np.loadtxt(sys.argv[1])

maq = int(plan.shape[0]/3)
jobs = plan.shape[1]
op=np.floor(plan[:maq]/maq)

colors = []
delta = 0xFFFFFF//jobs
for i in range(jobs):
    #colors.append('#%06X' % randint(0, 0xFFFFFF))
    colors.append('#%06X' % (delta*i))

cmap = mpl.colors.ListedColormap(colors)
bounds = [i for i in range(jobs)]
norm = mpl.colors.BoundaryNorm(bounds, cmap.N)

fig,ax = plt.subplots()
#ax.set_xticks(np.arange(np.max(plan)))
for i in range(maq):
    bb = [(start,end-start) for start, end in zip(plan[maq+i],plan[2*maq+i])]    
    col = [colors[int(x)] for x in op[i]] 
    ax.broken_barh(bb, (i+.5, .5), facecolors = col)
ax.grid(True)
ax.figure.colorbar(mpl.cm.ScalarMappable(cmap=cmap, norm=norm),ax=ax)

"""
fig.colorbar(
    mpl.cm.ScalarMappable(cmap=cmap, norm=norm),
    cax=ax,
    boundaries=bounds,
    ticks=bounds,
)
"""
plt.show()
