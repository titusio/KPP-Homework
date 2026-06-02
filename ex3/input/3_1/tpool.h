#ifndef TPOOL_H
#define TPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file tpool.h
 * @brief Bounded pthread-based thread pool API.
 *
 * Based on the implementation described in "Parallel Programming: for Multicore
 * and Cluster Systems" by Thomas Rauber and Gudula Rünger.
 */

/**
 * @brief Function signature for jobs executed by the thread pool.
 *
 * The function receives the argument pointer that was passed to
 * tpool_insert(). The caller owns the memory pointed to by @p arg.
 */
typedef void (*job_function)(void *);

/**
 * @brief Opaque thread pool handle.
 */
typedef struct tpool tpool_t;

/**
 * @brief Create and start a thread pool.
 *
 * A pool with @p num_threads worker threads is created. The internal queue is
 * bounded by @p max_size jobs. If the queue is full, producers calling
 * tpool_insert() block until capacity becomes available or shutdown starts.
 *
 * @param num_threads Number of worker threads to create. Must be greater than
 * 0.
 * @param max_size Maximum number of queued jobs. Must be greater than 0.
 * @return Pointer to an initialized pool on success, or NULL on failure.
 */
tpool_t *tpool_init(int num_threads, int max_size);

/**
 * @brief Submit one job to the thread pool.
 *
 * If the queue is full, this call blocks until space is available. If shutdown
 * was initiated, insertion fails.
 *
 * @param tpl Thread pool handle returned by tpool_init().
 * @param routine Job function to execute.
 * @param arg User data passed to @p routine.
 * @return 0 on success, -1 on failure or when the pool is shutting down.
 */
int tpool_insert(tpool_t *tpl, job_function routine, void *arg);

/**
 * @brief Shut down the thread pool and release all associated resources.
 *
 * This function signals workers to stop after processing already queued jobs,
 * waits for all worker threads to terminate, and frees pool resources.
 *
 * @param tpl Thread pool handle. Passing NULL is allowed and has no effect.
 */
void tpool_shutdown(tpool_t *tpl);

#ifdef __cplusplus
}
#endif

#endif // TPOOL_H
