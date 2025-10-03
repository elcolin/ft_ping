#ifndef ARGS_H
#define ARGS_H

// printf
#include <stdio.h>
// sockaddr_in
#include <netinet/ip.h>
// bool
#include "utils.h"
// strcmp
#include <string.h>
// isdigit
#include <ctype.h> 

#define NO_VALUE_OPTIONS 1
#define VALUED_OPTIONS 0
#define IMPLEMENTED_OPTIONS NO_VALUE_OPTIONS + VALUED_OPTIONS

enum OPTION {
    // non valued options
    VERBOSE,
};


typedef struct s_args
{
    bool                activatedOptions[IMPLEMENTED_OPTIONS];
    int                 optionsValue[VALUED_OPTIONS];

    struct sockaddr_in  destAddress;
    char                *domain;
} t_args;

void checkArguments(int argc, char **argv, t_args *args);

#endif