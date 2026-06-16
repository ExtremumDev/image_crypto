#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arg.h"

#define ALLOWED_OPTIONS "vdh"
#define ALLOWED_SETTINGS "s"

#define MAX_PARAM_NUMBER 1


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

static int parse_option_setting(char option_key, char *value, struct program_settings *settings)
{
    switch(option_key){
        case 's':
            settings->encoded_string = value;
            break;
        default:
            return -1;
    }
    return 0;
}

static int parse_option(char option, struct program_settings *settings)
{
    switch(option){
        case 'd':
            settings->decode_fl = 1;
            break;
        case 'h':
            settings->purpose = print_help_p;
            break;
        case 'v':
            settings->purpose = print_version_p;
            break;
        default: 
            return -1;
    }
    return 0;
}

int parse_arguments(struct program_settings *settings, int argc, char **argv)
{
    char *simple_parameters[MAX_PARAM_NUMBER];
    int param_idx = 0;

    int func_status, arg_idx, await_setting_value_fl = 0;
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
            func_status = parse_option_setting(setting_key, arg, settings);
            
            if(func_status == -1)
            {
                fprintf(stderr, "Invalid option -%c", setting_key);
                return -1;
            }
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
                func_status = parse_option(arg[1], settings);

                if(func_status == -1)
                {
                    fprintf(stderr, "Unknown option -%c", arg[1]);
                    return -1;
                }
            } else {
                fprintf(stderr, "Unknown option -%c\n", arg[1]);
                return -1;
            }
        } else {
            if(param_idx >= MAX_PARAM_NUMBER)
            {
                perror("Too many arguments, try enc2img -h for help information\n");
                return -1;
            }
            simple_parameters[param_idx] = arg;
            param_idx++;
        }

    }
    if(await_setting_value_fl) /* If last option awaits providing a value and arguments have finished */
    {
        fprintf(stderr, "Value of option -%c wasnt prodided\n", setting_key);
        return -1;
    }

    if(param_idx > 0)
    {
        settings->image_path = simple_parameters[0];
    }
    return 0;
}


void set_default_settings(struct program_settings *settings)
{
    settings->decode_fl = 0;
    settings->encoded_string = NULL;
    settings->image_path = NULL;
    settings->purpose = base_p;
}
