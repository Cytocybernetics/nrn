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

/**
 * @brief Shared memory file reference used with ftok()
 * 
 */
static const char* shm_token = "/home/cytocybernetics/Cybercyte/DC1/SourceFiles/Shared_Memory/barriers";

/**
 * @brief Shared memory project id used with ftok()
 * 
 */
static int shm_id = 1001;

typedef struct
{
    /**
     * @brief Number of threads or processes that will wait at the barrier.
     * 
     */
    unsigned int count;

    /**
     * @brief The number of threads or processes remaining to arrive at the barrier.
     * 
     */
    atomic_uint spaces;

    /**
     * @brief The current generation of waiting threads or processes.
     * 
     * A thread or process that is not last to arrive while spin-wait for its stored
     * generation value at the time of starting to wait to be different from the
     * current generation value of the barrier.
     * 
     * When the last thread or process to arrive at the barrier increments the
     * generation value of the barrier, all waiting threads or processes notices this
     * change and are then released from the barrier.
     */
    atomic_uint generation;

    /**
     * @brief A flag indicating whether to abort the waiting at the barrier.
     * 
     */
    bool abort;

    /**
     * @brief The reason for aborting the wait at the barrier if it was performed programmatically by another thread or process
     * 
     */
    cyto_barrier_reason reason;

    /**
     * @brief A timeout value for the barrier
     * 
     * If the timeout value is 0 the barrier will prevent further execution indefinitely
     * until all threads or processes has arrived at it.
     * 
     */
    long long timeout;
} cyto_barrier_t;

/**
 * @brief A dynmically allocated array or barriers whose size is determined by the CytoBarrierSize enum value
 * 
 */
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

    // Allocate enough space for all barriers in the shared memory segment
    int shmid = shmget(key, CytoBarrierSize*sizeof(cyto_barrier_t), 0666 | IPC_CREAT);

    if (shmid == -1) {
        fprintf(stderr, "FATAL ERROR! Allocating barriers shared memory failed: %s [%d]", strerror(errno), errno);
        exit(1);
    }

    // Get a pointer for the barrier array
    barriers = (cyto_barrier_t*)shmat(shmid, NULL, 0);

    if (barriers == NULL) {
        fprintf(stderr, "FATAL ERROR! Barriers shared memory segment is null!\n");
        exit(1);
    }

    if (creator) { // Initialize all barriers if I am the creator
        for (size_t i = 0; i < CytoBarrierSize; ++i) {
            // Initialize the barrier with an expected number of 2 threads or processes
            // In the future we may want to make this more customizable on a per barrier
            // basis, but for now it is a fixed number.
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

/**
 * @brief Private function to reset barrier to a known usable state
 * 
 * @param barrier 
 */
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

    if (!--barrier->spaces) { // I was last to arrive at barrier, increment generation
        barrier->spaces = barrier->count;
        barrier->generation++;
    } else { // I was not last to arrive at barrier, wait for generation increment (or timeout/break)
        struct timespec ts_start;
        struct timespec ts_current;
        long long start;
        long long current;

        clock_gettime(CLOCK_MONOTONIC, &ts_start);
        start = ts_start.tv_sec * 1000000000 + ts_start.tv_nsec;

        // Wait until the last thread or process to arrive increments the generation
        // or a timeout or break condition occurs
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