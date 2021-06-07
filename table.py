import numpy as np
from scipy.stats import kruskal
from scipy.stats import wilcoxon
import matplotlib.pyplot as plt
import os 
import sys 

# recibe el nombre del archivo con resultados
best = np.loadtxt("/home/cacao/cimat/proyectotec/bestres/dmu_best.txt")
res = np.zeros((len(sys.argv)-1,80,50))

# carga los archivos
names = []
for l,i in enumerate(sys.argv[1:]):
    if i[-1]!="/":
        i=i+"/"
    names.append(i[:-1])
    for k,j in enumerate(sorted(os.listdir(i))):
        res[l,k,:] = np.loadtxt(i+j)[:50]
med = np.median(res,axis=2)

# construye los nombres
dmu =[]
for i in range(80):
    if i+1<10:
        dmu.append("DMU0"+str(i+1))
    else:
        dmu.append("DMU"+str(i+1))

# construye los tamaños
dmusize=[]
nj=20
nm=15
for i in range(40):
    nm = 15+ 5*((i//5)%2==1)
    nj = 20+10*(i//10)
    dmusize.append(str(nj)+"$\\times$"+str(nm))




# construir la tabla
print("\\begin{table}[H]")    
print("\\centering")
print("\\begin{tabular}{@{}ccccc@{}}")
print("\\toprule")
print("\\multirow{2}{*}{Instancia} & \\multirow{2}{*}{Tamaño} & \\multicolumn{2}{c}{ILS con N7} & \\multirow{2}{*}{Estado del arte} \\\\ \\cmidrule(lr){3-4}")
print("& & Mejor& Mediana & \\\\ \\midrule")
for i in range(40):
    row = ""
    row = row+ dmu[i]
    row = row+ " & "+dmusize[i]
    for j,name in enumerate(names):
        # si es igual al estado del arte poner en negritas
        if res[j,i,:].min() == best[i]:
            row = row+ " & \\textbf{"+ str(int(res[j,i,:].min()))+"}"
        else:
            row = row+ " & "+ str(int(res[j,i,:].min()))
        row = row+ " & "+ str(int(np.median(res[j,i,:])))
    row = row + " & "+str(int(best[i]))
    if i+1<40:
        row = row + "\\\\ "
    else:
        row = row + "\\\\ \\bottomrule"
    #print("{} & {} & {} & {} & {} \\\\ \\bottomrule".format(dmu[i],dmusize[i]))
    print(row)
print("\\end{tabular}")
print("\\end{table}") 


# construir la tabla II
print("")
print("\\begin{table}[H]")    
print("\\centering")
print("\\begin{tabular}{@{}ccccc@{}}")
print("\\toprule")
print("\\multirow{2}{*}{Instancia} & \\multirow{2}{*}{Tamaño} & \\multicolumn{2}{c}{ILS con N7} & \\multirow{2}{*}{Estado del arte} \\\\ \\cmidrule(lr){3-4}")
print("& & Mejor& Mediana & \\\\ \\midrule")
for i in range(40,80):
    row = ""
    row = row+ dmu[i]
    row = row+ " & "+dmusize[i-40]
    for j,name in enumerate(names):
        # si es igual al estado del arte poner en negritas
        if res[j,i,:].min() == best[i]:
            row = row+ " & \\textbf{"+ str(int(res[j,i,:].min()))+"}"
        else:
            row = row+ " & "+ str(int(res[j,i,:].min()))
        row = row+ " & "+ str(int(np.median(res[j,i,:])))
    row = row + " & "+str(int(best[i]))
    if i+1<80:
        row = row + "\\\\ "
    else:
        row = row + "\\\\ \\bottomrule"
    #print("{} & {} & {} & {} & {} \\\\ \\bottomrule".format(dmu[i],dmusize[i]))
    print(row)
print("\\end{tabular}")
print("\\end{table}") 
