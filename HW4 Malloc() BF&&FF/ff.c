#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

struct block {
	size_t size;
	int free;
	struct block *prev;
	struct block *next;
};

struct block* begin = NULL;
struct block* head = NULL;

void init() {
    begin = mmap(0, 20000, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    head = begin;
    head->size = 20000 - 32;
    head->free = TRUE;
    head->prev = NULL;
    head->next = NULL;
}

void getMaximum() {
    struct block* current = head;
    size_t max = 0;
    while(current) {
        if(current->size > max && current->free)
            max = current->size;
    
        current = current->next;
    }

    const char *ans = "Max Free Chunk Size = ";
    char num[15];
    sprintf(num, "%ld", max);
    write(1, ans, strlen(ans));
    write(1, num, strlen(num));
    write(1, "\n", 1);

    munmap(head, 20000);
}

struct block* findPos(size_t size) {
    struct block* current = head;
    struct block* temp = NULL;

    while(current) {
        if(current->free && current->size >= size) {
            temp = current;
            break;
        } 
        current = current->next;
    }

    return temp;
}

struct block* insert(size_t size) {
    
    struct block* temp = findPos(size);
    struct block* current = temp;

    /*** If it perfectly fits it ***/
    if(current->size == size)
        current->free = FALSE;
    else {
        temp = current + 1 + size / 32;
        temp->size = current->size - 32 - size;
        temp->prev = current;
        temp->free = TRUE;
        temp->next = current->next;
        if(current->next)
            current->next->prev = temp;
        current->next = temp;
        current->free = FALSE;
        current->size = size;
    }

    return current + 1;
}

void *malloc(size_t size) {
    if(size) {
        if (size % 32)
            size = (size / 32 + 1) * 32;
        if(!begin)
            init();

        struct block* tmp = insert(size);
        // write(1, "Bonjour!\n", 9);
        return (void*)tmp;
    }
    else if(!size)
        getMaximum();

}

void free(void *ptr) {
    struct block *current;
    current = ptr;
    current -= 1;
    current->free = TRUE;
    if(current->next && current->next->free) {
        current->size += 32 + current->next->size;
        if(current->next->next)
            current->next->next->prev = current;
        current->next = current->next->next;
    }
    if(current->prev && current->prev->free) {
        current->prev->size += 32 + current->size;
        if(current->next)
            current->next->prev = current->prev;
        current->prev->next = current->next;
    }
}

/*
int main() {
    printf("%lu\n", sizeof(struct struct block));

    return 0;
}
*/