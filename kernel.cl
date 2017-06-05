kernel void matrix  (global unsigned char* input,               
                     global unsigned char* output,
                     global float* matrix,
                     unsigned int width,
                     unsigned int height,
                     unsigned int matrixWidth,
                     unsigned int matrixHeight)
{
    size_t i = get_global_id(0);
    size_t j = get_global_id(1);

    if(i>=width || j>=height) return;

    float res=0.f;
    for(unsigned int i_=0; i_<matrixWidth; i_++){
        for(unsigned int j_=0; j_<matrixHeight; j_++){
            int i__=(i+i_+width-matrixWidth/2)%width;
            int j__=(j+j_+height-matrixHeight/2)%height;

            res+=matrix[matrixWidth*j_+i_]*input[width*j__+i__];
        }
    }

    output[width*j+i] = res;
}

