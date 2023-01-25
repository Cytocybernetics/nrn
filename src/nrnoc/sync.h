#ifndef CYTO_SYNC_H
#define CYTO_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdbool.h>

/**
 * @brief Identifiers for the barriers.
 * 
 * The enum contains all valid identifiers for the barriers. Adding a new identifier will allocate another barrier,
 * no other changes are required in the implementation to make it available.
 * 
 * It is important that CytoBarrierSize exists and is listed last, as it is used as the number of barriers, and is
 * not an actual barrier id itself.
 * 
 */
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

/**
 * @brief Initialize the synchronization barriers.
 * 
 * As the implementation uses shared memory, only one process must actually create
 * the barriers, while the other simply maps it into memory.
 * 
 * Currently the barrier implementation expect there to be two threads or processes
 * that will synchronize their execution. Changes to initialization code would be
 * required in initialization to make this customizable for different barriers by
 * specifying the required number of threads or processes for it.
 * 
 * @param creator Flag indicating whether tot create and initialize the barriers or not
 */
void cyto_sync_init(bool creator);

/**
 * @brief Free up barrier resources after they are no longer needed
 * 
 */
void cyto_sync_cleanup();

/**
 * @brief Force barrier to break out of its wait state.
 * 
 * Calling this function will force the barrier to stop waiting and return the given reason (which can be some custom value).
 * The place in the code where the barrier was waited upon can then make use of the reason to determine how to proceed if the
 * value was not CYTO_BARRIER_REASON_OK.
 * 
 * @param barrier_index The barrier to break out of
 * @param reason The reason for breaking out of the wait state
 */
void cyto_barrier_break(size_t barrier_index, cyto_barrier_reason reason);

/**
 * @brief Define the timeout value for the specified barrier.
 * 
 * The barrier index must be in the range 0 to CytoBarrierSize-1.
 * 
 * @param barrier_index The barrier index
 * @param timeout The timeout value
 */
void cyto_barrier_set_timeout(size_t barrier_index, struct timespec timeout);

/**
 * @brief Wait for synchronization with another process using the specified barrier.
 * 
 * The barrier index must be in the range 0 to CytoBarrierSize-1.
 * 
 * @param barrier_id 
 * @return cyto_barrier_reason 
 */
cyto_barrier_reason cyto_barrier_wait(cyto_barrier_id barrier_id);

#ifdef __cplusplus
}
#endif

#endif /* CYTO_SYNC_H */