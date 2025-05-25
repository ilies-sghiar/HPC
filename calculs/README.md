Explication des différents dossiers/fichiers importants
-------------------------------------------------------

Codes CPU :

- saxpy-cpu.c : code séquentiel pour effectuer l'opération SAXPY
- somme-cpu.c : code séquentiel pour effectuer l'opération Somme Matricielle
- produit-cpu.c : code séquentiel pour effectuer l'opération Produit Matriciel

Code OpenMP : 

- saxpy-omp.c : code OpenMP pour effectuer l'opération SAXPY
- somme-omp.c : code OpenMP pour effectuer l'opération Somme Matricielle
- produit-omp.c code OpenMP pour effectuer l'opération Produit Matriciel

Code OpenCL (GPU) :

- saxpy-ocl.c : code C pour effectuer l'opération SAXPY sur GPU, kernel associé : saxpy-ocl.cl
- somme-ocl.c code C pour effectuer l'opération Somme Matricielle sur GPU, kernel associé somme-ocl.cl
- produit-ocl.c code C pour effectuer l'opération Produit Matriciel sur GPU, kernel associé produit-ocl.cl

Visualisation des Performances: 

- visu-saxpy-omp-ocl.py : visualiser résultats pour saxpy
- visu-somme-omp-ocl.py : visualiser résultats pour la somme matricielle
- visu-produit-omp-ocl.py : visualiser résultats pour le produit matriciel


