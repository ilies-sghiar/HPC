__kernel void somme_gpu(__global const float* A, 
                         __global const float* B, 
                         __global float* C,
                         const int DIM) 
{
    // Récupérer les indices globaux
    int i = get_global_id(0);
    int j = get_global_id(1);
    
    // Vérifier que nous sommes dans les limites de la matrice
    if (i < DIM && j < DIM) {
        // Calcul de l'index linéarisé
        int index = i * DIM + j;
        
        // Effectuer l'addition
        C[index] = A[index] + B[index];
    }
}