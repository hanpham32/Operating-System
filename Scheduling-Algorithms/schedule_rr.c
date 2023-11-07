// Pham Han
// schedule_rr.c
// Round Robin Scheduling Algorithm

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

// Global variables for the task list
struct node *head = NULL; // Head of the list
struct node *last = NULL; // Pointer to the last node in the list
struct node *new = NULL;  // Pointer to the new node to be added

// Function to add a task to the list
void add(char *name, int priority, int burst)
{
    if (head == NULL)
    {
        // If the list is empty, create the first node
        head = malloc(sizeof(struct node));
        last = malloc(sizeof(struct node));

        // Set the task data
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->burst = burst;
        head->task->priority = priority;
        // Set the next node to be null
        head->next = NULL;

        last = head;
    }
    else
    {
        // If the list is not empty, add a new node at the end
        new = malloc(sizeof(struct node));
        last->next = new;
        new->task = malloc(sizeof(struct task));
        new->task->name = name;
        new->task->burst = burst;
        new->task->priority = priority;
        new->next = NULL;
        last = new;
    }
}

// Function to invoke the scheduler
void schedule()
{
    struct node *current = head;
    struct node *ref = head;
    struct node *monitor = NULL;
    monitor = malloc(sizeof(struct node));
    monitor = head;
    int newburst = 0;
    int totburst = 0;

    // Iterate through the task list
    while (ref != NULL)
    {
        // If the task's burst time is greater or equal to 10
        if (ref->task->burst >= 10)
        {
            newburst = ref->task->burst - 10; // set newburst to 10

            // Check if the current task has been executed before
            if (current != head)
            {
                while (monitor != current)
                {
                    if (strcmp(current->task->name, monitor->task->name) == 0)
                    {
                        break;
                    }
                    monitor = monitor->next;
                }
                monitor = head;
            }

            // Update the total burst time and execute the task
            totburst = totburst + 10;
            ref->task->burst = 10;
            run(ref->task, 10);
            printf("Time is now: %d\n", totburst);
        }
        // If the task's burst time is less than 10
        else if (ref->task->burst < 10)
        {
            newburst = 0;
            totburst = totburst + ref->task->burst;
            run(ref->task, ref->task->burst);
            printf("Time is now: %d\n", totburst);
        }

        // Update the task list based on the new burst time
        while (1)
        {
            if (current->next != NULL)
            {
                current = current->next;
                if (current->next == NULL)
                {
                    // If there is a new burst time, create a new node for the task
                    if (newburst != 0)
                    {
                        struct node *newnode = malloc(sizeof(struct node));
                        newnode->task = malloc(sizeof(struct task));
                        newnode->task->name = ref->task->name;
                        newnode->task->burst = newburst;
                        newnode->task->priority = ref->task->priority;
                        ref = ref->next;
                        current->next = newnode;
                        newnode->next = NULL;
                        current = ref;
                        break;
                    }
                    // If there is no new burst time, move to the next task
                    else if (newburst == 0)
                    {
                        ref = ref->next;
                        current = ref;
                        break;
                    }
                }
            }
            else
            {
                // If this is the last element
                if (newburst != 0)
                {
                    struct node *new = malloc(sizeof(struct node));
                    new->task = malloc(sizeof(struct task));
                    new->task->name = ref->task->name;
                    new->task->priority = ref->task->priority;
                    new->task->burst = newburst;
                    ref->next = new;
                    ref = ref->next;
                    new->next = NULL;
                    current = new;
                }
                else
                {
                    ref = ref->next;
                }
                break;
            }
        }
    }
}
