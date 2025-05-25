/**
 * \file imageFilter.cpp
 * \mainpage Image filtering with openCL.
 *
 * This tutorial introduces openCL programming with images filtering
 *
 * Under linux, type the following commands:
 *
 * cmake -g .
 *
 * make
 * 
 * and then execute the command ./imageFilter -h
 *
 *
 * arguments are handled with cxxopts: more info on https://github.com/jarro2783/cxxopts
 */
#include <iomanip>
#include "utils.hpp"
#include <math.h>
#include "cxxopts.hpp"

// Define uchar4 structure matching OpenCL's uchar4
typedef struct {
    unsigned char x, y, z, w;  // R, G, B, A
} uchar4;

/// \brief Main function
///
/// Takes no arguments
int main(int argc, char* argv[])
{

  /// parameters managements. use cxxopts.hpp
  /// you can modify this and add other parameters easily
  string cl_filename;
  string kernel_name;
  string image_name;
  string output_name;
  int filter_size;  
  cxxopts::Options  desc(argv[0], " - Options");
  desc.add_options()
    ("h,help", "Help screen")
    ("i,image", "image file name", cxxopts::value<std::string>()->default_value("manet.jpg"))
    ("o,output", "output image file name", cxxopts::value<std::string>()->default_value("result.png"))
    ("ocl", "ocl file name", cxxopts::value<std::string>()->default_value("copyimageGa.cl"))
    ("k,kernel", "kernel name in ocl file", cxxopts::value<std::string>()->default_value("copy_image"))
    ("s,size", "Half window size (neighborhood radius)", cxxopts::value<int>()->default_value("3"));
  
  auto vm = desc.parse(argc, argv);
  
  if (vm.count("help"))
    {
      std::cout << desc.help() << std::endl;
      return 0;
    }
  
  cl_filename = vm["ocl"].as<string>();
  kernel_name = vm["kernel"].as<string>();
  filter_size = vm["size"].as<int>();
  image_name  = vm["image"].as<string>();
  output_name  = vm["output"].as<string>();
  
  cout << "working on filter size: " << filter_size << endl;
  ///---------------------------------------------------------------------------------------------------
  
  int rtnValue = SUCCESS;
  /*Step1: Getting platforms and choose an available one.*/
  cl_uint numPlatforms; //the NO. of platforms
  cl_platform_id platform = 0;  //the chosen platform
  cl_int  status = clGetPlatformIDs(1, &platform, &numPlatforms);
  if (status != CL_SUCCESS)
    {
      cout << "Error: Getting platforms!" << endl;
      return FAILURE;
    }

  /*For clarity, choose the first available platform. */
  if(numPlatforms > 0)
    {
      cl_platform_id* platforms = (cl_platform_id* )malloc(numPlatforms* sizeof(cl_platform_id));
      status = clGetPlatformIDs(numPlatforms, platforms, NULL);
      platform = platforms[0];
      free(platforms);
    }

  cout << "platform chosen" << endl;
  
  /*Step 2:Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device.*/
  cl_uint       numDevices = 0;
  cl_device_id        *devices;
  status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);  
  cout << "Found " << numDevices << " GPU device(s)" << endl;
  if (numDevices == 0)  //no GPU available.
    {
      cout << "No GPU device available." << endl;
      cout << "Choose CPU as default device." << endl;
      status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);  
      devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
      status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
    }
  else
    {
      devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
      status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
    }

    // Make sure the device supports images, otherwise exit
    cl_bool imageSupport = CL_FALSE;
    clGetDeviceInfo(devices[0], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool),
                    &imageSupport, NULL);
    if (imageSupport != CL_TRUE)
    {
        cerr << "[WARNING] OpenCL device does not support images." << std::endl;
    cerr << "          You should use 1D arrays" << endl;
    cerr << "          You can ignore this message coming from MINES St Etienne instructors" << endl;
    }

/* print information on openCL version */
    size_t valueSize;
    clGetDeviceInfo(devices[0], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
    char * value = (char*) malloc(valueSize);
    clGetDeviceInfo(devices[0], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
    printf(" OpenCL C version: %s\n", value);
    free(value);

  /*Step 3: Create context.*/
  cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);
  //displayImageFormats(context);
 
  /*Step 4: Creating command queue associate with the context.*/
  cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, NULL);

  /*Step 5: Create program object */
  const char *filename = "copyimageGa.cl";
  string sourceStr;
  status = convertToString(cl_filename, sourceStr);
  //cout << "-------------\nread: " << sourceStr << endl;

  // add some options
  string options("-D HALF_FILTER_SIZE=");
  options += std::to_string(filter_size);
  
  const char *source = sourceStr.c_str();
  size_t sourceSize[] = {strlen(source)};
  cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
  
  /*Step 6: Build program. */
  status=clBuildProgram(program, 0, NULL, (const char *)options.c_str(), NULL, NULL);
  if (status != CL_SUCCESS) {
    cout << "Erreur dans la compilation: " << getErrorString(status) << endl;
    cl_build_status build_status=0;
    status = clGetProgramBuildInfo(program, 0, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status),
           &build_status, NULL);

    cout << "build status: " << build_status << endl;
    size_t ret_val_size=0;
    clGetProgramBuildInfo(program,  devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
    cout << "size of log: " << ret_val_size << endl;
    char *build_log = new char[ret_val_size+1];
    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
    build_log[ret_val_size] = '\0';

    cout << "Build Log: " << endl 
   << build_log << endl;

    delete[] build_log;
    cout << "exiting..." << endl;
    return 1;
  }
  
  /*Step 7: Initial input,output for the host and create memory objects for the kernel*/
  size_t image_width, image_height;
  int channels = 4;
  unsigned char* rgba_data= readImageFile(image_name.c_str(), image_width, image_height,  channels);
  if (!rgba_data)
    {
      std::cout << desc.help() << std::endl;
      return -1;
    }

  // Convert RGBA data to uchar4 array
  uchar4* input_data = new uchar4[image_width * image_height];
  for (size_t i = 0; i < image_width * image_height; i++) {
      input_data[i].x = rgba_data[i*4];    // R
      input_data[i].y = rgba_data[i*4+1];  // G
      input_data[i].z = rgba_data[i*4+2];  // B
      input_data[i].w = rgba_data[i*4+3];  // A
  }
    
  uchar4* output_data = new uchar4[image_width * image_height];
  memset(output_data, 0, image_width * image_height * sizeof(uchar4));

  cout << "read image, width:" << image_width << ", height:" << image_height
       << ", channels:" << channels << endl;


  // Create exponential table
  float sigma = 10.0f;
  float sigmasq = sigma * sigma;
  
  float* expTab = new float[filter_size];
  
  // Fill exponential table
  ;
  for (int i=0;i<filter_size;i++) {
      
          int idx = -(i*i)/(2*sigmasq);
          expTab[i] = exp(idx);
      
  }

  cl_mem cl_input = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   image_width * image_height * sizeof(uchar4),
                                   input_data, NULL);
  cl_mem cl_output = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                  image_width * image_height * sizeof(uchar4),
                                  NULL, NULL);
  cl_mem cl_exp = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 filter_size * sizeof(float),
                                 expTab, &status);
  if (status != CL_SUCCESS) {
      cerr << "Error creating exponential table buffer: " << getErrorString(status) << endl;
      return FAILURE;
  }

  /* Create kernel and set arguments */
  cl_kernel kernel = clCreateKernel(program, "copy_image", NULL);
  
  clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_input);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), &cl_output);
  clSetKernelArg(kernel, 2, sizeof(int), &image_width);
  clSetKernelArg(kernel, 3, sizeof(int), &image_height);
  clSetKernelArg(kernel, 4, sizeof(int), &filter_size);
  clSetKernelArg(kernel, 5, sizeof(cl_mem), &cl_exp);
  clSetKernelArg(kernel, 6, sizeof(float), &sigma);

  /* Execute kernel */
  size_t global_work_size[2] = {image_width, image_height};
  cl_event event;
  status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, 
                                global_work_size, NULL, 
                                0, NULL, &event);
  clWaitForEvents(1, &event);

  /* Get execution time */
  cl_ulong time_start, time_end;
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
  cout << "Execution time: " << setprecision(4) << (time_end - time_start)/1E9 << " seconds" << endl;

  /* Read back results */
  status = clEnqueueReadBuffer(commandQueue, cl_output, CL_TRUE, 0,
                             image_width * image_height * sizeof(uchar4),
                             output_data, 0, NULL, NULL);
  
  if (status != CL_SUCCESS) {
      cerr << "Error reading result buffer: " << getErrorString(status) << endl;
      return FAILURE;
  }


  //Convert uchar4 output back to RGBA for saving 
  unsigned char* result_rgba = new unsigned char[image_width * image_height * 4];
  for (size_t i = 0; i < image_width * image_height; i++) {
      result_rgba[i*4]   = output_data[i].x;
      result_rgba[i*4+1] = output_data[i].y;
      result_rgba[i*4+2] = output_data[i].z;
      result_rgba[i*4+3] = output_data[i].w;
  }

  saveImageFile("result.png", result_rgba, image_width, image_height);

  /* Cleanup */
  delete[] rgba_data;
  delete[] input_data;
  delete[] output_data;
  delete[] result_rgba;
  clReleaseMemObject(cl_input);
  clReleaseMemObject(cl_output);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(commandQueue);
  clReleaseContext(context);
  free(devices);

  return rtnValue;
  
}
