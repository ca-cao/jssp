import zipfile
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict
import matplotlib.cbook as cbook
import ast

#def defaultval():
#    return 0
#
#def boxst(values, freqs):
#    values = np.array(values)
#    freqs = np.array(freqs)
#    arg_sorted = np.argsort(values)
#    values = values[arg_sorted]
#    freqs = freqs[arg_sorted]
#    count = freqs.sum()
#    fx = values * freqs
#    mean = fx.sum() / count
#    variance = ((freqs * values**2).sum() / count) - mean**2
#    std = np.sqrt(variance)
#    minimum = np.min(values)
#    maximum = np.max(values)
#    cumcount = np.cumsum(freqs)
#    Q1 = values[np.searchsorted(cumcount, 0.25*count)]
#    median = values[np.searchsorted(cumcount, 0.50*count)]
#    Q3 = values[np.searchsorted(cumcount, 0.75*count)]
#    ric = (Q3-Q1)
#    whislo =max(0, Q1 -1.5*ric)
#    whishi = Q3 +1.5*ric
#    result = {'med':median,'q1':Q1,'q3':Q3,'whislo':whislo,'whishi':whishi}
#    return result
#
#def makebxp(zfile,file_name):
#    count = 0
#    with z.open(file_name) as f:
#        dic = defaultdict(defaultval)
#        for line in f:
#            dic[abs(float(line[:-1]))]+=1
#            count+=1
#            if count >1000:
#                break
#        x,y = map(list,zip(*dic.items()))
#        return(boxst(x,y))
#
#with zipfile.ZipFile('clustering.zip') as z:
#    filen7 = ['clustering/dmu'+str(i).zfill(2)+'.txtn7clustering' for i in range(1,81)]
#    bxn7=[makebxp(z,file) for file in filen7]
#    filepr = ['clustering/dmu'+str(i).zfill(2)+'.txtprclustering' for i in range(1,81)]
#    bxpr=[makebxp(z,file) for file in filepr]
#    for i in range(1,41):
#        count = 0
#        nmbr = str(i).zfill(2)
#        bxps1.append(makebxp(z,file_name))
#       # with z.open(file_name) as f:
#       #     dic = defaultdict(defaultval)
#       #     for line in f:
#       #         dic[abs(float(line[:-1]))]+=1
#       #         count+=1
#       #         if count >50000:
#       #             break
#       #     x,y = map(list,zip(*dic.items()))
#       #     bxps.append(boxst(x,y))
#

# cargar los diccionarios
bxn7=[]
bxpr=[]
with open('n7dic','r') as f:
    for line in f:
        bxn7.append(ast.literal_eval(line))

with open('prdic','r') as f:
    for line in f:
        bxpr.append(ast.literal_eval(line))
    

labels = ["DMU"+str(i).zfill(2) for i in range(1,81)]
for start,end in [(0,40),(40,80)]:
    fig,ax =plt.subplots()
    ax.bxp(bxn7[start:end],showfliers=False)
    ax.set_xticklabels(labels[start:end], rotation=90, ha='center')
    ax.set_ylabel("Error relativo")
    #plt.savefig("n7"+"{}-{}".format(start+1,end)+".png")
    #plt.title("n7")
    print("n7")
    plt.show()
    
    fig,ax =plt.subplots()
    ax.bxp(bxpr[start:end],showfliers=False)
    ax.set_xticklabels(labels[start:end], rotation=90, ha='center')
    ax.set_ylabel("Error relativo")
    #plt.savefig("pr"+"{}-{}".format(start+1,end)+".png")
    #plt.title("Representación")
    print("pr")
    plt.show()
    
