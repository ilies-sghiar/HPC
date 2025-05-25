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
    ("ocl", "ocl file name", cxxopts::value<std::string>()->default_value("copyimage.cl"))
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
  const char *filename = "copyimage.cl";
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
  int channels;
  unsigned char* data= readImageFile(image_name.c_str(), image_width, image_height,  channels);
  if (!data)
    {
      std::cout << desc.help() << std::endl;
      return -1;
    }
  unsigned char *buffer = new unsigned char[image_width*image_height*channels];
  cout << "read image, width:" << image_width << ", height:" << image_height
       << ", channels:" << channels << endl;
  // this initialization makes sure that device memory is initialized
  for (int i=0; i<image_width*image_height*channels; ++i)
    buffer[i]=i%255;

  cl_image_format image_format;
  image_format.image_channel_order = CL_RGBA;
  image_format.image_channel_data_type = CL_UNORM_INT8;

  // use 1D arrays
  cl_mem cl_image = clCreateBuffer(context, CL_MEM_READ_ONLY| CL_MEM_COPY_HOST_PTR,
           image_width*image_height*channels * sizeof(unsigned char),
           data, NULL);
  cl_mem cl_output_image = clCreateBuffer(context, CL_MEM_WRITE_ONLY| CL_MEM_COPY_HOST_PTR,
           image_width*image_height*channels * sizeof(unsigned char),
           buffer, NULL);
 
  /*Step 8: Create kernel object */
  int errNum=0;
  cl_kernel kernel = clCreateKernel(program, kernel_name.c_str(), &errNum);
  if (kernel == NULL)
    {
      cerr << "Failed to create kernel (" << errNum << ")" << endl;
      cerr << getErrorString(errNum) << endl;
      return 1;
    }
  else
    cout << "Kernel created" << endl;

  /*Step 9: Sets Kernel arguments.*/
  // input and output images
  
  // Create sampler for sampling image object
  cl_sampler sampler = clCreateSampler(context,
               CL_FALSE, // Non-normalized coordinates
               CL_ADDRESS_CLAMP_TO_EDGE,
               CL_FILTER_NEAREST,
               0);

  // images arguments
  int argIndex=0;
  status = clSetKernelArg(kernel, argIndex++, sizeof(cl_mem), (void *)&cl_image);
  status = clSetKernelArg(kernel, argIndex++, sizeof(cl_mem), (void *)&cl_output_image);
 
  // width and height of image
  status = clSetKernelArg(kernel, argIndex++, sizeof(cl_int), &image_width);
  status = clSetKernelArg(kernel, argIndex++, sizeof(cl_int), &image_height);
  status = clSetKernelArg(kernel, argIndex++, sizeof(int), &filter_size);
  
  cout << "arguments assigned" << endl;

  /*Step 10: Running the kernel.*/
  size_t local_work_size[2] = {16, 16};
  size_t global_work_size[2] = {RoundUp(local_work_size[0], image_width), 
        RoundUp(local_work_size[1], image_height)};

  cl_event event=0;
  cl_event *pevent=NULL;
  cl_uint num_event =0;
  status = clEnqueueNDRangeKernel(commandQueue, kernel, /* dimension of data*/ 2, /* offset */ NULL, 
          global_work_size /*global_work_size*/, local_work_size/* local_work_size */,
          /* num_events */ num_event, /* wait_list */ pevent, /* event */ &event);
  // ensure execution is finished
  clWaitForEvents(1 , &event);
  cl_ulong time_start, time_end;
  double total_time;
  
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
  total_time = time_end - time_start;
  cout << "Execution time: " << setprecision(4) << total_time/1E9 << " seconds" << endl;

  /*Step 11: Read the output back to host memory.*/
  unsigned char *buffer2 = new unsigned char[image_width * image_height * 4](); 
  // this initialization makes sure buffer2 is written
  for (int i=0; i< image_width*image_height*channels; ++i)
    buffer2[i] = 0;

  size_t origin[3] = { 0, 0, 0 };
  size_t region[3] = { image_width, image_height, 1};

  status = clEnqueueReadBuffer(commandQueue, cl_output_image, CL_TRUE, 0,
             image_width*image_height*channels* sizeof(unsigned char),
             buffer2, 0, NULL, NULL);
    
  if (status != CL_SUCCESS)
    {
      std::cerr << "Error reading result buffer." << std::endl;
      cerr << getErrorString(status) << endl;
      return FAILURE;
    }

  // save resulting image into a file
  saveImageFile(output_name.c_str(), buffer2, image_width, image_height);

  
  /*Step 12: Clean the resources.*/
  delete data; 
  delete buffer2;
  status = clReleaseKernel(kernel);       //Release kernel.
  status = clReleaseProgram(program);       //Release the program object.
  status = clReleaseMemObject(cl_image);                    //Release mem object.
  status = clReleaseMemObject(cl_output_image);   //Release mem object.
  status = clReleaseContext(context);       //Release context.

  if (devices != NULL)
    {
      free(devices);
      devices = NULL;
    }

  if (rtnValue == SUCCESS)
    std::cout<<"Passed!\n";
  else
    std::cout << " Error in computation!\n";

  return rtnValue;
}
