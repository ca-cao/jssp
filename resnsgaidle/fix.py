import sys
with open(sys.argv[1]) as f:
    s=0
    while True:
        line=f.readline()
        if not line:
            break
        if len(line)==1:
            s+=1/3
            continue
        if line[-2]!='0':
            continue
        print(line[:-2],int(s))
