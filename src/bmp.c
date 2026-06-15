#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "bmp.h"
#include "encoder.h"

#define BMP_BM_SIGNATURE 0x4D42

/* Fixed offsets of different parametres im BMP format */

#define PIXELS_ARRAY_O 10 
#define PIXELS_INFO_O 18

/* end of offsets constants */

/* Fixed size of data portions */

#define PIXEL_ARRAY_O_SIZE 4

int check_bmp(int fd)
{
    /* Check bmp signature in input file
     * If failed to read the file or file is not BMP -> notice user about error and return -1,  */

    short file_format_header;
    int call_status = lseek(fd, 0, SEEK_SET);

    if(call_status == -1)
    {
        perror("Failed to read your file\n");
        return -1;
    }

    call_status = read(fd, &file_format_header, 2);

    if(call_status == 2)
    {
        if(file_format_header == BMP_BM_SIGNATURE) /* Checks only BM(Windows version) */
        {
            return 0;
        } else {
            perror("Your file is not BMP");
            return -1;
        }
    }else {
        perror("Your file is damaged\n");
        return -1;
    }
    return -1;  
}


static int read_pixel_data(int fd, struct image_data *img_d)
{
    char buffer[12];
    int sys_call_status = lseek(fd, PIXELS_INFO_O, SEEK_SET);

    if(sys_call_status == -1)
    {
        perror("Failed to read your file\n");
        return -1;
    }

    if(sys_call_status != PIXELS_INFO_O)  
    {
        perror("Your file is damaged\n");
        return -1;
    }
    sys_call_status = read(fd, &buffer, 12);

    if(sys_call_status == -1)
    {
        perror("Failed to read your file\n");
        return -1;
    }

    if(sys_call_status != 12)
    {
        perror("Your file is damaged\n");
        return -1;
    }

    img_d->width = *((int*) &buffer[0]); /* Get first 4 bytes(starting from 0) of char array, where readed image info keeped */
    img_d->height = *((int*) &buffer[4]);
    img_d->bits_per_pixel = *((short*) &buffer[10]); /* Skips 8-9 bytes */

    return 0; 
}


static int read_pixel_arr_offset(int fd, struct image_data *img_d)
{

    int sys_call_status;
    sys_call_status = lseek(fd, PIXELS_ARRAY_O, SEEK_SET);

    if(sys_call_status == -1)
    {
        perror("Failed to read your file\n");
        return -1;
    }

    if(sys_call_status != PIXELS_ARRAY_O) /* Failed to access needed position */
    {
        perror("Your file is damaged\n");
        return -1;
    }

    sys_call_status = read(fd, &(img_d->pixels_array_offset), PIXEL_ARRAY_O_SIZE);

    if(sys_call_status == -1)
    {
        perror("Failed to read your file\n");
        return -1;
    }
    if(sys_call_status != 4)
    {
        perror("Your file is damaged\n");
        return -1;
    }

}

int read_main_data(int fd, struct image_data *img_d)
{
    int func_status;

    func_status = read_pixel_arr_offset(fd, img_d);

    if(func_status == -1)
        return -1;

    func_status = read_pixel_data(fd, img_d);

    if(func_status == -1)
    {
        return -1;
    }
    return 0;
}

int get_total_row_size(int img_width, int bits_per_pixel)
{
    return ((bits_per_pixel * img_width + 31) / 32 ) * 4;
}
