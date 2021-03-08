import numpy as np
from scipy.stats import kruskal
import matplotlib.pyplot as plt
import os 
import sys 

res = np.zeros((2,80,100))
names = []
colors = ["r","g"]
for l,i in enumerate(sys.argv[1:]):
    names.append(i)
    for k,j in enumerate(sorted(os.listdir(i))):
        res[l,k,:] = np.loadtxt(i+j)

for i in range(80):
    s,p = kruskal(res[0,i],res[1,i])
    same = False
    if p > .05:
        same = True
    print(np.median(res[0,i]),np.median(res[1,i]),"Same ",same)
    print("best ",np.min(res[0,i]),np.min(res[1,i]))
for i in range(2):
    plt.plot(res[i].min(1),colors[i]+"o",label = names[i])
    plt.plot(np.median(res[i],axis=1),colors[i]+"--")

plt.grid(True)
plt.plot(np.loadtxt("/home/cacao/cimat/proyectotec/bestres/dmu_best.txt"),"b*",label="Estado del arte")
plt.legend()
plt.show()


#points = dict(markersize=2,markerfacecolor = 'k')
#for i in [0,40]:
#    plt.boxplot(res[int(i):int(i)+40],flierprops=points,whis=(0,100))
#    plt.plot(np.arange(1,41,1),best[int(i):int(i)+40],"o",label="Estado del arte")
#    plt.plot(np.arange(1,41,1),bestils[int(i):int(i)+40],"o",label="Mejores obtenidos")
#    #plt.grid(True)
#    plt.plot()
#    plt.xticks(np.arange(1,41,1),labels=np.arange(int(i)+1,int(i)+41,1))
#    plt.legend()
#    ##plt.savefig("dmu"+str(i+1)+"_"+str(i+40)+".png")
#    plt.show()
#        
