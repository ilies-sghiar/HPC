#include<stdlib.h>
#include<stdio.h>
#include <sys/time.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#define MAX_SOURCE_SIZE (0x100000)
#include <math.h>

double wtime(void);
int output_device_info(cl_device_id device_id);

double wtime(void)
{
   /* Use a generic timer */
   static int sec = -1;
   struct timeval tv;
   gettimeofday(&tv, NULL);
   if (sec < 0) sec = tv.tv_sec;
   return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

int output_device_info(cl_device_id device_id)
{
    int err;                            // error code returned from OpenCL calls
    cl_device_type device_type;         // Parameter defining the type of the compute device
    cl_uint comp_units;                 // the max number of compute units on a device
    cl_char vendor_name[1024] = {0};    // string to hold vendor name for compute device
    cl_char device_name[1024] = {0};    // string to hold name of compute device
#ifdef VERBOSE
    cl_uint          max_work_itm_dims;
    size_t           max_wrkgrp_size;
    size_t          *max_loc_size;
#endif


    err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to access device name!\n");
        return EXIT_FAILURE;
    }
    printf(" \n Device is  %s ",device_name);

    err = clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to access device type information!\n");
        return EXIT_FAILURE;
    }
    if(device_type  == CL_DEVICE_TYPE_GPU)
       printf(" GPU from ");

    else if (device_type == CL_DEVICE_TYPE_CPU)
       printf("\n CPU from ");

    else
       printf("\n non  CPU or GPU processor from ");


    err = clGetDeviceInfo(device_id, CL_DEVICE_VENDOR, sizeof(vendor_name), &vendor_name, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to access device vendor name!\n");
        return EXIT_FAILURE;
    }
    printf(" %s ",vendor_name);

    err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &comp_units, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to access device number of compute units !\n");
        return EXIT_FAILURE;
    }
    printf(" with a max of %d compute units \n",comp_units);

#ifdef VERBOSE
//
// Optionally print information about work group sizes
//
    err = clGetDeviceInfo( device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint),
                               &max_work_itm_dims, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to get device Info (CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS)!\n",
                                                                            err_code(err));
        return EXIT_FAILURE;
    }

    max_loc_size = (size_t*)malloc(max_work_itm_dims * sizeof(size_t));
    if(max_loc_size == NULL){
       printf(" malloc failed\n");
       return EXIT_FAILURE;
    }
    err = clGetDeviceInfo( device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, max_work_itm_dims* sizeof(size_t),
                               max_loc_size, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to get device Info (CL_DEVICE_MAX_WORK_ITEM_SIZES)!\n",err_code(err));
        return EXIT_FAILURE;
    }
    err = clGetDeviceInfo( device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t),
                               &max_wrkgrp_size, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to get device Info (CL_DEVICE_MAX_WORK_GROUP_SIZE)!\n",err_code(err));
        return EXIT_FAILURE;
    }
   printf("work group, work item information");
   printf("\n max loc dim ");
   for(int i=0; i< max_work_itm_dims; i++)
     printf(" %d ",(int)(*(max_loc_size+i)));
   printf("\n");
   printf(" Max work group size = %d\n",(int)max_wrkgrp_size);
#endif

    return CL_SUCCESS;

}

int main(int argc, char *argv[]) 
{
  //Analyse des param entres en ligne de commande   
  if ( argc != 3 )
  {
    printf("usage: somme-ocl.x <0=cpu/1=gpu> <OutputFile>\n");
    exit(1);
  }
  // Recuperer valeur param a
  
  int iter;
  int err;

  // Nom du fichier resultat
  FILE* res=NULL;
  res = fopen(argv[2], "w");
  if(res == NULL)
  {
    printf("Erreur ecriture fichier %s\n",argv[2]);
    exit(2);
  }


  // Boucle sur la dimension qui va de 2^10 a 2^26.
  for(iter = 4; iter < 14; iter++)
  {
    int MATRIX_DIM = 2<<(iter+1);
    int MATRIX_SIZE = MATRIX_DIM * MATRIX_DIM;

    printf("Dimension de la matrice: %d x %d (total éléments: %d)\n", MATRIX_DIM, MATRIX_DIM, MATRIX_SIZE);

    // Get platform IDs
    cl_uint nPlats;
    err = clGetPlatformIDs(0, NULL, &nPlats);
    cl_platform_id *platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * nPlats);
    err = clGetPlatformIDs(1, platforms, &nPlats);
    printf("Num platforms IDs = %d\n", nPlats);

    int i, j;
    // Create the two input matrices
    float *A = (float*)malloc(sizeof(float) * MATRIX_SIZE);
    float *B = (float*)malloc(sizeof(float) * MATRIX_SIZE);
    
    // Initialize matrices A and B
    for(i = 0; i < MATRIX_DIM; i++) 
    {
      for(j = 0; j < MATRIX_DIM; j++) 
      {
        int index = i * MATRIX_DIM + j;
        A[index] = 1.0f;
        B[index] = 2.0f;
      }
    }

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;
    fp = fopen("somme-ocl.cl", "r");
    if (!fp) 
    {
      fprintf(stderr, "Failed to load kernel.\n");
      exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );
    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    // Connect to a GPU (gpu=1) or a CPU (gpu=0)
    int gpu = atoi(argv[1]);
    err = clGetDeviceIDs(platforms[0], gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    free(platforms); platforms = NULL;
    // print context
    err = output_device_info(device_id);
    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);
    // Creation d un buffer memoire pour chaque vecteur

    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE * sizeof(float), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE * sizeof(float), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, MATRIX_SIZE * sizeof(float), NULL, &ret);

    // Copies dans les buffers
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, MATRIX_SIZE * sizeof(float), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, MATRIX_SIZE * sizeof(float), B, 0, NULL, NULL);

    // Creation du programmeeveeventeventnt
    cl_program program = clCreateProgramWithSource(context, 1,(const char **)&source_str, (const size_t *)&source_size, &ret);
    // Build
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (ret<0 )
      printf("Couldnot build program\n"),exit(1);

    // Creation du kernel opencl
    cl_kernel kernel = clCreateKernel(program, "somme_gpu", &ret);
    if (ret<0 )
      printf("Couldnot create kernel\n"),exit(1);

    // Initialisation des arguments
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
    ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&MATRIX_DIM);


    // Execute the OpenCL kernel on the list
    size_t global_item_size[] = {MATRIX_DIM, MATRIX_DIM}; // Process the entire lists
    size_t local_item_size[] = {32,32}; // Process in groups of 256

    // Event timer
    cl_event event_kernel;
    // Lancer le calcul sur le gpu/cpu
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,global_item_size, local_item_size, 0, NULL, &event_kernel);
    if (ret<0)
      printf("Erreur de soumission du calcul\n");
    printf("Enqueue Kernel1\n");
    // Read the memory buffer C on the device to the local variable C
    float *C = (float*)malloc(sizeof(float)*MATRIX_SIZE);
    //ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,LIST_SIZE * sizeof(float), C, 0, NULL, NULL);
    // Clean up command queue
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    /* ==== handle events to monitor kernel's execution === */
    clWaitForEvents(1, &event_kernel);

    cl_ulong time_start_kernel, time_stop_kernel;

    clGetEventProfilingInfo(event_kernel,CL_PROFILING_COMMAND_START,sizeof(time_start_kernel),&time_start_kernel,NULL);
    clGetEventProfilingInfo(event_kernel,CL_PROFILING_COMMAND_END,sizeof(time_stop_kernel),&time_stop_kernel,NULL);
    /* === computational time of the kernel === */
    double time_kernel=(time_stop_kernel-time_start_kernel)*1.0e-9;

    printf("\nThe kernel ran in %lf seconds\n",time_kernel);
    // Free all the opencl datas
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    // Ecriture sur fichier temps gpu vs Nb Elements
    fprintf(res, "%d\t",MATRIX_SIZE);
    fprintf(res,"%lf\n",time_kernel);
    //On libere la memoire
    free(A);
    free(B);
    free(C);
    free(source_str);
  }
  fclose(res);
  return 0;
}
