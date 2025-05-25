#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define SUCCESS 0

float *A;
float *B;
float *C;
unsigned int size;
unsigned int i, j, k;
int iter;
FILE *FileDump;

double wtime(void);

int main(void)
{
  double tic;
  double toc; 
  double ElapsedTime;

  FileDump = fopen("ProduitScalar.txt", "w");
  if (!FileDump) {
    printf("Erreur ouverture fichier\n");
    return 1;
  }

  for(iter = 4; iter < 10; iter++) 
  {
    int MATRIX_DIM = 2 << (iter + 1);  // Dimension de la matrice
    size = MATRIX_DIM * MATRIX_DIM;

    printf("\nIteration = %d\n", iter);
    printf("Dimension matrice: %d x %d\n", MATRIX_DIM, MATRIX_DIM);
    printf("Nombre d'éléments: %u\n", size);
    printf("Taille mémoire (Kb): %lu\n", size * sizeof(float) / 1024);

    A = (float*)calloc(size, sizeof(float));
    B = (float*)calloc(size, sizeof(float));
    C = (float*)calloc(size, sizeof(float));
    if (!A || !B || !C) {
      printf("Allocation mémoire échouée\n");
      return 1;
    }

    // Initialisation
    for(i = 0; i < size; i++) {
      A[i] = 1.0f;
      B[i] = 2.0f;
      C[i] = 0.0f;
    }

    tic = wtime();
    // Produit matriciel
    for(i = 0; i < MATRIX_DIM; i++) {
      for(j = 0; j < MATRIX_DIM; j++) {
        float sum = 0.0f;
        for(k = 0; k < MATRIX_DIM; k++) {
          sum += A[i * MATRIX_DIM + k] * B[k * MATRIX_DIM + j];
        }
        C[i * MATRIX_DIM + j] = sum;
      }
    }
    toc = wtime();

    ElapsedTime = toc - tic;
    printf("Temps produit matrices: %le secondes\n", ElapsedTime);
    fprintf(FileDump, "%u %e\n", size, ElapsedTime);

    free(A);
    free(B);
    free(C);
  }

  fclose(FileDump);

  return SUCCESS;
}

double wtime(void)
{
   static int sec = -1;
   struct timeval tv;
   gettimeofday(&tv, NULL);
   if (sec < 0) sec = tv.tv_sec;
   return (tv.tv_sec - sec) + 1.0e-6 * tv.tv_usec;
}
