
#ifndef ARG_H_SENTRY
#define ARG_H_SENTRY

#define OPTIONS_ARRAY_SIZE 5

enum program_purpose {
    base_p, print_help_p, print_version_p /* Determine final goal of current execution: direct goal of
                                          program or print help/version if relevant flags in cli arguments */
};

struct program_settings {
    char decode_fl;
    char *encoded_string;
    char *image_path;

    enum program_purpose purpose;
};

int parse_arguments(struct program_settings *settings, int argc, char **argv);

void set_default_settings(struct program_settings *settings);
#endif
