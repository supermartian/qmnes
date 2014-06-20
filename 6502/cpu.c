/*
 * cpu.c
 * Copyright (C) 2014 martian <supermartian@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <time.h>
#include <string.h>
#include <stdint.h>

#include "cpu.h"

#ifdef __MACH__
/*
 * Well, OSX doesn't have clock_gettime implemented.
 */
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
#include <sys/time.h>
//clock_gettime is not implemented on OSX
int clock_gettime(int clk_id, struct timespec* t)
{
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#endif

struct timespec span = {.tv_sec = 0, .tv_nsec = 5};

static struct timespec timediff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;

    return temp;
}

/*
 * CPU initialization. Just set the frequency.
 *
 * */
void cpu_init(struct cpu_6502 *p, float cpu_freq)
{
    memset(p, 0, sizeof(struct cpu_6502));
    p->cycle = 1000000000 / cpu_freq; // In nanoseconds
    clock_gettime(CLOCK_REALTIME, &(p->last_tick));
}

/*
 * Make sure the execution takes the specified time of cycles.
 * Must be called after every instruction with its corresponding cycles.
 *
 * */
void cycle_tick(struct cpu_6502 *p, int cycles)
{
    struct timespec diff, current, expect;

    expect.tv_sec = (p->last_tick).tv_sec;
    expect.tv_nsec = (p->last_tick).tv_nsec + cycles * p->cycle;

    for(;;) {
        clock_gettime(CLOCK_REALTIME, &(current));
        diff = timediff(expect, current);
        if (diff.tv_sec <= 0 &&
                diff.tv_nsec <= 0) {
            return;
        }
        nanosleep(&span, NULL);
    }

    clock_gettime(CLOCK_REALTIME, &(p->last_tick));
}
