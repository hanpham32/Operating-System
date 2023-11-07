// Pham Han
// schedule_sjf.c
// Shortest Job First Scheduling Algorithm

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

// Declare global variables for the head, current, new nodes
struct node *head = NULL;    // Head of the list
struct node *current = NULL; // Pointer to the current node in the list
struct node *new = NULL;     // Pointer to the new node to be added

// Add a task to the list with the given name, priority, and burst time
void add(char *name, int priority, int burst)
{
    // If head is null, initialize the head node
    if (head == NULL)
    {
        head = malloc(sizeof(struct node));

        // Set the task properties
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->priority = priority;
        head->task->burst = burst;

        // Set the next node to be null
        head->next = NULL;

        current = head;
    }
    else
    {
        // If head node exists, create a new node and set its task properties
        new = malloc(sizeof(struct node));

        new->task = malloc(sizeof(struct task));
        new->task->name = name;
        new->task->priority = priority;
        new->task->burst = burst;

        // Handle cases based on the burst times of the current and new tasks
        if (!(current->next))
        {
            if (((new->task->burst) > (current->task->burst)) || ((new->task->burst) == (current->task->burst)))
            {
                current->next = new; // head points to second node
                new->next = NULL;
            }
            else
            {
                // Handle the case when the second node burst is smaller than the current burst
                new->next = current;
                head = new;
                current = new;
            }
        }
        else
        {
            // Handle the cases for the third and subsequent nodes
            while (1)
            {
                if ((new->task->burst < current->next->task->burst))
                {
                    if (new->task->burst > current->task->burst)
                    {
                        new->next = current->next;
                        current->next = new;
                        current = head;
                        break;
                    }
                    else if (new->task->burst < current->task->burst)
                    {
                        head = new;
                        new->next = current;
                        current = head;
                        break;
                    }
                }
                else if (new->task->burst == current->next->task->burst)
                {
                    current = current->next;

                    if (current->next == NULL)
                    {
                        new->next = NULL;
                    }
                    else if (new->task->burst == current->next->task->burst)
                    {
                        current = current->next;
                        new->next = current->next;
                    }
                    else
                    {
                        new->next = current->next;
                    }

                    current->next = new;

                    current = head;
                    break;
                }
                else if ((new->task->burst) > (current->next->task->burst))
                {
                    current = current->next;
                    if (current->next == NULL)
                    {
                        current->next = new;
                        new->next = NULL;
                        current = head;
                        break;
                    }
                }
            }
        }
    }
}

// Invoke the scheduler
void schedule()
{
    int time = 0;
    struct node *ref = head;

    while (ref != NULL)
    {
        // Run the task and update the time
        run(ref->task, ref->task->burst);
        time = time + ref->task->burst;
        printf("Time is now: %d\n", time);
        ref = ref->next;
    }
}
