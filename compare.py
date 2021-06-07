import numpy as np
from scipy.stats import kruskal
from scipy.stats import wilcoxon
import matplotlib.pyplot as plt
import os 
import sys 

res = np.zeros((len(sys.argv)-1,80,50))
names = []

cl = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']
np.random.shuffle(cl)
for l,i in enumerate(sys.argv[1:]):
    if i[-1]!="/":
        i=i+"/"
    names.append(i[:-1])
    for k,j in enumerate(sorted(os.listdir(i))):
        #print(i+j)
        res[l,k,:] = np.loadtxt(i+j)[:50]

# ver cual gana mas
#plt.title("Minimo")
#med = np.min(res,axis=2)
#best = np.argmin(med,0)
#plt.xticks(np.arange(len(names))+.5,labels=names)
#xt=plt.hist(np.argmin(med,0),bins=np.arange(len(names)+1))
#plt.show()
#
#plt.title("Medianas")
med = np.median(res,axis=2)
#best = np.argmin(med,0)
#plt.xticks(np.arange(len(names))+.5,labels=names)
#xt=plt.hist(np.argmin(med,0),bins=np.arange(len(names)+1))
#plt.show()

wins = np.zeros(len(names))

heat = np.zeros((len(names),len(names)))
# comparar todos los pares y anotar quien gana o pierde
for k in range(80):
    for i in range(len(names)-1):
        for j in range(i+1,len(names)):
            #s,p = kruskal(res[i,k],res[j,k])
            if all(res[i,k]==res[j,k]):
                p=1
            else:
                s,p = wilcoxon(res[i,k],res[j,k])
            # si son diferentes asignar los puntos
            if p < .05:
                win = 1*(med[i,k] < med[j,k])-1*(med[i,k] > med[j,k])
                wins[i]+= win 
                wins[j]-= win
                heat[i,j]+=win
                heat[j,i]-=win


#xt=plt.plot(wins)
#plt.xticks(np.arange(len(names)),labels=names)
#plt.plot()
#plt.title("Numero de comparaciones ganadas")
#plt.show()

# heatmap
fig, ax = plt.subplots()
im = ax.imshow(heat)
# We want to show all ticks...
ax.set_xticks(np.arange(len(names)))
ax.set_yticks(np.arange(len(names)))
# ... and label them with the respective list entries
ax.set_xticklabels(names)
ax.set_yticklabels(names)
ax.set_xlim(-.5,len(names)-.5)
ax.set_ylim(-.5,len(names)-.5)

# Rotate the tick labels and set their alignment.
plt.setp(ax.get_xticklabels(), rotation=45, ha="right",
         rotation_mode="anchor")

# Loop over data dimensions and create text annotations.
for i in range(len(names)):
    for j in range(len(names)):
        text = ax.text(j, i, heat[i, j],
                       ha="center", va="center", color="w")

ax.set_title("Comparaciones ganadas o perdidas")
fig.tight_layout()
plt.show()
xx = np.arange(80)+1
best = np.loadtxt("/home/cacao/cimat/proyectotec/bestres/dmu_best.txt")
for i in range(len(names)):
    #plt.plot(res[i].min(1),"o",label = names[i],markersize=3,color=cl[i],alpha=.7)
    #plt.plot(np.median(res[i],axis=1),"o",markersize=4,color=cl[i],alpha =.7,label=names[i])
    plt.plot(xx,best/np.median(res[i],axis=1),"o",markersize=4,color=cl[i],alpha =.7,label=names[i])
plt.title("Medianas")
plt.grid(True)
plt.legend()
plt.show()

for i in range(len(names)):
    plt.plot(xx,best/res[i].min(1),"o",label = names[i],markersize=3,color=cl[i],alpha=.7)
plt.title("Minimos")

plt.grid(True)
#plt.plot(best,"k*",label="Estado del arte")
plt.legend()
plt.show()
plt.plot(res[i].min(1) - best)
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
