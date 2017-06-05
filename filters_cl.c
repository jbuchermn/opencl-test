#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <OpenCL/opencl.h>

#include "filters_cl.h"

#include "imageio.h"
#include "kernel.cl.h"

int next_16(int i){
    int j=(i/16)*16;
    if(j<i) j+=16;
    return j;
}

void gaussian_cl(struct image* input, struct image* output){
    // Get queue
    dispatch_queue_t queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_GPU, NULL);
    if (queue == NULL) {
        queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_CPU, NULL);
    }
    
    // Print out device name
    cl_device_id gpu = gcl_get_device_id_with_dispatch_queue(queue);
    char name[128];
    clGetDeviceInfo(gpu, CL_DEVICE_NAME, 128, name, NULL);
    printf("Created a dispatch queue using the %s\n", name);
 

    unsigned char* inp[]= {input->r,  input->g,  input->b };
    unsigned char* outp[]={output->r, output->g, output->b};

    
    float* gauss = (float*)malloc(sizeof(float) * GAUSSIAN_WIDTH*GAUSSIAN_WIDTH);
    float norm=0.;
    for(unsigned int i=0; i<GAUSSIAN_WIDTH; i++){
        for(unsigned int j=0; j<GAUSSIAN_WIDTH; j++){
            gauss[i*GAUSSIAN_WIDTH+j]=(cl_float)exp(-(pow(i-GAUSSIAN_WIDTH/2,2)+pow(j-GAUSSIAN_WIDTH/2,2))*pow(GAUSSIAN_GRID_SPACING,2));
            norm+=gauss[i*GAUSSIAN_WIDTH+j];
        }
    }

    for(unsigned int i=0; i<GAUSSIAN_WIDTH; i++){
        for(unsigned int j=0; j<GAUSSIAN_WIDTH; j++){
            gauss[i*GAUSSIAN_WIDTH+j]/=norm;
        }
    }

    
    void* mem_gauss = gcl_malloc(sizeof(cl_float) * GAUSSIAN_WIDTH*GAUSSIAN_WIDTH, gauss, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    for(unsigned int i=0; i<3; i++){

        // Allocate GCL memory
        void* mem_in  = gcl_malloc(sizeof(cl_uchar) * input->width*input->height, inp[i], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
        void* mem_out = gcl_malloc(sizeof(cl_uchar) * input->width*input->height, NULL,   CL_MEM_WRITE_ONLY);

        unsigned char* _outp = outp[i];

        // Dispatch the Kernel
        dispatch_sync(queue, ^{
            size_t wgs;
            gcl_get_kernel_block_workgroup_info(matrix_kernel, CL_KERNEL_WORK_GROUP_SIZE, sizeof(wgs), &wgs, NULL);
           
            // Setup configuration
            cl_ndrange range = {                                            
                2,                                   // Dimension
                {0,            0,               0 }, // Offset in each dimension  
                {next_16(input->width), next_16(input->height),   0 }, // Size in each dimension 
                {sqrt(wgs),    sqrt(wgs),       0 }  // Workgroup size in each dimension 
            };

            // Execute kernel
            matrix_kernel(&range, (cl_uchar*)mem_in, (cl_uchar*)mem_out, (cl_float*)mem_gauss, input->width, input->height, GAUSSIAN_WIDTH, GAUSSIAN_WIDTH);

            // Copy results back
            gcl_memcpy(_outp, mem_out, sizeof(cl_uchar) * input->width*input->height);
        });

        // Free GCL memory
        gcl_free(mem_in);
        gcl_free(mem_out);
    
    }

    free(gauss);

    dispatch_release(queue);
}
