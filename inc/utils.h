#ifndef UTILS_H
#define UTILS_H

//abs
#include <stdlib.h>
#include <stdio.h>

//time.h
#include <sys/time.h>

//uint64_t
#include <stdint.h>

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
void triggerErrorNoFreeingIf(bool condition, char *msg, char *reason);
uint64_t get_elapsed_ms(struct timeval start, struct timeval end);

#endif