# Operating-System
CSS 430 B Au 23: Operating Systems

# Projects:
- Unix-Shell
- Server Message Passing
- Scheduling Algorithms
- Multi-Threaded Server
- User-Thread-Context Switching
- Synchronization
- File System

## Multi-Threaded Server
This project focuses on the implementation of a simple multi-threaded server using Java sockets. The server handles client requests to return the current date in a multi-threaded manner. The assignment includes two distinct versions of the server:

DateServerMT: Where each client request is handled by explicitly creating and managing a new thread.
DateServerMTP: Utilizes a Java thread pool (from java.util.concurrent.Executors) to manage and run client requests efficiently.

## User-Thread-Context Switching
This project involves the creation of a user thread library and a round-robin scheduler, named sthread, distinct from pthread. It emphasizes capturing and restoring the execution environment using setjmp() and longjmp(). The project includes managing CPU register sets and function stacks for context switching between user threads, employing signal() and alarm() for time quantum management, and handling thread control blocks (TCB) for multiple user threads. The assignment focuses on non-preemptive scheduling and voluntary CPU yielding by threads.

## Scheduling Algorithms
Implementation of classic scheduling algorithms:
- First-Come-First-Served (FCFS)
- Shortest-Job-First (SJF)
- Priority Scheduling
- Round Robin (RR)

Analyzed and compared the performance of these algorithms with metrics like average turnaround time, waiting time, and CPU utilization. The project was provided with a simulation environment to manage process queues and allocate CPU utilization to simulate various workloads.

## Synchronization: The Sleeping Barbers Problem
The project is based off the classic barber problem, but extends on involving multiple barbers, which now has a waiting room with 'n' chairs and 'm' barbers. This project involves developing a multi-threaded synchronization solution to manage the barbers and customers threads, ensuring correct sequencing and prevent race conditions and deadlocks. The program effectively handles customer service in a synchronized manner, demonstrating understanding and application of threading and synchronization concepts in a real-world scenario.

## File Systems
The project involved implementing a Unix-like file system, termed BFS (Bothell File System), which included handling file operations and block I/O. The assignment's goal is to understand the BFS's structure, which consists of the three layers: user-level filesystem, internal BFS functions, and low-level block I/O functions, and metablocks, which responsible for trackling files and free blocks, and implement filesystem (user-level) functions like `fsRead` and `fsWrite`, and supporting mechanisms like block allocation, file creation, and file size management.
