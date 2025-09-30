#ifndef RTT_H
#define RTT_H

// size_t
#include <stdlib.h>
// fabs, sqrt
#include <math.h>

typedef struct s_rtt
{
   double   min;
   double   max;
   
   size_t   pkg_received;
   size_t   pkg_sent;
   double   mdev;
   double   mean;
   double   M2;
} t_rtt;

void rttUpdate(t_rtt *rtt, long rtt_microseconds);
void finalizeMeanDeviation(t_rtt *rtt);
void updateMeanDeviation(t_rtt *rtt, double rtt_microseconds);

#endif