#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "bmp.h"
#include "encoder.h"

#define BMP_BM_SIGNATURE 0x4D42

/* Fixed offsets of different parametres im BMP format */

#define PIXELS_ARRAY_O 10 
#define PIXELS_INFO_O 18

/* end of offsets constants */

int check_bmp(int fd)
{
    /* Check bmp signature in input file
     * Return: -1 if error during reading of file ocured, 0 if file is not BMP format, 1 if file is correct */

    short file_format_header;
    int call_status = lseek(fd, 0, SEEK_SET);

    if(call_status == -1)
    {
        return -1;
    }

    call_status = read(fd, &file_format_header, 2);

    if(call_status == 2)
    {
        return file_format_header == BMP_BM_SIGNATURE; /* Checks only BM(Windows version) */
    }
    return -1;  
}


static file_error_t read_pixel_data(int fd, struct image_data *img_d)
{
    char buffer[12];
    int sys_call_status = lseek(fd, PIXELS_INFO_O, SEEK_SET);

    if(sys_call_status == -1)
    {
        return SYSTEM_ERROR;
    }

    if(sys_call_status != PIXELS_INFO_O)  
    {
        return INVALID_USER_FILE;
    }
    sys_call_status = read(fd, &buffer, 12);

    if(sys_call_status == -1)
    {
        return SYSTEM_ERROR;
    }

    if(sys_call_status != 12)
    {
        return INVALID_USER_FILE;
    }

    img_d->width = *((int*) &buffer[0]); /* Get first 4 bytes(starting from 0) of char array, where readed image info keeped */
    img_d->height = *((int*) &buffer[4]);
    img_d->bits_per_pixel = *((short*) &buffer[10]); /* Skips 8-9 bytes */

    return SUCCESS; 
}


file_error_t read_main_data(int fd, struct image_data *img_d)
{
    
    int sys_call_status;
    file_error_t func_status;
    sys_call_status = lseek(fd, PIXELS_ARRAY_O, SEEK_SET);

    if(sys_call_status == -1)
    {
        return SYSTEM_ERROR;
    }

    if(sys_call_status != PIXELS_ARRAY_O) /* Failed to access needed position */
    {
        return INVALID_USER_FILE;
    }

    sys_call_status = read(fd, &(img_d->pixels_array_offset), 4);

    if(sys_call_status == -1)
    {
        return SYSTEM_ERROR;
    }
    if(sys_call_status != 4)
    {
        return INVALID_USER_FILE;
    }

    func_status = read_pixel_data(fd, img_d);

    if(func_status != SUCCESS)
    {
        return func_status;
    }
    return SUCCESS;
}

int get_total_row_size(int img_width, int bits_per_pixel)
{
    return ((bits_per_pixel * img_width + 31) / 32 ) * 4;
}


