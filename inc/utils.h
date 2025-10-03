#ifndef UTILS_H
#define UTILS_H

//abs
#include <stdlib.h>

typedef enum
{
   FALSE,
   TRUE
}  bool;

typedef enum
{
   SUCCESS,
   FAILURE
}  status;

int countDigits(int n);
int defineDecimals(int digits);
int getDecimalsToPrint(double value);

#endif