#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "sync.h"

static const char* shm_token = "/home/cytocybernetics/Cybercyte/DC1/SourceFiles/Shared_Memory/barriers";
static int shm_id = 1001;

typedef struct
{
    unsigned int count;
    atomic_uint spaces;
    atomic_uint generation;
    bool abort;
    cyto_barrier_reason reason;
    long long timeout;
} cyto_barrier_t;

static cyto_barrier_t* barriers;

void cyto_barrier_init(cyto_barrier_t* barrier, int n)
{
    barrier->count = n;
    barrier->spaces = n;
    barrier->generation = 0;
    barrier->abort = false;
    barrier->reason = CYTO_BARRIER_REASON_OK;
    barrier->timeout = 0;
}

void cyto_sync_init(bool creator)
{
    key_t key = ftok(shm_token, shm_id);

    if (key == -1) {
        fprintf(stderr, "FATAL ERROR! Token for barriers failed: %s [%d]", strerror(errno), errno);
        exit(1);
    }

    int shmid = shmget(key, CytoBarrierSize*sizeof(cyto_barrier_t), 0666 | IPC_CREAT);

    if (shmid == -1) {
        fprintf(stderr, "FATAL ERROR! Allocating barriers shared memory failed: %s [%d]", strerror(errno), errno);
        exit(1);
    }

    barriers = (cyto_barrier_t*)shmat(shmid, NULL, 0);

    if (barriers == NULL) {
        fprintf(stderr, "FATAL ERROR! Barriers shared memory segment is null!\n");
        exit(1);
    }

    if (creator) {
        for (size_t i = 0; i < CytoBarrierSize; ++i) {
            cyto_barrier_init(&barriers[i], 2);
        }
    } else {

    }
}

void cyto_sync_cleanup()
{
    shmdt(barriers);
}

void cyto_barrier_break(size_t barrier_index, cyto_barrier_reason reason)
{
    cyto_barrier_t* barrier = &barriers[barrier_index];
    barrier->reason = reason;
    barrier->abort = true;
}

void cyto_barrier_set_timeout(size_t barrier_index, struct timespec timeout)
{
    barriers[barrier_index].timeout = timeout.tv_sec * 1000000000 + timeout.tv_nsec;
}

static void cyto_barrier_reset(cyto_barrier_t* barrier)
{
    barrier->spaces = barrier->count;
    barrier->generation = 0;
    barrier->abort = false;
}

cyto_barrier_reason cyto_barrier_wait(cyto_barrier_id barrier_index)
{
    cyto_barrier_t* barrier = &barriers[barrier_index];
    unsigned int my_generation = barrier->generation;

    barrier->reason = CYTO_BARRIER_REASON_OK;

    if (!--barrier->spaces) {
        barrier->spaces = barrier->count;
        barrier->generation++;
    } else {
        struct timespec ts_start;
        struct timespec ts_current;
        long long start;
        long long current;

        clock_gettime(CLOCK_MONOTONIC, &ts_start);
        start = ts_start.tv_sec * 1000000000 + ts_start.tv_nsec;

        while (barrier->generation == my_generation) {
            if (barrier->abort) {
                cyto_barrier_reset(barrier);
                return barrier->reason;
            }

            if (barrier->timeout == 0)
                continue;

            clock_gettime(CLOCK_MONOTONIC, &ts_current);
            current = ts_current.tv_sec * 1000000000 + ts_current.tv_nsec;

            if ((current - start) > barrier->timeout) {
                fprintf(stderr, "WARNING: Cyto Barrier %d Timed Out\n", barrier_index);
                cyto_barrier_reset(barrier);
                return CYTO_BARRIER_REASON_TIMEOUT;
            }
        }
    }

    return CYTO_BARRIER_REASON_OK;
}