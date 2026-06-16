
#ifndef ARG_H_SENTRY
#define ARG_H_SENTRY

#define OPTIONS_ARRAY_SIZE 5


struct program_settings {
    char decode_fl;
    char *encoded_string;
    char *image_path;
};

int parse_arguments(struct program_settings *settings, int argc, char **argv);

void set_default_settings(struct program_settings *settings);
#endif
