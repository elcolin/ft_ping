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
// Buffer_size

#define NO_VALUE_OPTIONS 1
#define VALUED_OPTIONS 2
#define IMPLEMENTED_OPTIONS (NO_VALUE_OPTIONS + VALUED_OPTIONS)
#define OPTIONS {"-t", "-s", "-v"}
#define DEFAULT_PADDING 56
#define DEFAULT_TTL 64

enum OPTION {
    TTL,
    PACKET_SIZE,
    // non valued options
    VERBOSE,
};


typedef struct s_args
{
    bool                activatedOptions[IMPLEMENTED_OPTIONS];
    size_t                 optionsValue[VALUED_OPTIONS];
    char                *domain;
} t_args;

void checkArguments(int argc, char **argv, t_args *args);
#include "packet.h"
void initDefaultOptions(t_args *args);

#endif