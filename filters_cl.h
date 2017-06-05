#ifndef FILTERS_CL_H
#define FILTERS_CL_H

struct image;

#define GAUSSIAN_GRID_SPACING 0.1
#define GAUSSIAN_WIDTH 25

void gaussian_cl(struct image* input, struct image* output);

#endif // FILTERS_CL_H
