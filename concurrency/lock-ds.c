#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#define THREAD_COUNT 16
#define MAXCPUS 4

// Simple counter (one lock)
typedef struct _scounter {
  int             val;
  pthread_mutex_t lock;
} scounter;

// Approximate counter (local locks & global lock)

typedef struct _acounter {
  int gval;
  pthread_mutex_t glock;
  int lvals[MAXCPUS];
  pthread_mutex_t llocks[MAXCPUS];
  int threshold;
} acounter;

// Wrappers (for catching mutex, thread function failures)

void Pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
  int ret = pthread_mutex_init(mutex, mutexattr);
  if (ret)
    {
      fprintf(stderr, "Mutex init failed. Exiting\n");
      exit(1);
    }
}

void Pthread_mutex_lock(pthread_mutex_t *mutex)
{
  int ret = pthread_mutex_lock(mutex);
  if (ret)
    {
      fprintf(stderr, "Mutex lock failed. Exiting\n");
      exit(1);
    }
}

void Pthread_mutex_unlock(pthread_mutex_t *mutex)
{
  int ret = pthread_mutex_unlock(mutex);
  if (ret)
    {
      fprintf(stderr, "Mutex unlock failed. Exiting\n");
      exit(1);
    }
}

void Pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, typeof(void *(void *)) *start_routine, void* restrict arg)
{
  int ret = pthread_create(thread, attr, start_routine, arg);
  if (ret)
    {
      fprintf(stderr, "Thread creation failed. Exiting\n");
      exit(1);
    }
}

// Simple counter (scounter): one lock for entire counter

void init_scounter(scounter* sc)
{
  sc->val = 0;
  Pthread_mutex_init(&sc->lock, NULL);
}

void inc_scounter(scounter* sc)
{
  Pthread_mutex_lock(&sc->lock);
  sc->val++;
  Pthread_mutex_unlock(&sc->lock);
}

void res_scounter(scounter* sc)
{
  Pthread_mutex_lock(&sc->lock);
  sc->val = 0;
  Pthread_mutex_unlock(&sc->lock);
}

int get_scounter(scounter* sc)
{
  Pthread_mutex_lock(&sc->lock);
  int ret = sc->val;
  Pthread_mutex_unlock(&sc->lock);
  return ret;
}

// Approximate counter functions

void init_acounter(acounter* ac, int threshold)
{
  ac->gval = 0;
  ac->threshold = threshold;
  Pthread_mutex_init(&ac->glock, NULL);
  for (int i = 0; i < MAXCPUS; i++)
    {
      ac->lvals[i] = 0;
      Pthread_mutex_init(&ac->llocks[i], NULL);
    }
}

void inc_acounter(acounter* ac, int threadId, int amount)
{
  int cpu = threadId % MAXCPUS;
  Pthread_mutex_lock(&ac->llocks[cpu]);
  ac->lvals[cpu] += amount;
  if (ac->lvals[cpu] >= ac->threshold)
    {
      Pthread_mutex_lock(&ac->glock);
      ac->gval += ac->lvals[cpu];
      Pthread_mutex_unlock(&ac->glock);
      ac->lvals[cpu] = 0;
    }
  Pthread_mutex_unlock(&ac->llocks[cpu]);
}

int get_acounter(acounter *ac)
{
  Pthread_mutex_lock(&ac->glock);
  int ret = ac->gval;
  Pthread_mutex_unlock(&ac->glock);
  return ret;
}

//

void print_time(struct timeval* tv)
{
  printf("tv = %ld.%06ld secs\n", tv->tv_sec, tv->tv_usec);
}

void* many_inc_scounter(void* arg)
{
  scounter* sc = (scounter*)arg;
  while(get_scounter(sc) < 10000000)
    {
      inc_scounter(sc);
    }
}

void* many_inc_acounter(void* arg)
{
  acounter* ac = (acounter*)arg;
  while(get_acounter(ac) < 10000000)
    {
      inc_acounter(ac, 1, pthread_self());
    }
  
}

void timeval_sub(struct timeval *tv2, struct timeval *tv1)
{
  if (tv2->tv_usec < tv1->tv_usec)
    {
      int nsec = (tv1->tv_usec - tv2->tv_usec) / 1000000 + 1;
      tv1->tv_usec -= 1000000 * nsec;
      tv1->tv_sec += nsec;
    }
  if (tv2->tv_usec - tv1->tv_usec > 1000000)
    {
      int nsec = (tv2->tv_usec - tv1->tv_usec) / 1000000;
      tv1->tv_usec += 1000000 * nsec;
      tv1->tv_sec -= nsec;
    }

  tv2->tv_sec = tv2->tv_sec - tv1->tv_sec;
  tv2->tv_usec = tv2->tv_usec - tv1->tv_usec;
}

int main(int argc, char* argv[])
{
  struct timeval tv1 = {0};
  struct timeval tv2 = {0};
  pthread_t threads[THREAD_COUNT];

  if(argc >= 2 && !(strcmp(argv[1], "time")))
    { 
      gettimeofday(&tv1, NULL);

      print_time(&tv1);
    }
  if(argc >= 2 && !(strcmp(argv[1], "sc")))
    {
      scounter sc;
      init_scounter(&sc);
      for (int i = 1; i <= THREAD_COUNT; i++)
	{
	  res_scounter(&sc);
	  gettimeofday(&tv1, NULL);
	  for(int j = 0; j < i; j++)
	    {
	      Pthread_create(&threads[j], NULL, many_inc_scounter, &sc); 
	    }
	  for(int j = 0; j < i; j++)
	    {
	      pthread_join(threads[j], NULL);
	    }
	  gettimeofday(&tv2, NULL);
	  timeval_sub(&tv2, &tv1);
	  print_time(&tv2);
	}
    }
  if (argc >= 2 && !(strcmp(argv[1], "ac")))
    {
      acounter ac;
      printf("Performance as threshold increases (threads: %d)\n", THREAD_COUNT);
      for (int i = 1; i <= 8192; i*=2)
	{
	  init_acounter(&ac, i);
	  gettimeofday(&tv1, NULL);
	  for (int j = 0; j < THREAD_COUNT; j++)
	    {
	      Pthread_create(&threads[j], NULL, many_inc_acounter, &ac);
	    }
	  for (int j = 0; j < THREAD_COUNT; j++)
	    {
	      pthread_join(threads[j], NULL);
	    }
	  gettimeofday(&tv2, NULL);
	  timeval_sub(&tv2, &tv1);
	  print_time(&tv2);	  
	}
      printf("Performance as thread count increases (threshold: 8)\n");
      for (int i = 1; i <= THREAD_COUNT; i++)
	{
	  init_acounter(&ac, 8);
	  gettimeofday(&tv1, NULL);
	  for (int j = 0; j < i; j++)
	    {
	      Pthread_create(&threads[j], NULL, many_inc_acounter, &ac);
	    }
	  for (int j = 0; j < i; j++)
	    {
	      pthread_join(threads[j], NULL);
	    }
	  gettimeofday(&tv2, NULL);
	  timeval_sub(&tv2, &tv1);
	  print_time(&tv2);	  
	}
    }
  return 0;
}
