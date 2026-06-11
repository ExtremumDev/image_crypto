

#ifndef ENCODE_H_SENTRY
#define ENCODE_H_SENTRY

int encode(const char *encoding_str, char *dst, int row_bytes_c, int entire_row_bytes_c);

int decode(char *dst_str, const char *src_buffer, int row_bytes_c, int entire_row_bytes_c, int buffer_size);

#endif
