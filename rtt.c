#include "rtt.h"

void rttUpdate(t_rtt *rtt, long rtt_microseconds, size_t rtt_count)
{
    rtt->rtt_sumdev += fabs(rtt_microseconds - rtt->rtt_avg);
    rtt->rtt_sum += rtt_microseconds;
    if (rtt->rtt_min == 0 || rtt_microseconds < rtt->rtt_min)
        rtt->rtt_min = rtt_microseconds;
    if (rtt->rtt_max == 0 || rtt_microseconds > rtt->rtt_max)
        rtt->rtt_max = rtt_microseconds;
    rtt->rtt_mdev = rtt->rtt_sumdev / (double) (rtt_count);
    rtt->rtt_avg = rtt->rtt_sum / (double) (rtt_count);
}