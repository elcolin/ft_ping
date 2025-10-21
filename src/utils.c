#include "utils.h"

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