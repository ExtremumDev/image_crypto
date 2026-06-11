#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "error.h"
#include "bmp.h"
#include "encoder.h"

int print_all_pixels(char *pixels_buffer, int buffer_size)
{
    for(int i = 0; i < buffer_size; i++)
    {
        printf("%3hhu ", pixels_buffer[i]);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("You dont provide path to image!\n");
        return 1;
    }
    if(argc < 3)
    {
        printf("You dont prodive me encoding string!\n");
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

    printf("Pixels offset in file: %d, width: %d, height: %d, pixels per bit: %d\n",
            data_storage.pixels_array_offset, data_storage.width, data_storage.height, data_storage.bits_per_pixel);

    int row_size = get_actual_row_size(data_storage.width, data_storage.bits_per_pixel), pixel_buffer_size = row_size * data_storage.height;
    
    printf("Actual row size in the image - %d, all pixels size - %d\n", row_size, pixel_buffer_size);

    char *pixel_buffer = malloc(pixel_buffer_size);

    lseek(image_fd, data_storage.pixels_array_offset, SEEK_SET);
    int read_status = read(image_fd, pixel_buffer, pixel_buffer_size);

    print_all_pixels(pixel_buffer, pixel_buffer_size);
    
    putchar('\n');
    encode(argv[2], pixel_buffer, data_storage.width * (data_storage.bits_per_pixel / 8), row_size);

    print_all_pixels(pixel_buffer, pixel_buffer_size);

    char decode_buffer[20] = {0};
    int status_of_decoding = decode(decode_buffer, pixel_buffer, data_storage.width * (data_storage.bits_per_pixel / 8), row_size, pixel_buffer_size);
    if(status_of_decoding < 0)
        printf("\nFailed to decode");
    else 
        printf("Decoding result: %s", decode_buffer);
    close(image_fd);

}
