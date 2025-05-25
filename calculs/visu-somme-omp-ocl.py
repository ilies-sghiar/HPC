import numpy as np
import matplotlib.pyplot as plt
import os


os.system("echo calcul somme sur cpu : scalaire")
os.system("echo '...compilation'")
os.system('gcc -O3 somme-cpu.c -o somme-cpu.x -lm')
os.system("echo '...execution'")
os.system('./somme-cpu.x')

os.system("echo calcul somme sur cpu : openmp")
os.system("echo '...compilation'")
os.system('gcc -O3 -march=native -fopenmp somme-omp.c -o somme-omp.x -lm')
os.system("echo '...execution'")
os.system('./somme-omp.x')

os.system("echo calcul somme sur gpu")
os.system("echo '...compilation'")
os.system('nvcc -O3 somme_ocl.c -o somme-ocl.x -lOpenCL')
#os.system('gcc -O3 -framework OpenCL saxpy-ocl.c -o saxpy-ocl.x -lm ')
os.system("echo '...execution'")
os.system('./somme-ocl.x 1 SommeOCL.txt')

os.system("echo '...analyse des resultats'")
data_gpu=np.loadtxt('./SommeOCL.txt')
data_cpu=np.loadtxt('./SommeScalar.txt')
print("shape",data_cpu.shape)
data_omp=np.loadtxt('./SommeOmp.txt')
print("shape",data_omp.shape)
dimdata=data_omp.shape
nblines  =dimdata[0]
nbthreads=dimdata[1]

mycol=['red','blue','green','orange','cyan','purple','red','blue','black']
plt.figure(1)
plt.clf()
plt.grid(True)
plt.xscale('log')
plt.yscale('log')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('$\log(N)$')
plt.ylabel('$\log(t [s])$')
plt.plot(data_cpu[:,0],data_cpu[:,1],'ro-',label='cpu')
print(data_gpu)
plt.plot(data_gpu[:,0],data_gpu[:,1],'ko-',label='gpu')
for n in range(1,nbthreads):
   lab='nbthreads='+str(n)
   plt.plot(data_omp[:,0],data_omp[:,n],color=mycol[n-1],marker='*',linestyle='-',label=lab)

plt.title("Time required to compute matrix sum with OpenMP and OpenCL")
plt.legend(loc='best')
plt.savefig('somme.png',dpi=400)
plt.close(1)
os.system("echo")
os.system("echo '----script ended'")

#*mastergpugrp03!