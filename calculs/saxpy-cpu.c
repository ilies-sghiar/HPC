#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


#define SUCCESS 0

float *x;
float *y;
unsigned int size;
unsigned int i;
int n;
FILE *FileDump;

double wtime(void);

int main(void)
{

  double tic;
  double toc; 
  double ElapsedTime;
  

  FileDump = fopen ("SaxpyScalar.txt", "w");
  /* Loop over Number of vector elements */
  for(n=12;n<=24; n++) 
  {
    size=2<<(n-1);
    printf("\n");
    printf("n    = %ld\n",n);
    printf("2^n  = %ld\n",size);
    printf("size = %ld [Kb]\n",size*sizeof(float)/1024);
     
    x= (float*)calloc(size,sizeof(float));
    y= (float*)calloc(size,sizeof(float));
    float a=3.0;
    for (i=0 ; i < size ; i++)
    {
      x[i]=1.0f;
      y[i]=1.0f;
    }
  
    /* start timer */ 
    tic = wtime();
    for (i=0 ; i < size ; i++)
      y[i]+=a*x[i];
    /* end   timer */
    toc=wtime();
    /* cpu time */ 
    ElapsedTime= (double)(toc - tic);
    printf("%le\n",ElapsedTime);
    fprintf(FileDump,"%ld %e\n",size,ElapsedTime);
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

