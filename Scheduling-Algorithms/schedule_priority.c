// Include necessary libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

// Declare global pointers for nodes in the linked list
struct node *head = NULL;
struct node *current = NULL;
struct node *new = NULL;
struct node *final = NULL;

// Function to add tasks to the linked list
void add(char *name, int priority, int burst)
{
    // If head is NULL, initialize the first node
    if (head == NULL)
    {
        head = malloc(sizeof(struct node));
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->burst = burst;
        head->task->priority = priority;
        head->next = NULL;

        current = head;
    }
    else
    {
        // Allocate memory for a new node and set its task values
        new = malloc(sizeof(struct node));
        new->task = malloc(sizeof(struct task));
        new->task->burst = burst;
        new->task->name = name;
        new->task->priority = priority;

        // Insert the new node based on priority
        if (!(current->next))
        {
            // If the list has only one node, insert the new node before or after the current node based on priority
            if (((new->task->priority) < (current->task->priority)) || ((new->task->priority) == (current->task->priority)))
            {
                current->next = new;
                new->next = NULL;
            }
            else
            {
                new->next = current;
                head = new;
                current = new;
            }
        }
        else
        {
            // Traverse the list and insert the new node at the appropriate position based on priority
            while (1)
            {
                if ((new->task->priority > current->next->task->priority))
                {
                    // Insert the new node before the current node based on priority
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

// Function to run the scheduler
void schedule()
{
    int num = 0;
    int burst = 0;

    struct node *ref = head;
    // Traverse the list and run each task, keeping track of the total burst time
    while (ref != NULL)
    {
        num = num + 1;
        run(ref->task, ref->task->burst);
        burst = burst + ref->task->burst;
        printf("Time is now: %d\n", burst);
        ref = ref->next;
    }
}
