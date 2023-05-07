#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t *q)
{
        if (q == NULL)
                return 1;
        return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
        /* TODO: put a new process to queue [q] */
        if (q->size >= MAX_QUEUE_SIZE)
        {
                // printf("Error: Queue is full\n");
                return;
        }

        q->proc[q->size] = proc;
        q->size++;
}

struct pcb_t *dequeue(struct queue_t *q)
{
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */
        //
        if (q->size == 0)
        {
                printf("Error: Queue is empty\n");
                return NULL;
        }

        // Find the process with the highest priority in the queue
        struct pcb_t *highest = q->proc[0];
        int highest_index = 0;
        for (int i = 1; i < q->size; i++)
        {
                if (q->proc[i]->prio < highest->prio)
                {
                        highest = q->proc[i];
                        highest_index = i;
                }
        }

        // Remove the highest priority process from the queue
        for (int i = highest_index; i < q->size - 1; i++)
        {
                q->proc[i] = q->proc[i + 1];
        }
        q->size--;

        return highest;

}
