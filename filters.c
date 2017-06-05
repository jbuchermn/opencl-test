#include <math.h>

#include "filters.h"

#include "imageio.h"

void gaussian(struct image* input, struct image* output){

    float gauss[GAUSSIAN_WIDTH*GAUSSIAN_WIDTH];
    float norm=0.;
    for(unsigned int i=0; i<GAUSSIAN_WIDTH; i++){
        for(unsigned int j=0; j<GAUSSIAN_WIDTH; j++){
            gauss[i*GAUSSIAN_WIDTH+j]=exp(-(pow(i-GAUSSIAN_WIDTH/2,2)+pow(j-GAUSSIAN_WIDTH/2,2))*pow(GAUSSIAN_GRID_SPACING,2));
            norm+=gauss[i*GAUSSIAN_WIDTH+j];
        }
    }

    for(unsigned int i=0; i<GAUSSIAN_WIDTH; i++){
        for(unsigned int j=0; j<GAUSSIAN_WIDTH; j++){
            gauss[i*GAUSSIAN_WIDTH+j]/=norm;
        }
    }

    

    for(unsigned int i=0; i<input->height; i++){
        for(unsigned int j=0; j<input->width; j++){

            unsigned char r=0;
            unsigned char g=0;
            unsigned char b=0;
            for(int i_=0; i_<GAUSSIAN_WIDTH; i_++){
                for(int j_=0; j_<GAUSSIAN_WIDTH; j_++){
                    int iIdx = i+i_-GAUSSIAN_WIDTH/2;
                    int jIdx = j+j_-GAUSSIAN_WIDTH/2;

                    if(iIdx<0) iIdx+=input->width;
                    if(jIdx<0) jIdx+=input->height;
                    if(iIdx>=input->height) iIdx-=input->height;
                    if(jIdx>=input->width) jIdx-=input->width;

                    r+=input->r[iIdx*input->width+jIdx]*gauss[i_*GAUSSIAN_WIDTH+j_];
                    g+=input->g[iIdx*input->width+jIdx]*gauss[i_*GAUSSIAN_WIDTH+j_];
                    b+=input->b[iIdx*input->width+jIdx]*gauss[i_*GAUSSIAN_WIDTH+j_];
                }
            }

            output->r[i*input->width+j]=r;
            output->g[i*input->width+j]=g;
            output->b[i*input->width+j]=b;

        }
    }
}
