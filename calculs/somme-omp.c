#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define SUCCESS 0

float *A;
float *B;
float *C;
unsigned int size;
unsigned int i;
int n;
FILE *FileDump;

double wtime(void);

int main(int argc, char **argv)
{
  double tic;
  double toc;
  double ElapsedTime;
  int NbMaxThreads=1;
  int nbthreads;

  printf("argc= %d\n",argc);
  if ( argc == 1 )
    NbMaxThreads=4;
  else
    NbMaxThreads=atoi(argv[1]);

  printf("NbMaxThreads= %d\n",NbMaxThreads);

  FileDump = fopen ("SommeOmp.txt", "w");
  
  for(n=4; n<=11; n++) // taille 2^(n+1)
  {
    unsigned int MATRIX_DIM = 2 << (n+1);
    size = MATRIX_DIM * MATRIX_DIM;
    
    printf("\n");
    printf("n         = %d\n", n);
    printf("MATRIX_DIM= %u\n", MATRIX_DIM);
    printf("size      = %u [Kb]\n", size*sizeof(float)/1024);
    
    A = (float*)calloc(size, sizeof(float));
    B = (float*)calloc(size, sizeof(float));
    C = (float*)calloc(size, sizeof(float));
    if (!A || !B || !C)
    {
      printf("Erreur allocation mémoire\n");
      return 1;
    }

    for(i=0; i<size; i++)
    {
      A[i] = 1.0f;
      B[i] = 2.0f;
      C[i] = 0.0f;
    }

    fprintf(FileDump,"%17u ", size);

    for(nbthreads=1; nbthreads <= NbMaxThreads; nbthreads++)
    {
      omp_set_dynamic(0);
      omp_set_num_threads(nbthreads);

      tic = omp_get_wtime();

      #pragma omp parallel for private(i) shared(A,B,C,size)
      for(i=0; i<size; i++)
      {
        C[i] = A[i] + B[i];
      }

      toc = omp_get_wtime();
      ElapsedTime = toc - tic;

      printf("nbthreads=%d cputime=%le\n", nbthreads, ElapsedTime);
      fprintf(FileDump,"\t%le", ElapsedTime);
    }

    fprintf(FileDump, "\n");
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
  return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}
