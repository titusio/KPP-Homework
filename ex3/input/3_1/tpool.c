#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tpool.h"

typedef struct work {
  job_function routine;
  void *arg;
  struct work *next;
} work_t;

struct tpool {
  int num_threads;
  int max_size;
  int current_size;

  pthread_t *threads;

  work_t *head;
  work_t *tail;

  pthread_mutex_t lock;

  pthread_cond_t queue_has_items;
  pthread_cond_t queue_has_capacity;

  int finished;
};

void *tpool_thread(void *tpl_arg);
void tpool_shutdown(tpool_t *tpl);

static void tpool_perror_pthread(const char *msg, int err) {
  errno = err;
  perror(msg);
}

static void tpool_cleanup_on_init_error(tpool_t *tpl, int lock_initialized,
                                        int queue_has_items_initialized,
                                        int queue_has_capacity_initialized,
                                        int created_threads) {
  if (tpl == NULL) {
    return;
  }

  if (tpl->threads != NULL) {
    for (int i = 0; i < created_threads; i++) {
      pthread_cancel(tpl->threads[i]);
    }
    for (int i = 0; i < created_threads; i++) {
      pthread_join(tpl->threads[i], NULL);
    }
    free(tpl->threads);
  }

  if (queue_has_capacity_initialized) {
    pthread_cond_destroy(&tpl->queue_has_capacity);
  }
  if (queue_has_items_initialized) {
    pthread_cond_destroy(&tpl->queue_has_items);
  }
  if (lock_initialized) {
    pthread_mutex_destroy(&tpl->lock);
  }

  free(tpl);
}

tpool_t *tpool_init(int num_threads, int max_size) {
  tpool_t *tpl;
  int lock_initialized = 0;
  int queue_has_items_initialized = 0;
  int queue_has_capacity_initialized = 0;
  int created_threads = 0;

  tpl = (tpool_t *)calloc(1, sizeof(tpool_t));

  if (tpl == NULL) {
    perror("calloc");
    return NULL;
  }

  // initialization
  tpl->num_threads = num_threads;
  tpl->max_size = max_size;

  int err = pthread_mutex_init(&tpl->lock, NULL);
  if (err) {
    tpool_perror_pthread("pthread_mutex_init", err);
    tpool_cleanup_on_init_error(
        tpl, lock_initialized, queue_has_items_initialized,
        queue_has_capacity_initialized, created_threads);
    return NULL;
  }
  lock_initialized = 1;

  // initialize conditionals
  err = pthread_cond_init(&tpl->queue_has_items, NULL);
  if (err) {
    tpool_perror_pthread("pthread_cond_init(not_empty)", err);
    tpool_cleanup_on_init_error(
        tpl, lock_initialized, queue_has_items_initialized,
        queue_has_capacity_initialized, created_threads);
    return NULL;
  }
  queue_has_items_initialized = 1;

  err = pthread_cond_init(&tpl->queue_has_capacity, NULL);
  if (err) {
    tpool_perror_pthread("pthread_cond_init(not_full)", err);
    tpool_cleanup_on_init_error(
        tpl, lock_initialized, queue_has_items_initialized,
        queue_has_capacity_initialized, created_threads);
    return NULL;
  }
  queue_has_capacity_initialized = 1;

  tpl->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);

  if (tpl->threads == NULL) {
    perror("malloc");
    tpool_cleanup_on_init_error(
        tpl, lock_initialized, queue_has_items_initialized,
        queue_has_capacity_initialized, created_threads);
    return NULL;
  }

  for (int i = 0; i < num_threads; i++) {
    err = pthread_create(&tpl->threads[i], NULL, tpool_thread, (void *)tpl);
    if (err) {
      tpool_perror_pthread("pthread_create", err);
      tpool_cleanup_on_init_error(
          tpl, lock_initialized, queue_has_items_initialized,
          queue_has_capacity_initialized, created_threads);
      return NULL;
    }
    created_threads++;
  }

  return tpl;
}

void *tpool_thread(void *tpl_arg) {
  work_t *wl;
  tpool_t *tpl = (tpool_t *)tpl_arg;

  while (1) {
    pthread_mutex_lock(&tpl->lock);
    while (tpl->current_size == 0 && !tpl->finished) {
      pthread_cond_wait(&tpl->queue_has_items, &tpl->lock);
    }

    if (tpl->current_size == 0 && tpl->finished) {
      pthread_mutex_unlock(&tpl->lock);
      pthread_exit(NULL);
    }

    wl = tpl->head;
    tpl->current_size--;

    if (tpl->current_size == 0) {
      tpl->head = NULL;
      tpl->tail = NULL;
    } else {
      tpl->head = wl->next;
    }

    if (tpl->current_size == tpl->max_size - 1) {
      pthread_cond_broadcast(&tpl->queue_has_capacity);
    }

    pthread_mutex_unlock(&tpl->lock);

    // the real work starts here
    wl->routine(wl->arg);

    free(wl);
  }

  pthread_exit(NULL);
}

int tpool_insert(tpool_t *tpl, job_function routine, void *arg) {
  work_t *wl;

  pthread_mutex_lock(&tpl->lock);

  if (tpl->finished) {
    pthread_mutex_unlock(&tpl->lock);
    return -1;
  }

  while (tpl->current_size == tpl->max_size) {
    pthread_cond_wait(&tpl->queue_has_capacity, &tpl->lock);
    if (tpl->finished) {
      pthread_mutex_unlock(&tpl->lock);
      return -1;
    }
  }

  wl = (work_t *)calloc(1, sizeof(work_t));
  if (wl == NULL) {
    pthread_mutex_unlock(&tpl->lock);
    return -1;
  }

  wl->routine = routine;
  wl->arg = arg;

  if (tpl->current_size == 0) {
    tpl->tail = tpl->head = wl;
    pthread_cond_broadcast(&tpl->queue_has_items);
  } else {
    tpl->tail->next = wl;
    tpl->tail = wl;
  }

  tpl->current_size++;
  pthread_mutex_unlock(&tpl->lock);

  return 0;
}

void tpool_shutdown(tpool_t *tpl) {
  if (tpl == NULL) {
    return;
  }

  pthread_mutex_lock(&tpl->lock);
  tpl->finished = 1;
  pthread_cond_broadcast(&tpl->queue_has_items);
  pthread_cond_broadcast(&tpl->queue_has_capacity);
  pthread_mutex_unlock(&tpl->lock);

  for (int i = 0; i < tpl->num_threads; i++) {
    pthread_join(tpl->threads[i], NULL);
  }

  pthread_cond_destroy(&tpl->queue_has_items);
  pthread_cond_destroy(&tpl->queue_has_capacity);
  pthread_mutex_destroy(&tpl->lock);

  free(tpl->threads);
  free(tpl);
}