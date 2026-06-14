
#ifndef ARG_H_SENTRY
#define ARG_H_SENTRY

#define OPTIONS_ARRAY_SIZE 5


struct option_setting {
    char key;
    char *value;
};


struct input_parameters {
    char options[OPTIONS_ARRAY_SIZE];
    int options_len;

    struct option_setting settings[OPTIONS_ARRAY_SIZE];
    int settings_len;

    char *simple_parameters[OPTIONS_ARRAY_SIZE];
    int parameters_len;
};

struct program_settings {
    char decode_fl;
    char *encoded_string;
    char *image_path;
};

int parse_arguments(struct input_parameters *parameters, int argc, char **argv);

int parse_program_settings(struct input_parameters *param, struct program_settings *settings);

void set_default_settings(struct program_settings *settings);
#endif
