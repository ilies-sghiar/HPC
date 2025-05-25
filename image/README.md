Explication des différents dossiers/fichiers importants
-------------------------------------------------------

- copyimagenaif.cl : Kernel pour l’implémentation du filtre moyenneur naïf (sans UCHAR4)
- copyimage.cl : Kernel pour l’implémentation du filtre moyenneur optimisé (avec UCHAR4)
- copyimageGa.cl : Kernel pour l’implémentation du filtre gaussien naïf
- copyimageGb.cl : Kernel pour le filtre gaussien optimisé V1
- copyimageGc.cl : Kernel pour le filtre gaussien optimisé V2
- copyimageGd.cl : Kernel pour le filtre gaussien optimisé V3
- copyimageGe.cl : Kernel pour le filtre gaussien optimisé V4

- imageCopyFilterwithoutUCHAR4.cpp : Code C++ associé au filtre moyenneur naïf
- imageCopyFilterABC.cpp : Code C++ initialement utilisé avec les kernels : filtre moyenneur optimisé, filtre gaussien naïf, optimisés V1 et V2
- imageCopyFilter.cpp : Code C++ utilisé avec tous les kernels du filtre gaussien

- createCSV.py : Script Python qui exécute les filtres et génère des fichiers CSV des temps d'exécution (à adapter selon les kernels testés)
- execution_results/ : Dossier contenant les fichiers CSV générés
- trace.py : Script Python pour tracer les courbes de performance à partir des CSV


Comment exécuter le code sur le cluster
---------------------------------------

1. Exécuter le script de génération des données :
   python createCSV.py

2. Une fois les fichiers CSV générés, les rapatrier en local (le module 'pandas' n’est pas disponible sur le cluster)

3. En local, exécuter le script de visualisation :
   python trace.py