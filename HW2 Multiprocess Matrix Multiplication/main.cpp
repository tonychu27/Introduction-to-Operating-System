#include <iostream>
#include <cstdint>
#include <cstdlib> 
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))
using namespace std;

uint32_t n;

/* A function to generate matrices */
void generateMatrix(uint32_t* A, uint32_t* B) {
    
    uint32_t val = 0;
    for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++) {
            A[i*n + j] = val;
            B[i*n + j] = val;
            val++;
        }
    }

}

/* A function to calculate matrices multiplication */
void calculate(uint32_t p, uint32_t id, uint32_t* C, uint32_t* A, uint32_t* B) {
    
    int num = n / p + (((n%p)) ? 1 : 0);
    int b = num * id;
    int t = MIN (num * (id + 1), n);

    for(uint32_t i = b; i < t; i++){
        for(uint32_t j = 0; j < n; j++){
            C[i*n + j] = 0;
            for(int k = 0; k < n; k++){
               C[i*n + j] += A[i*n + k] * B[k*n + j];
            }
        }
    }

}

/* A function to calculate checksum */
uint32_t checksum(uint32_t* A) {
    
    uint32_t ans = 0;
    for (uint32_t i = 0; i < n; i++)
        for (uint32_t j = 0; j < n; j++)
            ans += A[i*n + j];

    return ans;
}

int main() {

    cout << "Input the matrix dimension: ";
    cin >> n;

    /* Matrix generation and shared memory allocation */
    uint32_t* A, *B, *C;
    A = new uint32_t [n*n];
    B = new uint32_t [n*n];
    int shmidC = shmget(0, n*n*sizeof(uint32_t), IPC_CREAT|0600);
    generateMatrix(A, B);
    
    /* Fork multiple processes, spilt matrices and do matrix multipulication */
    for (uint32_t p = 1; p <= 16; p++) {

        pid_t pid;

        timeval start, end;
        gettimeofday(&start, 0);

        uint32_t id;
        for (id = 0; id < p; id++) {
            pid = fork();
            if (!pid)
                break;
        }

        /* Child Processes */
        if (pid == 0) {
            C = (uint32_t*)shmat(shmidC, NULL, 0);
            calculate(p, id, C, A, B);
            shmdt(C);
            exit(0);
        }
        /* Parent Process */
        else {
            C = (uint32_t*)shmat(shmidC, NULL, 0);
            for (id = 0; id < p; id++)
                wait(NULL);
            gettimeofday(&end, 0);
            int sec = end.tv_sec - start.tv_sec;
            int usec = end.tv_usec - start.tv_usec;

            if (p==1) 
                cout << "Multiplying matrices using 1 process\n";
            else 
                cout << "Multiplying matrices using " << p << " processes\n";
            cout << "elapsed " << sec + (usec/1000000.0) << " sec, Checksum: " << checksum(C) << endl;

            shmdt(C);
        }

    }

    delete []A;
    delete []B;
    shmctl(shmidC, IPC_RMID, NULL);

    return 0;
}