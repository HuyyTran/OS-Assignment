// Synchronization tasks - comment this to turn off synchronization
// #define SYNCH
//****************************************************************//
#include "queue.h"
#include "sched.h"
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
static struct queue_t ready_queue;
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
static struct queue_t mlq_ready_queue[MAX_PRIO];
static int slot[MAX_PRIO];
#endif

int queue_empty(void)
{
#ifdef MLQ_SCHED
	unsigned long prio;
	for (prio = 0; prio < MAX_PRIO; prio++)
		if (!empty(&mlq_ready_queue[prio]))
			return -1;
#endif
	return (empty(&ready_queue) && empty(&run_queue));
}

void init_scheduler(void)
{
#ifdef MLQ_SCHED
	int i;

	for (i = 0; i < MAX_PRIO; i++)
	{
		mlq_ready_queue[i].size = 0;
		slot[i] = 0;
	}
#endif
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

#ifdef MLQ_SCHED
/*
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
 */
struct pcb_t *get_mlq_proc(void)
{
	struct pcb_t *proc = NULL;
/*TODO: get a process from PRIORITY [ready_queue].
 * Remember to use lock to protect the queue.
 * */
#ifdef SYNCH
	pthread_mutex_lock(&queue_lock);
#endif

	int prio = 0;
	int loop = 0;
	while (proc == NULL)
	{
		if (!empty(&mlq_ready_queue[prio]) && slot[prio] < (MAX_PRIO - prio))
		{
			proc = dequeue(&mlq_ready_queue[prio]);
			slot[prio]++;
		}
		else
		{
			proc = NULL;
			prio++;
			if (prio == MAX_PRIO)
			{
				prio = 0;
				for (int i = 0; i < MAX_PRIO; i++)
				{
					slot[i] = 0;
				}
				if (loop == 1)
				{
#ifdef SYNCH
					pthread_mutex_unlock(&queue_lock);
#endif

					return NULL;
				}
				loop = 1;
			}
		}
	}
#ifdef SYNCH
	pthread_mutex_unlock(&queue_lock);
#endif

	return proc;
}

void put_mlq_proc(struct pcb_t *proc)
{
#ifdef SYNCH
	pthread_mutex_lock(&queue_lock);
#endif

	enqueue(&mlq_ready_queue[proc->prio], proc);
#ifdef SYNCH
	pthread_mutex_unlock(&queue_lock);
#endif
}

void add_mlq_proc(struct pcb_t *proc)
{
#ifdef SYNCH
	pthread_mutex_lock(&queue_lock);
#endif

	enqueue(&mlq_ready_queue[proc->prio], proc);
#ifdef SYNCH
	pthread_mutex_unlock(&queue_lock);
#endif
}

struct pcb_t *get_proc(void)
{
	return get_mlq_proc();
}

void put_proc(struct pcb_t *proc)
{
	return put_mlq_proc(proc);
}

void add_proc(struct pcb_t *proc)
{
	return add_mlq_proc(proc);
}
#else
struct pcb_t *get_proc(void)
{
	struct pcb_t *proc = NULL;
/*TODO: get a process from [ready_queue].
 * Remember to use lock to protect the queue.
 * */
#ifdef SYNCH
	pthread_mutex_lock(&queue_lock);
#endif

	proc = dequeue(&ready_queue);
#ifdef SYNCH
	pthread_mutex_unlock(&queue_lock);
#endif

	return proc;
}

void put_proc(struct pcb_t *proc)
{
#ifdef SYNCH
	pthread_mutex_lock(&queue_lock);
#endif

	enqueue(&run_queue, proc);
#ifdef SYNCH
	pthread_mutex_unlock(&queue_lock);
#endif
}

void add_proc(struct pcb_t *proc)
{
#ifdef SYNCH
	pthread_mutex_lock(&queue_lock);
#endif

	enqueue(&ready_queue, proc);
#ifdef SYNCH
	pthread_mutex_unlock(&queue_lock);
#endif
}
#endif
