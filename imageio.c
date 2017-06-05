#include <stdio.h>
#include <stdlib.h>
#include "libjpeg/jpeglib.h"

#include "imageio.h"

struct image* create_image(int width, int height){
    struct image* res = malloc(sizeof(struct image));

    res->r=(unsigned char*)malloc(sizeof(char)*width*height);
    res->g=(unsigned char*)malloc(sizeof(char)*width*height);
    res->b=(unsigned char*)malloc(sizeof(char)*width*height);

    res->height = height;
    res->width = width;

    return res;
}

void destroy_image(struct image* img){
    free(img->r);
    free(img->g);
    free(img->b);
}

struct image* read_jpeg_file(char* filename){
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE * infile;		
    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Error: Can't open %s\n", filename);
        exit(1);
    }

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);


    struct image* target = create_image(cinfo.output_width, cinfo.output_height);

    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, cinfo.num_components * cinfo.output_width, 1);
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);

        for(unsigned int i=0; i<cinfo.output_width; i++){
            target->r[(cinfo.output_scanline-1)*cinfo.output_width + i]=(buffer[0])[cinfo.num_components*i];
            target->g[(cinfo.output_scanline-1)*cinfo.output_width + i]=(buffer[0])[cinfo.num_components*i+1];
            target->b[(cinfo.output_scanline-1)*cinfo.output_width + i]=(buffer[0])[cinfo.num_components*i+2];
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);

    return target;
}


void write_jpeg_file(struct image* img, char* filename){
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE * outfile;		
    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);
    
    cinfo.image_width = img->width;
    cinfo.image_height = img->height;
    cinfo.input_components = 3;		
    cinfo.in_color_space = JCS_RGB; 	

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE); 
    jpeg_start_compress(&cinfo, TRUE);


    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, 4*img->width, 1);
    while (cinfo.next_scanline<cinfo.image_height) {
        
        for(unsigned int i=0; i<img->width; i++){
            (buffer[0])[cinfo.num_components*i]=  img->r[cinfo.next_scanline*img->width + i];
            (buffer[0])[cinfo.num_components*i+1]=img->g[cinfo.next_scanline*img->width + i];
            (buffer[0])[cinfo.num_components*i+2]=img->b[cinfo.next_scanline*img->width + i];
        }

        jpeg_write_scanlines(&cinfo, buffer, 1);
    }


    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

}
