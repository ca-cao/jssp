import numpy as np
import matplotlib.pyplot as plt
import sys

name=[]
res=[]
inst = []

for fname in sys.argv[1:]:
    res+=[np.loadtxt(fname).astype(int)]
    #name+=[fname:-4].replace("_"," ")]
    name += [fname[:fname.find(".")]]
    inst += [fname[fname.find("u")+1:fname.find(".")]]

inst = np.array([int(i)-1 for i in inst])
print(inst)
best = np.loadtxt("../dmu_best.txt")
#title = "dmu" + sys.argv[1][sys.argv[1].find("_")+1:sys.argv[1].find("_")+6]
title = "dmu_"+str(inst[0]+1)+"_"+str(inst[0]+1+inst.size)
#print(title)
#title=name[i][name[i].find(" ")+1:len(name[i])-name[i][::-1].find(" ")-1]
plt.boxplot(res,meanline=True,showmeans=True,labels=name,vert=False)
plt.plot(best[inst],np.arange(inst.size)+1,"g*")
plt.grid(True)
#print("\\begin{table}[]")
#print("\\begin{tabular}{lllll}")
#print("Mejor reportado & Mediana & Menor costo & Mayor costo & Desviación estándar \\\\")
#for b,n in zip(best,res):
#    print("{:d} & {:d} & {:d} & {:d} & {:4.2f}".format(int(b),int(np.median(n)),int(n.min()),int(n.max()),n.std())) 
#print("\end{tabular}")
#print("\end{table}")
#title = name[0]
#plt.title(title)
#plt.plot(np.std(res,axis=1).flatten(),[i for i in range(len(res))],"bo")
plt.savefig(title+".png")
#plt.show()



