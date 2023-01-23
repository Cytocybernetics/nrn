#ifndef CYTO_SYNC_H
#define CYTO_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdbool.h>

typedef enum {
    CytoBarrierStart,
    CytoBarrierLoopIndex,
    CytoBarrierMidpoint,
    CytoBarrierTiming,
    CytoBarrierEnd,
    CytoBarrierSize // Must be last
} cyto_barrier_id;

typedef int cyto_barrier_reason;

#define CYTO_BARRIER_REASON_OK 0
#define CYTO_BARRIER_REASON_TIMEOUT 1
#define CYTO_BARRIER_REASON_BREAK 2
#define CYTO_BARRIER_REASON_USER 1000

void cyto_sync_init(bool creator);
void cyto_sync_cleanup();
void cyto_barrier_break(size_t barrier_index, cyto_barrier_reason reason);
void cyto_barrier_set_timeout(size_t barrier_index, struct timespec timeout);
cyto_barrier_reason cyto_barrier_wait(cyto_barrier_id barrier_id);

#ifdef __cplusplus
}
#endif

#endif /* CYTO_SYNC_H */