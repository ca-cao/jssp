import numpy as np
import matplotlib.pyplot as plt
import sys

name=[]
res=[]
best = np.loadtxt("dmu_best.txt")
bestils = np.loadtxt("best_ils_fit")
for fname in sys.argv[1:]:
    res+=[np.loadtxt(fname)]
    #name+=[fname:-4].replace("_"," ")]
    name += [fname[:fname.find("_")]+fname[fname.find("_")+6:]]


#title = "dmu" + sys.argv[1][sys.argv[1].find("_")+1:sys.argv[1].find("_")+6]
#print(title)
#title=name[i][name[i].find(" ")+1:len(name[i])-name[i][::-1].find(" ")-1]
points = dict(markersize=2,markerfacecolor = 'k')
for i in [0,40]:
    plt.boxplot(res[int(i):int(i)+40],flierprops=points,whis=(0,100))
    plt.plot(np.arange(1,41,1),best[int(i):int(i)+40],"o",label="Estado del arte")
    plt.plot(np.arange(1,41,1),bestils[int(i):int(i)+40],"o",label="Mejores obtenidos")
    #plt.grid(True)
    plt.plot()
    plt.xticks(np.arange(1,41,1),labels=np.arange(int(i)+1,int(i)+41,1))
    plt.legend()
    ##plt.savefig("dmu"+str(i+1)+"_"+str(i+40)+".png")
    plt.show()
#title = name[0]
#plt.title(title)
#plt.plot(np.std(res,axis=1).flatten(),[i for i in range(len(res))],"bo")



