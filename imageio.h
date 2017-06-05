#ifndef IMAGEIO_H
#define IMAGEIO_H


struct image {
    unsigned char* r;
    unsigned char* g;
    unsigned char* b;
    unsigned int width;
    unsigned int height;
};

struct image* create_image(int width, int height);
void destroy_image(struct image* img);

struct image* read_jpeg_file(char* filename);
void write_jpeg_file(struct image* img, char* filename);

#endif //IMAGEIO_H
