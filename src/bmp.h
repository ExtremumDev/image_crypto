

#ifndef BMP_H_SENTRY
#define BMP_H_SENTRY

#include "error.h"

struct image_data {
    int width, height, bits_per_pixel, pixels_array_offset;
    int total_row_size, valuable_row_bytes;
};


int check_bmp(int fd);

file_error_t read_main_data(int fd, struct image_data *img_d);

int get_total_row_size(int img_width, int bits_per_pixel);

#endif
