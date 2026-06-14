#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arg.h"

#define ALLOWED_OPTIONS "dh"
#define ALLOWED_SETTINGS "s"


static int check_el_in_array(char el, const char *arr)
{
    while(*arr)
    {
        if(*arr == el)
            return 1;
        arr++;
    }
    return 0;
}


static int is_option(char option)
{
    return check_el_in_array(option, ALLOWED_OPTIONS);
}

static int is_setting(char option)
{
    return check_el_in_array(option, ALLOWED_SETTINGS);
}


int parse_arguments(struct input_parameters *parameters, int argc, char **argv)
{
    int arg_idx, await_setting_value_fl = 0;
    char setting_key;

    for(arg_idx = 1; arg_idx < argc; arg_idx++)
    {
        char *arg = argv[arg_idx];
        int str_len = strlen(arg);

        if(str_len == 0)
        {
            fprintf(stderr, "Error: empty %d argument\n", arg_idx + 1);
            return -1;
        }
        if(await_setting_value_fl)
        {
            parameters->settings[parameters->settings_len].key = setting_key;
            parameters->settings[parameters->settings_len].value = arg;
            parameters->settings_len++;
            await_setting_value_fl = 0;
            continue;
        }

        if(arg[0] == '-')
        {
            if(str_len < 2)
            {
                fprintf(stderr, "Invalid flag %s\n", arg);
                return -1;
            }

            if(is_setting(arg[1]))
            {
                await_setting_value_fl = 1;
                setting_key = arg[1];
                continue;
            }

            if(is_option(arg[1]))
            {
                parameters->options[parameters->options_len] = arg[1];
                parameters->options_len++;
            } else {
                fprintf(stderr, "Unknown option -%c\n", arg[1]);
                return -1;
            }
        } else {
            parameters->simple_parameters[parameters->parameters_len] = arg;
            parameters->parameters_len++;
        }

    }
    if(await_setting_value_fl) /* If last option awaits providing a value and arguments have finished */
    {
        fprintf(stderr, "Value of flag wasnt prodided\n");
        return -1;
    }
    return 0;
}


int parse_program_settings(struct input_parameters *param, struct program_settings *settings)
{
    char option; int i = 0;
    char *value;
    for(option = param->options[i]; i < param->options_len; option=param->options[i++])
    {
        switch(option){
            case 'd':
                settings->decode_fl = 1;
                break;
            default: 
                fprintf(stderr, "Unknown option -%c", option);
                return -1;
        }
    }
    
    for(i = 0; i < param->settings_len; i++)
    {
        value = param->settings[i].value;
        option = param->settings[i].key;

        switch(option){
            case 's':
                settings->encoded_string = value;
                break;
            default:
                fprintf(stderr, "Unknow option -%c", option);
                return -1;
        }
    }

    if(param->parameters_len > 0)
    {
        settings->image_path = param->simple_parameters[0];
    }
    return 0;
}

void set_default_settings(struct program_settings *settings)
{
    settings->decode_fl = 0;
    settings->encoded_string = NULL;
    settings->image_path = NULL;
}
