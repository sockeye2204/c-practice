#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#define THREAD_COUNT 8

typedef struct _scounter {
  int             val;
  pthread_mutex_t lock;
} scounter;

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

//

void print_time(struct timeval* tv)
{
  printf("tv = %ld.%ld secs\n", tv->tv_sec, tv->tv_usec);
}

void* many_inc_scounter(void* arg)
{
  scounter* sc = (scounter*)arg;
  for (int i = 0; i < 10000000; i++)
    {
      inc_scounter(sc);
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
  return 0;
}
