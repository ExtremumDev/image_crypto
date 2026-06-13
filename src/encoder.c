#include <stdio.h>

#include "encoder.h"

int encode(const char *encoding_str, char *dst, int row_bytes_c, int entire_row_bytes_c)
{
    char current_char;
    int row_bytes_index = 0, bit_index = 0;
    int last = 0;

    while(!last)
    {
        current_char = *encoding_str;
        if(current_char == 0)
        {
            last = 1;
        }
        bit_index = 0;
        
        while(bit_index != 8)
        {
            *dst &= 0b11111110; /* Last bit of image byte doesnt matter now */
            *dst |= (current_char & 0b00000001); /* Turn off other bits of char, and change byte of miage due to this mask */

            row_bytes_index++;

            dst++;

            if(row_bytes_index == row_bytes_c)
            {
                dst += (entire_row_bytes_c - row_bytes_c); /* Skip alignment bytes */
                row_bytes_index = 0;
            }
            current_char >>= 1;
            bit_index++;
        }
        encoding_str++;

    }
    return 0;
}


int decode(char *dst_str, const char *src_buffer, int row_bytes_c, int entire_row_bytes_c, int buffer_size)
{
    char current_char = 0;
    int current_bit_index = 0, current_byte_index = 0, current_row_byte_index = 0;

    while(current_byte_index < buffer_size)
    {
        current_char += (*src_buffer & 0b00000001) << current_bit_index;
        current_bit_index++;
        if(current_bit_index == 8)
        {
            current_bit_index = 0;
            *dst_str = current_char;
            dst_str++;

            if(current_char == 0)
            {
                return 0;
            }
            current_char = 0;
        }
        current_row_byte_index++;
        current_byte_index++;

        src_buffer++;
        
        if(current_row_byte_index == row_bytes_c)
        {
            int skipped = entire_row_bytes_c - row_bytes_c;
            current_row_byte_index = 0;
            src_buffer += skipped;
            current_byte_index += skipped;
        }
    }

    return -1;
    
} 

