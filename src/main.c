#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "error.h"
#include "bmp.h"
#include "encoder.h"
#include "arg.h"

#define VERSION "v alpha 0.0.0"


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

/*int image_fd;

int check_file(int fd);

int read_pixels_offset(int fd);

int read_pixels_data(int fd);*/




int main(int argc, char **argv)
{
    struct input_parameters parameters;
    parameters.options_len = parameters.parameters_len = parameters.settings_len = 0;
    int parse_status = parse_arguments(&parameters, argc, argv);

    if(parse_status < 0)
    {
        return 1;
    }

    struct program_settings settings;
    set_default_settings(&settings);
    parse_status = parse_program_settings(&parameters, &settings);

    if(parse_status < 0)
    {
        return 1;
    }

    char *image_path = settings.image_path;

    if(image_path == NULL)
    {
        fprintf(stderr, "No image provided");
        return 1;
    }

    if(!settings.decode_fl && settings.encoded_string == NULL)
    {
        fprintf(stderr, "No encoded string provided");
        return 1;
    }

    int image_fd = open(image_path, O_RDWR);

    if(image_fd == -1)
    {
        switch(errno){
            case ENOENT:
                perror("Image does not exist");
                break;
            default:
                perror("Failed to open your image\n");
                break;

        }
        return 1;
    }

    int check_bmp_st = check_bmp(image_fd);

    if(check_bmp_st == -1)
    {
        return 1;
    }

    struct image_data img_d;

    int call_status = read_main_data(image_fd, &img_d);

    if(call_status == -1)
    {
        return 1;
    }

    img_d.valuable_row_bytes = (img_d.bits_per_pixel / 8) * img_d.width;

    if(!settings.decode_fl)
    {
        if(img_d.valuable_row_bytes * img_d.height < 8 * (strlen(settings.encoded_string) + 1)) /* If string will not fit in the pixel array*/
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

    if(settings.decode_fl)
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
        int encoding_status = encode(settings.encoded_string, pixel_buffer, img_d.valuable_row_bytes, img_d.total_row_size);

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

