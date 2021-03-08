import numpy as np
import matplotlib.pyplot as plt
import sys

name=[]
res=[]
inst = []

for fname in sys.argv[1:]:
    res+=[np.loadtxt(fname)[:,0]]
    #name+=[fname:-4].replace("_"," ")]
    name += [fname[:fname.find("n")]]
    inst += [fname[fname.find("u")+1:fname.find("n")]]

inst = np.array([int(i)-1 for i in inst])
#print(inst)
best = np.loadtxt("../dmu_best.txt")
#title = "dmu" + sys.argv[1][sys.argv[1].find("_")+1:sys.argv[1].find("_")+6]
#print(title)
#title=name[i][name[i].find(" ")+1:len(name[i])-name[i][::-1].find(" ")-1]
#plt.boxplot(res,meanline=True,showmeans=True,labels=name,vert=False)
#plt.grid(True)
#plt.plot(best[inst],np.arange(inst.size)+1,"g*")
#title = name[0]
#plt.title(title)
#plt.plot(np.std(res,axis=1).flatten(),[i for i in range(len(res))],"bo")
#plt.savefig(title+".png")
#plt.show()

print("\\begin{table}[]")
print("\\begin{tabular}{@{}llllll@{}}")
print("\\toprule")
print("Instancia & Mejor reportado & Mediana & Menor costo & Mayor costo & Desviación estándar \\\\ \\midrule")
for b,n,r in zip(best,res,name):
    print(r+" & {:d} & {:d} & {:d} & {:d} & {:4.2f}\\\\".format(int(b),int(np.median(n)),int(n.min()),int(n.max()),n.std())) 
print("\\\\ \\bottomrule")
print("\end{tabular}")
print("\end{table}")


