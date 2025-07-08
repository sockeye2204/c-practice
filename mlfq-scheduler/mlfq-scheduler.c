#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
This is a 'toy' multi-level feedback queue implementation.
It takes in a set of jobs and queues, and outputs a log file
showing the job ran at each timestep and change in job positions
within queues.
 */

#define QUEUE_COUNT 20

#define RESET_TIMESTEP 50

struct Queue {
  int allotment;
};

struct Job {
  int timeInQueue;
  struct Queue* curQueue;
  int runtime;
};

static int gCounter = 0; // This is a 'pseudo' time-slice counter variable.
                         // We do not check real run-times, but run-times as
                         // given in the input that run for n increments of
                         // gCounter.

int main(int argc, char *argv[])
{
  return 0;
}
