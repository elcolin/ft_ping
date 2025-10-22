#include "utils.h"

uint64_t get_elapsed_ms(struct timeval start, struct timeval end)
{
    long seconds  = end.tv_sec - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;

    return (uint64_t)(seconds * 1000L + useconds / 1000L);
}

int countDigits(int n)
{
    int i = 0;
    while (n)
    {
        n /= 10;
        i++;
    }
    return i;
}

int defineDecimals(int digits)
{
    if (digits <= 2)
        return 3;
    if (digits < 6)
        return abs(digits - 6);
    return 0;
}

int getDecimalsToPrint(double value)
{
    return defineDecimals(countDigits(value));
}

void triggerErrorNoFreeingIf(bool condition, char *msg, char *reason)
{
    if (condition)
    {
        fprintf(stderr, "%s: %s", msg, reason);
        exit(EXIT_FAILURE);
    }
}