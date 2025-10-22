#include "args.h"

bool strIsDigit(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (!isdigit(str[i]))
            return FALSE;
    }
    return TRUE;
}

void initDefaultOptions(t_args *args)
{
    for (size_t optionIdx = 0; optionIdx < VALUED_OPTIONS; optionIdx++)
    {
        switch (optionIdx)
        {
            case PACKET_SIZE:
                if (args->activatedOptions[PACKET_SIZE] == FALSE)
                    args->optionsValue[PACKET_SIZE] = DEFAULT_PADDING;
                triggerErrorNoFreeingIf(args->optionsValue[PACKET_SIZE] > (BUFFER_SIZE - IPHDR_SIZE - ICMPHDR_SIZE), "Packets above 65535 aren't allowed through Internet Protocol.", "Wrong Packet Value");
            break;
            case TTL:
                if (args->activatedOptions[TTL] == FALSE)
                    args->optionsValue[TTL] = DEFAULT_TTL;
            break;
            default:
                break;
        }
    }
}

void checkArguments(int argc, char **argv, t_args *args)
{
    const char *optionsName[] = OPTIONS;
    const size_t ADDRIDX = argc - 1;
    size_t  param = 0;
    args->domain = argv[ADDRIDX];
    
    triggerErrorNoFreeingIf(argc < 2, "./ft_ping <IP address>\n", "");
    for (size_t i = 1; i < ADDRIDX; i++)
    {
        for (size_t optionIdx = 0; optionIdx < IMPLEMENTED_OPTIONS; optionIdx++)
        {
            bool cmp =  !strcmp(argv[i], optionsName[optionIdx]);
            if (cmp == FALSE)
            {
                triggerErrorNoFreeingIf(optionIdx + 1 >= IMPLEMENTED_OPTIONS, "ping: invalid argument", argv[i]);
                continue;
            }
            args->activatedOptions[optionIdx] = TRUE;
            if(optionIdx >= VALUED_OPTIONS)
                break;
            param = atoi(argv[++i]);
            triggerErrorNoFreeingIf(!param || strIsDigit(argv[i]) == FALSE || i >= ADDRIDX, "ping: invalid argument", argv[i]);
            args->optionsValue[optionIdx] = param;
            break;
        }
    }
}