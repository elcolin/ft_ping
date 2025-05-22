#ifndef RTT_H
#define RTT_H

// size_t
#include <stdlib.h>
// fabs
#include <math.h>

typedef struct s_rtt
{
   double         rtt_min;
   double         rtt_max;
   double         rtt_sum;
   double         rtt_avg;
   double         rtt_sumdev;
   double         rtt_mdev;
} t_rtt;

void rttUpdate(t_rtt *rtt, long rtt_microseconds, size_t rtt_count);

#endif