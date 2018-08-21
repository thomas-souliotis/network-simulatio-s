import matplotlib.pyplot as plt
import sys

# task1 diagrams
f1   = open("task1.txt","r")
k    = 20*[0]
vals = 80*[0]
for i in range(0,20):
    k[i] = 5*(i+1)

i = 0
for line in f1:
    line = line.strip()
    if len(line) > 30: # to avoid header lines
        left,val = line.split("is equal to:")
        vals[i]  = float(val)
        i        = i+1

aarfNo = vals[0:20]
aarfRF = vals[20:40]
caraNo = vals[40:60]
caraRF = vals[60:80]

plt.plot(k,aarfNo,'bo',k,aarfNo,'b',k,aarfRF,'ro',k,aarfRF,'r',k,caraNo,'go',k,caraNo,'g',k,caraRF,'yo',k,caraRF,'y')
plt.ylabel('Throughput in Mbps')
plt.xlabel('Distance in m')
plt.title("AARF no fading: blue, AARF rayleigh fading: red,\n CARA no fading: green, CARA rayleigh fading: yellow") 
plt.show()

# task2 diagrams
f2   = open("task2.txt","r")
k    = 11*[0]
vals = 22*[0]
k[0] = 1
for i in range(1,11):
    k[i] = 5*i

i = 0
for line in f2:
    line = line.strip()
    if len(line) > 30: # to avoid header lines
        left,val = line.split("is equal to:")
        vals[i]  = float(val)
        i        = i+1

aarfWIFI = vals[0:11]
caraWIFI = vals[11:22]

plt.plot(k,aarfWIFI,'bo',k,aarfWIFI,'b' ,k, caraWIFI,'yo',k,caraWIFI,'y')
plt.ylabel('Throughput in Mbps')
plt.xlabel('Number of station nodes')
plt.title("AARF: blue, CARA: yellow") 
plt.show()

# task3 diagrams
f3   = open("task3.txt","r")
k    = 11*[0]
vals = 22*[0]
k[0] = 1
for i in range(1,11):
    k[i] = 5*i

i = 0
for line in f3:
    line = line.strip()
    if len(line) > 30: # to avoid header lines
        left,val = line.split("is equal to:")
        vals[i]  = float(val)
        i        = i+1

aarfWIFI = vals[0:11]
caraWIFI = vals[11:22]

plt.plot(k,aarfWIFI,'bo',k,aarfWIFI,'b' ,k, caraWIFI,'yo',k,caraWIFI,'y')
plt.ylabel('Throughput in Mbps')
plt.xlabel('Number of station nodes')
plt.title("AARF: blue, CARA: yellow") 
plt.show()
