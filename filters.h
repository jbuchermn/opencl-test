#ifndef FILTERS_H
#define FILTERS_H

struct image;

#define GAUSSIAN_GRID_SPACING 0.1
#define GAUSSIAN_WIDTH 25

void gaussian(struct image* input, struct image* output);


#endif //FILTERS_H
