#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"
// Global variables for the task list and counters
struct node *head = NULL;
struct node *last = NULL;
struct node *new = NULL;
struct node *current = NULL;

// Function to add a task to the list
void add(char *name, int priority, int burst)
{

    if (head == NULL)
    {
        // If the list is empty, create the first node
        head = malloc(sizeof(struct node));
        // Set the task data
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->burst = burst;
        head->task->priority = priority;
        // set the next node to be null
        head->next = NULL;
        current = head;
    }
    else
    {
        // If the list is not empty, add a new node at the end
        new = malloc(sizeof(struct node));
        new->task = malloc(sizeof(struct task));
        new->task->burst = burst;
        new->task->name = name;
        new->task->priority = priority;
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
    int time = 0;
    current = head;
    struct node *ref = head;
    struct node *beginning = malloc(sizeof(struct node));
    int newburst = 0;
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
