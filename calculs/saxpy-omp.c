#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define SUCCESS 0

float *x;
float *y;
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
  int    NbMaxThreads=1;
  int    nbthreads;

  printf("argc= %d\n",argc);
  if ( argc == 1 )
    NbMaxThreads=4;
  else    
    NbMaxThreads=atoi(argv[1]);

  printf("NbMaxThreads= %d\n",NbMaxThreads);
  
  FileDump = fopen ("SaxpyOmp.txt", "w");
  /* Loop over Number of vector elements */
  for(n=12;n<=24; n++) 
  {
    size=2<<(n-1);
    printf("\n");
    printf("n    = %ld\n",n);
    printf("2^n  = %ld\n",size);
    printf("size = %ld [Kb]\n",size*sizeof(float)/1024);
    
    /* initialize arrays */ 
    x= (float*)calloc(size,sizeof(float));
    y= (float*)calloc(size,sizeof(float));
    float a=3.0;
    for (i=0 ; i < size ; i++)
    {
      x[i]=1.0f;
      y[i]=1.0f;
    }
    
    fprintf(FileDump,"%17ld ",size);
    for(nbthreads= 1; nbthreads <= NbMaxThreads; nbthreads++)
    {
      /* allocate number of threads */
      omp_set_dynamic(0); 
      omp_set_num_threads(nbthreads);
      /* start timer */ 
      tic=omp_get_wtime();
      /* parallelise loops over elements */
      #pragma omp parallel for private(i) shared(x,y,a,size)
      for (i=0; i < size; i++)
         y[i]+=a*x[i];
      /* end   timer */
      toc=omp_get_wtime();
      /* cpu time */ 
      ElapsedTime= (double)(toc - tic);
      printf("nbthreads=%d cputime=%le\n",nbthreads,ElapsedTime);
      fprintf(FileDump,"\t %le",ElapsedTime);
    }
    fprintf(FileDump,"\n");
    /* free dynamically allocated arrays */
    free(x);
    free(y);
  }
  fclose(FileDump);

  return SUCCESS;
}

double wtime(void)
{
   /* Use a generic timer */
   static int sec = -1;
   struct timeval tv;
   gettimeofday(&tv, NULL);
   if (sec < 0) sec = tv.tv_sec;
   return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

