#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

// A demo to help myself get to grips with multi-threading.

#define THREAD_COUNT 10
#define ARGS_COUNT 5

pthread_mutex_t lock;

void* thread_func(void* arg)
{
  int* args = (int*)arg;

  assert(pthread_mutex_lock(&lock) == 0);

  printf("RUNNING: thread with ");
  for (int i = 0; i < ARGS_COUNT; i++)
    {
      printf("%d, ", args[i]);
    }
  printf("\n");

  assert(pthread_mutex_unlock(&lock) == 0);
  
  pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
  pthread_t threads[THREAD_COUNT];
  int args[THREAD_COUNT][ARGS_COUNT];

  assert(pthread_mutex_init(&lock, NULL) == 0);

  srand(time(NULL));

  for (int i = 0; i < THREAD_COUNT * ARGS_COUNT; i++)
    {
      args[i / ARGS_COUNT][i % ARGS_COUNT] = rand() % 100;
    }

  for (int i = 0; i < THREAD_COUNT; i++)
    {
      if (pthread_create(&threads[i], NULL, thread_func, &args[i]) != 0)
	{
	  printf("ERROR: creating thread\n");
	  return 1;
	}
    }

  for (int i = 0; i < THREAD_COUNT; i++)
    {
      pthread_join(threads[i], NULL);
    }
  
  printf("All threads completed.\n");
  
  return 0;
}
