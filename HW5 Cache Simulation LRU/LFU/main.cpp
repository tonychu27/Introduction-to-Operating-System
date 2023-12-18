#include <iostream>
#include <vector>
#include <utility>
#include <sys/time.h>
#include <algorithm>
#define MAXN 1200
#define hash(x) ((x) % (MAXN))
using namespace std;
typedef pair<int, int> pii;

struct LFU {
    int feq, pos, seq;
};

struct LRU {
    int pos;
    LRU* next;
    LRU* prev;
};

typedef pair<int, LRU*> pir;

void doLFU(string fileName) {
    printf("LFU policy:\n");
    printf("Frame\tHit\t\tMiss\t\tPage fault ratio\n");

    for(int frame = 64; frame <= 512; frame <<= 1) {
        vector<pii> hashing[MAXN];
        LFU* node = new LFU [frame];
        FILE* trace = fopen(fileName.c_str(), "r");
        int hit = 0, miss = 0, free = 0, seq = 0;
        int addr;

        while(fscanf(trace, "%d", &addr) == 1 && ++seq) {
            bool flag = 0;
            
            //puts("Bonjour");
            /*** Check if page is already in frame ***/
            for(pii& p: hashing[hash(addr)]) {
                if(p.first == addr) {
                    hit++;
                    flag = 1;
                    node[p.second].seq = seq;
                    node[p.second].feq++;
                    break;
                }
            }

            if(flag)
                continue;
        
            /*** The page is missed ***/
            miss++;

            /*** If there still has space to place page ***/
            if(free < frame) {
                hashing[hash(addr)].push_back(make_pair(addr, free));
                node[free].feq = 1;
                node[free].pos = addr;
                node[free].seq = seq;
                free++;

                continue;
            }

            /*** Have to remove page in the frame ***/
            int min = 0;
            for (int i = 1; i < frame; i++) {
                if(node[min].feq > node[i].feq || ((node[min].feq == node[i].feq) && node[min].seq > node[i].seq))
                    min = i;
            }

            hashing[hash(node[min].pos)].erase(remove(hashing[hash(node[min].pos)].begin(), hashing[hash(node[min].pos)].end(), make_pair(node[min].pos, min)));
            hashing[hash(addr)].push_back(make_pair(addr, min));
            
            node[min].feq = 1;
            node[min].pos = addr;
            node[min].seq = seq; 
        }

        delete []node;
        fclose(trace);
        printf("%d\t%d\t\t%d\t\t%.10f\n", frame, hit, miss, (double)miss / (miss + hit));
    }
}

void doLRU(string fileName) {
    printf("LRU policy:\n");
    printf("Frame\tHit\t\tMiss\t\tPage fault ratio\n");

    for(int frame = 64; frame <= 512; frame <<= 1) {
        vector<pir> hashing[MAXN];
        LRU *head = NULL, *back = NULL, *current = NULL;
        FILE* trace = fopen(fileName.c_str(), "r");
        int hit = 0, miss = 0, free = 0;
        int addr;

        while(fscanf(trace, "%d", &addr) == 1) {
            current = NULL;
            /*** Check if page is already in frame ***/
            for(pir& p: hashing[hash(addr)])
                if(p.first == addr) {
                    current = p.second;
                    hit++;
                }

            if(current) {
                /*** If the hitted page is at the head of the linked list ***/
                if (head == current)
                    continue;

                /*** If the hitted page is at the end of the linked list ***/
                if(back == current) {
                    back = back->prev;
                    back->next = NULL;
                }
                /*** If the hitted page is at the middle of the linked list ***/
                else
                    current->next->prev = current->prev;
                    
                current->prev->next = current->next;
                current->next = head;
                current->prev = NULL;
                head->prev = current;
                head = current;

                continue;
            }

            miss++;

            while(free >= frame) {
                hashing[hash(back->pos)].erase(remove(hashing[hash(back->pos)].begin(), hashing[hash(back->pos)].end(), make_pair(back->pos, back)));
                back = back->prev;
                back->next = NULL;
                free--;
            }

            current = new LRU;
            current->pos = addr;
            current->prev = NULL;
            current->next = head;
            if(head)
                head->prev = current;
            head = current;
            if(!free)
                back = head;
            free++;
            hashing[hash(addr)].push_back(make_pair(addr, current));
        }

        fclose(trace);
        printf("%d\t%d\t\t%d\t\t%.10f\n", frame, hit, miss, (double)miss / (miss + hit));
    }
}

int main(int argc, char * argv[]) {

    if(argc < 2) {
        cout << "No file given\n";
        exit(-1);
    }

    string fileName = string(argv[1]);

    timeval startLFU, endLFU;
    timeval startLRU, endLRU;

    gettimeofday(&startLFU, NULL);
    doLFU(fileName);
    gettimeofday(&endLFU, NULL);

    double elapseLFU = (endLFU.tv_sec - startLFU.tv_sec) + (endLFU.tv_usec - startLFU.tv_usec) / 1e6;
    printf("Total elapsed time %.4f sec\n\n", elapseLFU);

    gettimeofday(&startLRU, NULL);
    doLRU(fileName);
    gettimeofday(&endLRU, NULL);

    double elapseLRU = (endLRU.tv_sec - startLRU.tv_sec) + (endLRU.tv_usec - startLRU.tv_usec) / 1e6;
    printf("Total elapsed time %.4f sec\n", elapseLRU);    


    return 0;
}