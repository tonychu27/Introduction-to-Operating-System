# HW2- Multi-Process Matrix Multiplication using Shared Memory

## Overview
Matrix multiplication using multiple processes 
Basic parallel processing 
Will be faster on multicore machines \
Input: the dimension of two square matrices A & B 
> E.g., 100 -> A, B, and C are 100x100 square matrices 
Output: an execution time and a checksum

## Shared Memory
Matrices A, B, and C are stored in a shared memory. \
No locking/synchronization is required since multiplication on sub-metrices are mutually independent

## Header Files
```c
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
```
```c
int shmget(key_t key, size_t size, int shmflag); /*** Areate a block of shared memory ***/
void *shmat(int shmid, const void *shmaddr, int shmflg); /*** Attach shared memory to the current process’s address space ***/
int shmdt(const void *shmaddr); /*** Detach shared memory from the current process’s address space ***/
int shmctl(int shmid, int cmd, struct shmid_ds *buf); /*** Control shared memory ***/
int gettimeofday(struct timeval *restrict tv, struct timezone *_Nullable restrict tz); /*** Calculate the elapsed time ***/
```

## Matrix Initial Values
Matrix elements in A and B are initialized as follows (for example, an 8x8 matrix)
```
0   1   2   3   4   5   6   7
8   9   10  11  12  13  14  15
16  17  18  19  20  21  22  23
24  25  26  27  28  29  30  31
32  33  34  35  36  37  38  39
40  41  42  43  44  45  46  47
48  49  50  51  52  53  54  55
56  57  58  59  60  61  62  63
```

## Requirements
Your program using any matrix dimension between 100x100 and 800x800 \

Test your program on a multicore machine \

Create worker processes using fork()
• Wait until all worker processes have exit()’ed \

About matrices A, B, and C
• A and B can be inside or outside of the shared memory \
• A and B can be the same or separate matrices \
• C `must` be in the shared memory \

Print the elapsed time and the matrix checksum (Add all the elemets in C)
• 16 cases, degree of process parallelism increases from 1 to 16 \
• The final checksum must be correct \
• Matrix elements and the checksum are all `32-bit unsigned integers` \
• Don’t worry, just let the checksum integer overflow 


## Output
Input the matrix dimension: 800

```
Multiplying matrices using 1 process 
Elapsed time: 1.814723 sec, Checksum: 561324032
Multiplying matrices using 2 processes
Elapsed time: 0.50231 sec, Checksum: 561324032
Multiplying matrices using 3 processes
Elapsed time: 0.327338 sec, Checksum: 561324032 
......
Multiplying matrices using 16 processes
Elapsed time: x.xxxxx sec, Checksum: 561324032
```
