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
  struct Job* head;
  int numJobs;
};

struct Job {
  int jobId; // Mostly used to make logging easier.
  int timeInQueue;
  struct Job* next;
  // Simulation-related - these are set values that would not be present in a
  // 'real-time' system. Jobs would run for an unknown time, would enter the
  // system at an unknown time - but for the sake of simulation we pre-
  // define their run and entry times.
  int runTime;
  int entryTime;
};

static struct Queue sQueues[QUEUE_COUNT] = {0}; 

static int sCounter = 0; // This is a 'pseudo' time-slice counter variable.
                         // We do not check real run-times, but run-times as
                         // given in the input that run for n increments of
                         // gCounter.

static int sMainState = 0;

static FILE* fptr = NULL;

void ResetJobs(void)
{
  int i;
  struct Job* last = NULL;
  struct Job* cur = NULL;
  
  for (i = 0; i < QUEUE_COUNT; i++)
    {
      sQueues[i].numJobs = 0;
      if (sQueues[i].head != NULL)
	{
	  cur = sQueues[i].head;
	  if (sQueues[0].head == NULL && last == NULL) // Need to set [0]'s head
	    {
	      sQueues[0].head = cur;
	    }
	  while(cur != NULL)
	    {
	      printf("Job %d returned to queue 0 and time in queue set to 0\n", sQueues[0].numJobs);
	      sQueues[0].numJobs++;
	      cur->timeInQueue = 0; // Reset time used
	      if (last != NULL)
		{
		  last->next = cur;
		}
	      last = cur;
	      cur = cur->next;
	    }
	}
    }
  last->next = NULL; // Very last job
}

void AddJobToQueue(struct Job*, int queueNum)
{
}

void CompleteJob(struct Job* job)
{
  
}

int main(int argc, char *argv[])
{
  while(1)
 {
  switch(sMainState)
    {
    case 0:
      if (argc != 2)
	{
	  fprintf(stderr, "Usage: mlfq <csv-with-job-info>\n");
	  return(1);
	}
      sMainState++;
      break;
    case 1:
      fptr = fopen(argv[1], "r");

      if (fptr == NULL)
	{
	  fprintf(stderr, "Error: file could not be opened\n");
	  return(2);
	}
      sMainState++;
      break;
    case 2:
      char readBuffer[20];
      struct Job* lastJob = NULL;
      int counter = 0;

      char* tokPtr;

      while (fgets(readBuffer, 20, fptr)) {
	struct Job* curJob = malloc(sizeof(struct Job));
	if (curJob == NULL)
	  {
	    fprintf(stderr, "Error: malloc failed for job.");
	    return(5);
	  }
	curJob->jobId = counter;
      tokPtr = strtok(readBuffer, ",");
      if (tokPtr == NULL)
	{
	  fprintf(stderr, "Line format: <runTime>,<entryTime>\n");
	  return(3);
	}
      curJob->runTime = atoi(tokPtr);
      tokPtr = strtok(NULL, ",");
      if (tokPtr == NULL)
	{
	  fprintf(stderr, "Line format: <runTime>,<entryTime>\n");
	  return(4);
	}
      curJob->entryTime = atoi(tokPtr);
      curJob->timeInQueue = 0;
      if (lastJob != NULL)
	{
	  lastJob->next = curJob;
	}
      else
	{
	  sQueues[0].head = curJob;
	}

      sQueues[0].numJobs++;
      counter++;

      printf("Job %d loaded: runTime %d, entryTime %d\n", curJob->jobId, curJob->runTime, curJob->entryTime);
	    
      lastJob = curJob;
      }

      sMainState++;
      break;
    case 3:
      ResetJobs();
      sMainState++;
      break;
    case 4:
    case 5:
      return(0); // Completed without error.
    }
    }
}
