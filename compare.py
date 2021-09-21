import numpy as np
from scipy.stats import kruskal
from scipy.stats import wilcoxon
import matplotlib.pyplot as plt
import os 
import sys 
import re

# construir un diccionario para nombres

regexps  = [
    # pr
    #('.*pr.*','RpKeTup'),
    # cmax
    #('(?i).*[^i]cmax.*', r'$C_{max}$'),
    ('(?i).*[^i]cmax.*', 'Cmax'),
    # dist2
    #('.*dist2',r'$Var(C_i)$'),
    ('.*dist2','VarC'),
    # flowtime
    #('.*flowtime',r'$\sum\,\,J_i$'),
    ('.*flowtime','Flow'),
    # ftimes
    #('.*ftimes',r'$(\{C_i\})$'),
    ('.*ftimes','Tup'),
    # Icmax
    ('.*[I]cmax','Icmax'),
    # rcsize
    ('.*rcsize','Rc'),
    # totime2
    ('.*totime2','C2'),
    # tuple
    #('.*7tuple',r'$\left(\sum C^2_i,\sum\,\,J_i,Var(C_i)\right)$'),
    ('.*tuple','C2/Flow/VarC')
]

def lookup(s):
    for pattern, value in regexps:
        if re.search(pattern, s):
            return value
    return s

def getname(s):
    name =''
    # representación
    if 'pr' in s:
        return "RpKeTup"
    # vecindad
    if 'n7' in s:
        name = name+"PN7"
    elif 'n8' in s:
        name = name+"PN7ext"
    return name +lookup(s)

res = np.zeros((len(sys.argv)-1,80,50))
names = []
for l,i in enumerate(sys.argv[1:]):
    if i[-1]!="/":
        i=i+"/"
    #names.append(lookup(i[:-1]))
    #names.append(i[:-1])
    names.append(getname(i[:-1]))
    for k,j in enumerate(sorted(os.listdir(i))):
        #print(i+j)
        res[l,k,:] = np.loadtxt(i+j)[:50]

cl = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf','000000']
if len(names)<=3:
    cl = ['r','k','b']
np.random.shuffle(cl)

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
            if p < .01:
                win = 1*(med[i,k] < med[j,k])-1*(med[i,k] > med[j,k])
                wins[i]+= win 
                wins[j]-= win
                heat[i,j]+=win
                heat[j,i]-=win

# ordenarlos por comparaciones ganadas
#for i in np.argsort(-1*wins):
#    print("{} & {} \\\\ \\midrule".format(names[i],wins[i]))
#print( [(names[i],wins[i]) for i in np.argsort(-1*wins)] )
#xt=plt.plot(wins)
#plt.xticks(np.arange(len(names)),labels=names)
#plt.plot()
#plt.title("Numero de comparaciones ganadas")
#plt.show()
#from matplotlib import rc,rcParams
#rc('font', weight='bold')
# heatmap
fig, ax = plt.subplots()
im = ax.imshow(heat,cmap="plasma")
# We want to show all ticks...
ax.set_xticks(np.arange(len(names)))
ax.set_yticks(np.arange(len(names)))
# ... and label them with the respective list entries
ax.set_xticklabels(names)
ax.set_yticklabels(names)
ax.set_xlim(-.5,len(names)-.5)
ax.set_ylim(-.5,len(names)-.5)
#ax.grid(which="both", color="w", linestyle='-', linewidth=2)

# Rotate the tick labels and set their alignment.
plt.setp(ax.get_xticklabels(), rotation=45, ha="right",
         rotation_mode="anchor")

# Loop over data dimensions and create text annotations.
for i in range(len(names)):
    for j in range(len(names)):
        text = ax.text(j, i, heat[i, j],
                       ha="center", va="center", color="k")

#ax.set_title("Re")
fig.tight_layout()
plt.show()

# tipos de puntos
markers =['o','s','D','v','P','p','<','>']

                    # Graficar en dos mitades dmu01-40 y dmu41-80 #
instalabel = ["DMU"+str(i+1).zfill(2) for i in range(80)]
xx = np.arange(80)+1
best = np.loadtxt("/home/cacao/cimat/proyectotec/bestres/dmu_best.txt")
for st,end in [(0,40),(40,80)]:
    for i in range(len(names)):
        med = np.median(res[i],axis=1)/best-1
        plt.plot(xx[st:end],med[st:end].flatten(),markers[i],markersize=5,color=cl[i],alpha =.8,label=names[i])
        plt.plot(xx[np.argwhere(med==0)],med[np.argwhere(med==0)],"*",markersize=9,color=cl[i])
    plt.xlim(st+.5,end+.5)
    plt.ylim(-0.01,0.5)
    plt.xticks(xx[st:end],instalabel[st:end],rotation='vertical')
    locs,labs=plt.yticks(rotation='vertical')
    #plt.yticks(np.linspace(0,.5,11)+.01,[str(i) for i in np.linspace(0,.5,11)])
    locs = np.linspace(0,.5,6)
    plt.yticks(locs,["{:1.2f}".format(i) for i in locs])
    plt.ylabel("Mediana del error relativo")
    #plt.ylabel(r'$\frac{x_{best}}{x_{EA}}$',rotation='horizontal')
    plt.grid(True)
    plt.legend(prop={'weight':'bold'})
    plt.show()

