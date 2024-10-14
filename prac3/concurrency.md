# Concurrency

Concurrency is the ability to execute multiple tasks/ processes simultaneously.

## Key aspects:

**Processes**: A process is a program in execution, with it's own memory space. When OS switches between process rapidly, it is called context switching

**Threads**: Threads are smaller units of a process that share the same memory space. Concurrency with threads is acheived by having multiple threads within 1 process run simultaneously.

**Multitasking**: OS can run multiple processes at the same time, using Cooperative or Preemptive multitasking.

**Parrallelism**: Requires true simultaneous task execution, can only be achieved on multi-core or multi-processor systems.

**Synchronisation**: Mechanisms used to control shared access:

- Locks/Mutexes: Prevent multiple threads from accessing a resource
- Semaphores: Used for signaling between processes/threads
- Monitors: High level constructs that manage locks and condition variables

**Challenges**

- Race conditions: Multiple processes/threads access shared resource without syncrhonisation
- Deadlock: Two+ processes are waiting for eachother to move
- Starvation: One process is denied access because others are perpetually prioritised
