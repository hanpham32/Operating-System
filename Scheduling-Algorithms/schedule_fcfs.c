// Pham Han
// schedule_fcfs.c
// First Come First Serve Scheduling Algorithm

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

// Declare and initialize global variables for the head, last, new_node nodes
struct node *head_list = NULL; // Head of the list
struct node *last_list = NULL; // Pointer to the last node in the list
struct node *new_node = NULL;  // Pointer to the new node to be added

// Function to add a task to the linked list
void add(char *name, int priority, int burst)
{
    // If the list is empty, create the first node
    if (head_list == NULL)
    {
        // Allocate memory for head_list and last_list nodes
        head_list = malloc(sizeof(struct node));
        last_list = malloc(sizeof(struct node));

        // Set the task properties for the head_list node
        head_list->task = malloc(sizeof(struct task));
        head_list->task->name = name;
        head_list->task->priority = priority;
        head_list->task->burst = burst;

        // Set the next node to be null
        head_list->next = NULL;
        last_list = head_list;
    }
    // If the list is not empty, add the task to the end of the list
    else
    {
        // Allocate memory for the new_node node
        new_node = malloc(sizeof(struct node));

        // Link the new_node node to the last_list node in the list
        last_list->next = new_node;

        // Set the task properties for the new_node node
        new_node->task = malloc(sizeof(struct task));
        new_node->task->name = name;
        new_node->task->burst = burst;
        new_node->task->priority = priority;

        // Set the next node to be null
        new_node->next = NULL;

        // Update the last_list node to be the new_node node
        last_list = new_node;
    }
}

// Function to invoke the scheduler and execute the tasks in the order they were added (FCFS)
void schedule()
{
    // setup time
    int time = 0;

    // Use a reference node to traverse the list
    struct node *current = head_list;

    // Iterate through the list, executing each task and updating the time counter
    while (current != NULL)
    {
        // Run the task using the burst time
        run(current->task, current->task->burst);

        // Update the time counter
        time = time + current->task->burst;

        // Print the current time
        printf("Time is now: %d\n", time);

        // Move to the next task in the list
        current = current->next;
    }
}
