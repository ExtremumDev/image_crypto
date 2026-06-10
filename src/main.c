#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "error.h"
#include "bmp.h"


int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("You dont provide path to image!\n");
        return 1;
    }

    int image_fd = open(argv[1], O_RDWR);

    if(image_fd == -1)
    {
        perror("Faile to open your image!\n");
    }

    int check_file_status = check_bmp(image_fd);

    if(check_file_status == -1)
    {
        perror("Failed to read your image file!\n");
        return 2;
    } else
 
    if(!check_file_status)
    {
        printf("Your file is not BMP image");
        return 3;
    }

    struct image_data data_storage; 
    check_file_status = read_main_data(image_fd, &data_storage); 

    if(check_file_status == SYSTEM_ERROR)
    {
        printf("Failed to read your file\n");
        return 4;
    }
    if(check_file_status == INVALID_USER_FILE)
    {
        printf("Your file is invalid!\n");
        return 2;
    }

    printf("Pixels offset in file: %d, width: %d, height: %d, pixels per bit: %d",
            data_storage.pixels_array_offset, data_storage.width, data_storage.height, data_storage.bits_per_pixel);

    close(image_fd);

}
