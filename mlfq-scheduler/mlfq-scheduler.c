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

#define ASSERTS_ENABLED (argc == 3 && argv[2] != "0")

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

static FILE* outptr;

static void ResetJobs(void)
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
	      fprintf(outptr, "Job %d returned to queue 0 and time in queue set to 0\n", cur->jobId);
	      
	      sQueues[0].numJobs++;
	      cur->timeInQueue = 0; // Reset time used

	      if (last != NULL)
		{
		  last->next = cur;
		}
	      else
		{
		  sQueues[0].head = cur;
		}
	      
	      last = cur;
	      cur = cur->next;
	    }
	}
    }
  last->next = NULL; // Very last job
}

static void AddJobToQueue(struct Job*, int queueNum)
{
}

static int CompleteJob(struct Job* cur)
{
  int i;
  struct Job* prev = NULL;

  for (i = 0; i < QUEUE_COUNT; i++)
    {
	  if (sQueues[i].head == cur)
	    {
	      sQueues[i].head = cur->next;
	      sQueues[i].numJobs--;
	      fprintf(outptr, "Job %d completed.\n", cur->jobId);
	      free(cur);
	      return(0);
	    }

	  prev = sQueues[i].head;
	  while(prev != NULL && prev->next != NULL)
	    {
	      if (prev->next == cur)
		{
		  prev->next = cur->next;
		  sQueues[i].numJobs--;
		  fprintf(outptr, "Job %d completed.\n", cur->jobId);
		  free(cur);
		  return(0);
		}
	      prev = prev->next;
	    }
    }
  return(1); // Could not find job to remove
}

static int DowngradeJob(struct Job* cur)
{
  int i;
  struct Job* prev = NULL;

  for (i = 0; i < QUEUE_COUNT; i++)
    {
	  prev = sQueues[i].head;

	  if (prev == cur)
	    {
	      sQueues[i].head = cur->next;
	      break;
	    }

	  while(prev != NULL)
	    {
	      if (prev->next == cur)
		{
		  prev->next = cur->next;
		  break;
		}
	      prev = prev->next;
	    }

	  if (prev != NULL)
	    break;
    }
  if (prev == NULL)
    {
      return(1); // Could not find job to downgrade
    }
  if (i == QUEUE_COUNT)
    {
      return(2); // Downgrading would put us OOB
    }

  sQueues[i].numJobs--;
  sQueues[i+1].numJobs++;

  prev = sQueues[i+1].head;

  if (prev == NULL)
    {
      sQueues[i+1].head = cur;
    }
  else
    {
      fprintf(outptr, "Go into job %d at %p\n", prev->jobId, prev);
      fflush(stdout);
      while(prev->next != NULL)
	{
	  prev = prev->next;
	}
        prev->next = cur;
    }

  cur->timeInQueue = 0;
  cur->next = NULL;
  
  fprintf(outptr, "Job %d downgraded to queue %d.\n", cur->jobId, i+1);
  return(0);
}

static int RunMLFQScheduler(void)
{
  int i, skips, numJobs;
  struct Job* chosenJob;

  for (i = 0; i < QUEUE_COUNT; i++)
    {
      if (sQueues[i].head != NULL)
	{
	  numJobs = sQueues[i].numJobs;
	  // Get the index relative to jobs in the queue
	  skips = sCounter % numJobs;

	  chosenJob = sQueues[i].head;

	  while(skips != 0)
	    {
	      skips--;
	      chosenJob = chosenJob->next;
	    }
	  fprintf(outptr, "Chosen job at %p out of %d (skips: %d)\n", chosenJob, numJobs, skips);
	  
	  chosenJob->runTime--;
	  chosenJob->timeInQueue++;

	  fprintf(outptr, "Running job %d (runtime %d), has been in queue %d for %d counts. Total counts: %d\n",
		 chosenJob->jobId, chosenJob->runTime, i, chosenJob->timeInQueue, sCounter);
	  
	  if(chosenJob->runTime == 0)
	    {
	      CompleteJob(chosenJob);
	    }
	  else if (chosenJob->timeInQueue >= sQueues[i].allotment)
	    {
	      DowngradeJob(chosenJob);
	    }

	  sCounter++;

	  return 1; // We ran something.
	}
    }
  
  return 0; // No jobs to run, we are done.
}

static void AssertJobCounts(void)
{
  int i;
  int counter;
  struct Job* cur = NULL;
  
  
  for (i = 0; i < QUEUE_COUNT; i++)
    {
      counter = 0;

      cur = sQueues[i].head;

      while(cur != NULL)
	{
	  counter++;
	  cur = cur->next;
	}

      assert(counter == sQueues[i].numJobs);
    }
}

int main(int argc, char *argv[])
{
  while(1)
 {
  switch(sMainState)
    {
    case 0:
      if ASSERTS_ENABLED
	{
	  printf("Assertions ENABLED\n");
	}
      if (argc < 2 && argc > 4)
	{
	  fprintf(stderr, "Usage: mlfq <csv-with-job-info> (enable-asserts) (out)\n");
	  return(1);
	}
      sMainState++;
      break;
    case 1:
      fptr = fopen(argv[1], "r");
      if (argc == 4)
	{
	  outptr = fopen(argv[3], "w");
	}
      else
	{
	  outptr = stdout;
	}

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
      int counter;

      char* tokPtr;

      for (counter = 0; counter < QUEUE_COUNT; counter++)
	{
	  sQueues[counter].allotment = (counter+1)*10;
	}
      counter = 0;

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

      fprintf(outptr, "Job %d loaded: runTime %d, entryTime %d\n", curJob->jobId, curJob->runTime, curJob->entryTime);
	    
      lastJob = curJob;
      }

      sMainState++;
      break;
    case 3:
      ResetJobs();
      sMainState++;
      break;
    case 4:
      // This case asserts that jobs exist in queues as expected.
      if ASSERTS_ENABLED
	{
	  AssertJobCounts();
	}
      sMainState++;
      break;
    case 5:
      while (RunMLFQScheduler())
	{
	  if (sCounter % RESET_TIMESTEP == 0)
	    {
	      ResetJobs();
	    }
	  sMainState = 4;	  
	}
      sMainState++;
      break;
    case 6:
      return(0); // Completed without error.
    }
    }
}
