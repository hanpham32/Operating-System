// Pham Han
// schedule_priority_rr.c
// Priority Round Robin Scheduling Algorithm

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

// Global variables for the task list
struct node *head = NULL;    // Head of the list
struct node *last = NULL;    // Pointer to the last node in the list
struct node *new = NULL;     // Pointer to the new node to be added
struct node *current = NULL; // Pointer to the current node in the list

// Function to add a task to the list
void add(char *name, int priority, int burst)
{
    // If the list is empty
    if (head == NULL)
    {
        // Create the first node
        head = malloc(sizeof(struct node));

        // Set the task data
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->priority = priority;
        head->task->burst = burst;

        // set the next node to be null
        head->next = NULL;
        current = head;
    }
    else
    {
        // If the list is not empty, add a new node at the end
        new = malloc(sizeof(struct node));
        new->task = malloc(sizeof(struct task));
        new->task->name = name;
        new->task->priority = priority;
        new->task->burst = burst;

        // if current->next is NULL
        if (!(current->next))
        {
            if (((new->task->priority) < (current->task->priority)) || ((new->task->priority) == (current->task->priority)))
            {
                current->next = new;
                new->next = NULL;
            }
            // if the second node burst is smaller than the current burst
            else
            {
                // set new to point to head which is in the second position
                new->next = current;

                // head now holds the address of new which is in the first position
                head = new;

                // reset current to new
                current = new;
            }
        }
        // If current->next is not null
        else
        {
            // For nodes in the middle
            while (1)
            {
                // decide order of tasks based on their priority correlation
                if ((new->task->priority > current->next->task->priority))
                {
                    if (new->task->priority < current->task->priority)
                    {
                        new->next = current->next;
                        current->next = new;
                        current = head;
                        break;
                    }
                    else if (new->task->priority > current->task->priority)
                    {
                        head = new;
                        new->next = current;
                        current = head;
                        break;
                    }
                    else if (new->task->priority == current->task->priority)
                    {
                        new->next = current->next;
                        current->next = new;
                        break;
                    }
                }
                else if (new->task->priority == current->next->task->priority)
                {
                    // Handle the case where the new task has the same priority as the next task
                    current = current->next;
                    if (current->next == NULL)
                    {
                        new->next = NULL;
                    }
                    else if (new->task->priority == current->next->task->priority)
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
                else if ((new->task->priority) < (current->next->task->priority))
                {
                    // Continue searching for the right place to insert the new node
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
// Function to invoke the scheduler
void schedule()
{
    // Initialize time and reference pointers for scheduling
    int time = 0;
    current = head;
    struct node *ref = head;
    struct node *beginning = malloc(sizeof(struct node));
    int newburst = 0;

    // Iterate through the task list
    while (ref != NULL)
    {
        if (ref->next == NULL)
        {
            run(ref->task, ref->task->burst);
            time += ref->task->burst;
            printf("Time is now %d\n", time);
            break;
        }
        else
        {
            if (ref->task->priority != ref->next->task->priority)
            {

                run(ref->task, ref->task->burst);
                time += ref->task->burst;
                printf("Time is now %d\n", time);
                ref = ref->next;
            }
            else
            {
                if (ref->next != NULL)
                {
                    if (ref->task->priority == ref->next->task->priority)
                    {
                        current = ref;
                        while (current->next != NULL)
                        {
                            current = current->next;
                            // Inspect if current->next != NULL before priority comparison
                            if (current->next != NULL && current->task->priority != current->next->task->priority)
                            {
                                beginning = current->next;
                                current->next = NULL;
                                // Move back concurrent ref to do round-robin.
                                current = ref;
                                break;
                            }
                        }
                        while (ref != NULL)
                        {
                            if (ref->task->burst >= 10)
                            {
                                newburst = ref->task->burst - 10;
                                ref->task->burst = 10;
                                run(ref->task, 10);
                                time += 10;
                                printf("Time is now %d\n", time);
                            }
                            else if (ref->task->burst < 10)
                            {
                                newburst = 0;
                                run(ref->task, ref->task->burst);
                                time += ref->task->burst;
                                printf("Time is now %d\n", time);
                            }

                            while (1)
                            {
                                if (current->next != NULL)
                                {
                                    current = current->next;
                                    if (current->next == NULL)
                                    {
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
                                    // If this is the last element, set to null through this.
                                    ref = ref->next;
                                    current = beginning;
                                    break;
                                }
                            }
                        }
                        ref = beginning; // oh well
                    }
                }
            }
        }
    }
}
