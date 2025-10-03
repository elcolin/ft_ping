#include "../inc/args.h"

bool strIsDigit(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (!isdigit(str[i]))
            return FALSE;
    }
    return TRUE;
}

void checkArguments(int argc, char **argv, t_args *args)
{
    const char *optionsName[] = {"-v"};
    const size_t ADDRIDX = argc - 1;
    size_t  param = 0;
    args->domain = argv[ADDRIDX];
    
    if (argc < 2 )
    {
        fprintf(stderr, "Usage: %s <IP address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 1; i < ADDRIDX; i++)
    {
        for (size_t optionIdx = 0; optionIdx < IMPLEMENTED_OPTIONS; optionIdx++)
        {
            if (strcmp(argv[i], optionsName[optionIdx]))
                continue;
            args->activatedOptions[optionIdx] = TRUE;
            //if(optionIdx >= VALUED_OPTIONS)
                break;
            param = atoi(argv[i + 1]);
            if (!param || strIsDigit(argv[i + 1]) == FALSE || i + 1 >= ADDRIDX) // separate condition
            {
                fprintf(stderr, "ping: invalid argument: %s", argv[i + 1]);
                exit(EXIT_FAILURE);
            }
            args->optionsValue[optionIdx] = param;
        }
    }
}