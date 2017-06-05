#include <stdio.h>

#include "imageio.h"
#include "filters.h"
#include "filters_cl.h"

int main(int argc, char* argv[]){
    if(argc<=2){
        printf("Usage: main <inputfile> <outputfile> \n");
        return 1;
    }
    char* input = argv[1];
    char* output = argv[2];

    printf("Reading file...\n");
    struct image* img = read_jpeg_file(input);
    printf("done! \n");

    printf("Applying Gaussian filter...\n");
    struct image* img_filtered = create_image(img->width, img->height);
    gaussian(img, img_filtered);
    printf("done! \n");

    printf("Writing file...\n");
    write_jpeg_file(img_filtered, output);
    printf("done! \n");

    destroy_image(img);

    return 0;
}
