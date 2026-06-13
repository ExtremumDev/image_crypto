#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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

int write_pixel_buffer(int fd, char *buffer, int buffer_size,  int pixels_offset)
{
    int call_status = lseek(fd, pixels_offset, SEEK_SET);

    if(call_status == -1)
    {
        return -1;
    }

    call_status = write(fd, buffer, buffer_size);


    return 0;
}

// int main(int argc, char **argv)
// {
//     if(argc < 2)
//     {
//         printf("You dont provide path to image!\n");
//         return 1;
//     }
//     if(argc < 3)
//     {
//         printf("You dont prodive me encoding string!\n");
//         return 1;
//     }

//     int image_fd = open(argv[1], O_RDWR);

//     if(image_fd == -1)
//     {
//         perror("Faile to open your image!\n");
//     }

//     int check_file_status = check_bmp(image_fd);

//     if(check_file_status == -1)
//     {
//         perror("Failed to read your image file!\n");
//         return 2;
//     }
 
//     if(!check_file_status)
//     {
//         printf("Your file is not BMP image");
//         return 3;
//     }

//     struct image_data data_storage; 
//     int check_file_status = read_main_data(image_fd, &data_storage); 

//     if(check_file_status == SYSTEM_ERROR)
//     {
//         printf("Failed to read your file\n");
//         return 4;
//     }
//     if(check_file_status == INVALID_USER_FILE)
//     {
//         printf("Your file is invalid!\n");
//         return 2;
//     }

//     printf("Pixels offset in file: %d, width: %d, height: %d, pixels per bit: %d\n",
//             data_storage.pixels_array_offset, data_storage.width, data_storage.height, data_storage.bits_per_pixel);

//     int row_size = get_actual_row_size(data_storage.width, data_storage.bits_per_pixel), pixel_buffer_size = row_size * data_storage.height;
    
//     printf("Actual row size in the image - %d, all pixels size - %d\n", row_size, pixel_buffer_size);

//     char *pixel_buffer = malloc(pixel_buffer_size);

//     lseek(image_fd, data_storage.pixels_array_offset, SEEK_SET);
//     int read_status = read(image_fd, pixel_buffer, pixel_buffer_size);

//     print_all_pixels(pixel_buffer, pixel_buffer_size);
    
//     putchar('\n');
//     encode(argv[2], pixel_buffer, data_storage.width * (data_storage.bits_per_pixel / 8), row_size);

//     print_all_pixels(pixel_buffer, pixel_buffer_size);

//     char decode_buffer[20] = {0};
//     int status_of_decoding = decode(decode_buffer, pixel_buffer, data_storage.width * (data_storage.bits_per_pixel / 8), row_size, pixel_buffer_size);
//     if(status_of_decoding < 0)
//         printf("\nFailed to decode");
//     else 
//         printf("Decoding result: %s", decode_buffer);
//     close(image_fd);

// }

int main(int argc, char **argv)
{
    int decode_fl = 0;
    if(argc < 2)
    {
        printf("What do you want to do? Type 'imgencr --help' for more information'");
        return 0;
    }

    if(strlen(argv[1]) > 0)
    {
        if(strcmp(argv[1], "-d") == 0)
        {
            decode_fl = 1;
        }

    }
    char *image_path;
    if(decode_fl)
    {
        if(argc < 3)
        {
            printf("Need a path to the image for decoding\n");
            return 1;
        }
        image_path = argv[2];
    } else{
        if(argc < 3)
        {
            printf("Not enough arguments to encode message");
            return 1;
        }
        image_path = argv[2];
    }

    int image_fd = open(image_path, O_RDWR);

    if(image_fd == -1)
    {
        printf("Failed to open your image");
        return 1;
    }

    int check_bmp_st = check_bmp(image_fd);

    if(check_bmp_st == -1)
    {
        printf("Your image is not BMP");
        return 1;
    }

    struct image_data img_d;

    int call_status = read_main_data(image_fd, &img_d);

    if(call_status < 0)
    {
        printf("Failed to parse your file");
        return 1;
    }

    img_d.valuable_row_bytes = (img_d.bits_per_pixel / 8) * img_d.width;

    if(!decode_fl)
    {
        if(img_d.valuable_row_bytes * img_d.height < 8 * (strlen(argv[1]) + 1)) /* If string will not fit in the pixel array*/
        {
            printf("Your image is too small to encode your string in it\n");
            return 1;
        }
    }
    img_d.total_row_size = get_total_row_size(img_d.width, img_d.bits_per_pixel);
    int pixel_array_size = img_d.total_row_size * img_d.height;

    call_status = lseek(image_fd, img_d.pixels_array_offset, SEEK_SET);

    char *pixel_buffer = malloc(pixel_array_size);

    call_status = read(image_fd, pixel_buffer, pixel_array_size);

    if(call_status < pixel_array_size)
    {
        printf("Your file is invalid");
        return 1;
    }

    if(decode_fl)
    {
        char buffer[1024];
        int decode_status = decode(
            buffer,
            pixel_buffer,
            img_d.valuable_row_bytes,
            img_d.total_row_size,
            pixel_array_size
        );

        if(decode_status < 0)
        {
            printf("Failed to decode your message");
        } else{
            printf("Your message: %s\n",buffer);
        }
    } else{
        char *string = argv[1];        
        
        int encoding_status = encode(string, pixel_buffer, img_d.valuable_row_bytes, img_d.total_row_size);

        if(encoding_status < 0)
        {
            printf("Failed to encode your message");
            return 1;
        }

        call_status = lseek(image_fd, img_d.pixels_array_offset, SEEK_SET);
        call_status = write(image_fd, pixel_buffer, pixel_array_size);
    }
    close(image_fd);
    
    return 0;

}

