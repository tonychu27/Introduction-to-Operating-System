#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <cstring>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#define UNDISPATCHED 0
#define DISPATCHED 1
#define DONE 2
using namespace std;

struct Job {
    int begin;
    int end;
    int id;
} jobs[7];

int length;
int jobS[15];  
vector<int> arr, worker; 
list<Job> jobList;
pthread_t threads[8];

/* sem[0]: Arrival of new job, sem[1]: Completion of a job, sem[2]: Job List, sem[3]: Job State */
sem_t sem[4];

/*******************************************************************************************

 ------------------------------------------------------------------------------
 |                                 id = 0                                     |
 ------------------------------------------------------------------------------
 |              id = 1                |                id = 2                 |
 ------------------------------------------------------------------------------
 |     id = 3      |     id = 4       |      id = 5       |      id = 6       |
 ------------------------------------------------------------------------------
 | id = 7 | id = 8 | id = 9 | id = 10 | id = 11 | id = 12 | id = 13 | id = 14 |
 ------------------------------------------------------------------------------

********************************************************************************************/


void bubbleSort(int begin, int end) {

    for (int i = 0; i < end - begin; i++) 
        for (int j = begin; j < end; j++) 
            if (worker[j] > worker[j + 1])
                swap(worker[j], worker[j + 1]);
}

void merge(const int left, const int right, const int mid) {

    vector<int> l(worker.begin() + left, worker.begin() + mid + 1);
    vector<int> r(worker.begin() + mid + 1, worker.begin() + right + 1);
    int left_index = 0, right_index = 0, index = left;

    while(left_index < l.size() && right_index < r.size()) {
        if(l[left_index] <= r[right_index])
            worker[index++] = l[left_index++];
        else
            worker[index++] = r[right_index++];
    }

    while(left_index < l.size())
        worker[index++] = l[left_index++];
    while(right_index < r.size())
        worker[index++] = r[right_index++];

}

void* working(void* args) {

    for(; ;) {
        sem_wait(&sem[0]);
        
        sem_wait(&sem[2]);
        Job job = jobList.front();
        jobList.pop_front();
        sem_post(&sem[2]);

        if (job.begin < job.end) {
            if(job.id >= 7) 
                bubbleSort(job.begin, job.end);
            else 
                merge(job.begin, job.end, (job.begin + job.end) / 2);
        }
    
        sem_wait(&sem[3]);
        jobS[job.id] = DONE;
        sem_post(&sem[3]);
        
        sem_post(&sem[1]);
    }
}

void jobAllocate(Job temp) {
    
    sem_wait(&sem[2]);
    jobList.push_back(temp);
    sem_post(&sem[2]);

    sem_post(&sem[0]);
}

void merge_sort(int begin, int end, int id) {
  
    if (begin < end) {
        
        Job temp;
        temp.begin = begin;
        temp.end = end;
        temp.id = id;

        if (id >= 7 ) { 
            // cout << begin << " " << end << endl ;
            jobAllocate(temp);
            return;
        }
    
        int mid = (begin + end) / 2;
        merge_sort(begin, mid, id * 2 + 1);
        merge_sort(mid + 1, end, id * 2 + 2);

        jobs[id] = temp;
    
    }
}

void checkJobs() {

    for (; ;) {

        sem_wait(&sem[1]);
        sem_wait(&sem[3]);

        if (jobS[0] == 2) {
            sem_post(&sem[3]);
            break;
        }

        for (int i = 0; i < 7; i++) {
            if (jobS[i] == UNDISPATCHED && jobS[i * 2 + 1] == DONE && jobS[i * 2 + 2] == DONE) {
                
                jobS[i] = DISPATCHED;

                sem_wait(&sem[2]);
                jobList.push_back(jobs[i]);
                sem_post(&sem[2]);
                
                sem_post(&sem[0]);
            }
        }

        sem_post(&sem[3]);
    }

}

void readFile() {

    ifstream file;
    file.open("input.txt");

    file >> length;
    worker.resize(length);

    while(file) {
        int num;
        file >> num;
        arr.push_back(num);
    }

    file.close();

}

void writeFile(int n) {

    string name = "output_" + to_string(n + 1) + ".txt";
    ofstream file(name);

    for (int i=0; i < length; i++)
        file << worker[i] << ' ';

    file.close();
}

int main() {

    timeval start, end;
    readFile();

    sem_init(&sem[0], 0, 0);
    sem_init(&sem[1], 0, 0);
    sem_init(&sem[2], 0, 1);
    sem_init(&sem[3], 0, 1);

    for (int i = 0; i < 8; i++) {

        cout << "worker thread #"<< i + 1 << ", elapsed ";

        memset(jobS, UNDISPATCHED, sizeof(jobS));

        for (int k = 0; k < length; k++)
            worker[k] = arr[k];

        gettimeofday(&start, 0);

        pthread_create(&threads[i], NULL, working, NULL);

        merge_sort(0, length - 1, 0);

        checkJobs();

        gettimeofday(&end, 0);

        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;

        cout << 1000 * (sec + (usec/1000000.0)) << " ms" << endl;

        writeFile(i);
    }

    return 0;
}