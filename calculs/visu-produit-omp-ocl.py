import numpy as np
import matplotlib.pyplot as plt
import os


os.system("echo calcul produit sur cpu : scalaire")
os.system("echo '...compilation'")
os.system('gcc -O3 produit-cpu.c -o produit-cpu.x -lm')
os.system("echo '...execution'")
os.system('./produit-cpu.x')

os.system("echo calcul produit sur cpu : openmp")
os.system("echo '...compilation'")
os.system('gcc -O3 -march=native -fopenmp produit-omp.c -o produit-omp.x -lm')
os.system("echo '...execution'")
os.system('./produit-omp.x')

os.system("echo calcul produit sur gpu")
os.system("echo '...compilation'")
os.system('nvcc -O3 produit-ocl.c -o produit-ocl.x -lOpenCL')
#os.system('gcc -O3 -framework OpenCL saxpy-ocl.c -o saxpy-ocl.x -lm ')
os.system("echo '...execution'")
os.system('./produit-ocl.x 1 ProduitOCL.txt')

os.system("echo '...analyse des resultats'")
data_gpu=np.loadtxt('./ProduitOCL.txt')
data_cpu=np.loadtxt('./ProduitScalar.txt')
print("shape",data_cpu.shape)
data_omp=np.loadtxt('./ProduitOmp.txt')
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

plt.title("Time required to compute matrix product with OpenMP and OpenCL")
plt.legend(loc='best')
plt.savefig('produit.png',dpi=400)
plt.close(1)
os.system("echo")
os.system("echo '----script ended'")

#*mastergpugrp03!