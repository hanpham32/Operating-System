// Include necessary header files
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Include custom header files for task, list, schedulers, and cpu
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

// Declare and initialize global variables for the head, last, new nodes, and task counter
struct node *head = NULL;
struct node *last = NULL;
struct node *new = NULL;
int num = 0;

// Function to add a task to the linked list
void add(char *name, int priority, int burst)
{
    // If the list is empty, create the first node
    if (head == NULL)
    {
        // Allocate memory for head and last nodes
        head = malloc(sizeof(struct node));
        last = malloc(sizeof(struct node));

        // Set the task properties for the head node
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->burst = burst;
        head->task->priority = priority;
        // Set the next node to be null
        head->next = NULL;
        last = head;
    }
    // If the list is not empty, add the task to the end of the list
    else
    {
        // Allocate memory for the new node
        new = malloc(sizeof(struct node));
        // Link the new node to the last node in the list
        last->next = new;
        // Set the task properties for the new node
        new->task = malloc(sizeof(struct task));
        new->task->name = name;
        new->task->burst = burst;
        new->task->priority = priority;
        // Set the next node to be null
        new->next = NULL;
        // Update the last node to be the new node
        last = new;
    }
}

// Function to invoke the scheduler and execute the tasks in the order they were added (FCFS)
void schedule()
{
    // setup time
    int time = 0;

    // Use a reference node to traverse the list
    struct node *ref = head;
    // Iterate through the list, executing each task and updating the time counter
    while (ref != NULL)
    {
        // Increment the task counter
        num = num + 1;
        // Run the task using the burst time
        run(ref->task, ref->task->burst);

        // Update the time counter
        time = time + ref->task->burst;
        // Print the current time
        printf("Time is now: %d\n", time);
        // Move to the next task in the list
        ref = ref->next;
    }
}
