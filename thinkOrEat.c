/********************************************************************************
* File: thinkOrEat.c
* Author: Anuja Sahu
* Procedures:
* thread_info - Struct that defines variables and attributes for each thread.
* philosopher - Creates a philosopher that eats and thinks. Prints which
* philosopher is working.
* main - Test program which initializes rooms, philosophers, and prints results
* of time spent eating and thinking for each philosopher.
********************************************************************************/

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>

sem_t forks[5] = {1};		// Initialize semaphores for forks
sem_t room = {4};		// Initialize semaphores for rooms

/*********************************************************************************
* struct thread_info
* Author: Anuja Sahu
* Date: 19 October 2021
* Description: Define attributes and variables for a thread.
*********************************************************************************/
struct thread_info
{
	pthread_t thread_id;	// Variable for each thread's id
	int       thread_num;	// Variable for each thread's number
	int       eat;		// Variable for number of times eating
	int	  think;	// Variable for number of times thinking
	int	  eatingTime;	// Variable that stores total eating time
	int 	  thinkingTime; // Variable that stores total thinking time
};

/*********************************************************************************
* void philosopher(void *arg)
* Author: Anuja Sahu
* Date: 19 October 2021
* Description: Function that creates a philosopher, displays which one is
* currently working, and runs thinking and eating functionalities.
* The operations of eat and think are millisecond sleeps of random
* duration (25 to 49 milliseconds each).
*
* Parameters:
* arg		I/P	 void*	The arguments on the command line
* philosopher	O/P	 void	Status code (not currently used)
*********************************************************************************/
static void *philosopher(void *arg)
{
	struct thread_info *tinfo = arg;	// Create a thread
	srand(time(0));				// Random generator for time
	int time;				// Variable that stores time

	// Display which philosopher has begun working
	printf("Philosopher #%d is now working.\n", tinfo->thread_num);

	// Continue thinking and eating until threads terminate
	while(1)
	{
		// Think operation
		tinfo -> think++;

		// Time randomly generated between 25 and 49 milliseconds
		time = (1000 * (rand() % 24 + 25));
		tinfo -> thinkingTime += time;	// Thinking time count
		usleep(time);			// End of thinking activity

		sem_wait(&room);		// Wait to enter room

		// Pick up left and right forks
		sem_wait(&forks[tinfo -> thread_num]);
		sem_wait(&forks[(tinfo -> thread_num + 1) % 5]);

		// Eat operation
		tinfo -> eat++;

		// Time randomly generated between 25 and 49 milliseconds
		time = (1000 * (rand() % 24 + 25));
		tinfo -> eatingTime += time;	// Eating time count
		usleep(time);			// End of eating activity

		// Put down right and left forks
		sem_post(&forks[(tinfo -> thread_num + 1) % 5]);
		sem_post(&forks[tinfo -> thread_num]);

		sem_post(&room);		// Exit room
	}
}

/*********************************************************************************
* int main(int argc, char *argv[])
* Author: Anuja Sahu
* Date: 19 October 2021
* Description: Initialize rooms and philosophers.
* Prints the amount of time (in miliseconds) that it takes each philosopher
* to eat and think.
*
* Parameters:
* argc		I/P	int	The number of arguments on the command line
* argv		I/P	char*[] The arguments on the command line
* main 		O/P	int	Status code (not currently used)
********************************************************************************/
int main(int argc, char *argv[])
{
	int s, tnum, opt, num_threads, start;
	struct thread_info * tinfo;		// Create main thread
	pthread_attr_t attr;			// Declare attribute for thread

	// Initialize semaphore for rooms
	sem_init(&room, 0, 4);
	for (int i = 1; i < 5; i++)
	{
		sem_init(&forks[i], 0, 1);
	}

	// Initialize number of threads received from command line
	num_threads = argc;
	s = pthread_attr_init(&attr);		// Initialize attribute

	// Allocate memory in main thread
	tinfo = calloc(num_threads, sizeof(struct thread_info));

	start = time(NULL);			// Start time of threads

	// Create threads for each philosopher and set their numbers
	for(tnum = 0; tnum < num_threads; tnum++)
	{
		tinfo[tnum].thread_num = tnum;

		s = pthread_create(&tinfo[tnum].thread_id, &attr, &philosopher, &tinfo[tnum]);
	}

	s = pthread_attr_destroy(&attr);	// Destroy thread

	while(1)
	{
		// Make sure threads run for five minutes
		if (difftime(time(NULL), start) >= 100)
		{
			// Print out number of times and how much time
			// each philosopher spends completing each activity
			for (int i = 0; i < num_threads; i++)
			{
				printf("\nPhilosopher #%d ate %d times for %d milliseconds.\n", tinfo[i].thread_num, tinfo[i].eat, tinfo[i].eatingTime/1000);
				printf("Philosopher #%d was thinking %d times for %d milliseconds.\n", tinfo[i].thread_num, tinfo[i].think, tinfo[i].thinkingTime/1000);
				printf("\n");
	    			pthread_detach(tinfo[i].thread_id);
	    		}
			break;
	    	}
	}

	free(tinfo);				// End use of thread
	exit(EXIT_SUCCESS);			// End program
}
