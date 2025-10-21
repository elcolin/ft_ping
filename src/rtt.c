#include "rtt.h"

void rttUpdate(t_rtt *rtt, long rtt_microseconds)
{
    updateMeanDeviation(rtt, (double) rtt_microseconds);
    if (rtt->min == 0 || rtt_microseconds < rtt->min)
        rtt->min = rtt_microseconds;
    if (rtt->max == 0 || rtt_microseconds > rtt->max)
        rtt->max = rtt_microseconds;
}

void updateMeanDeviation(t_rtt *rtt, double rtt_microseconds)
{
    double delta = rtt_microseconds - rtt->mean;
    rtt->mean += (double) delta / (double) rtt->pkg_received;
    double delta2 = rtt_microseconds - rtt->mean;
    rtt->M2 += delta * delta2;
}

void finalizeMeanDeviation(t_rtt *rtt)
{
    rtt->mdev = sqrt((double) rtt->M2 / (double) rtt->pkg_received);
}