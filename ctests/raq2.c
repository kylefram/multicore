#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

struct targs {
    int *x;            
    atomic_int *y;      // syncer
};

void* writer(void *arg) {
    struct targs *args = (struct targs *)arg;
    
    *(args->x) = 1; // write 
    
    // release
    atomic_store_explicit(args->y, 1, memory_order_release);
    return NULL;
}

void* reader(void *arg) {
    struct targs *args = (struct targs *)arg;

    // acquire 
    while (atomic_load_explicit(args->y, memory_order_acquire) == 0) {}
    
    // read
    printf("read x: %d\n", *(args->x));
    return NULL;
}

int main(void) {   
    pthread_t t1, t2;
    int x = 0;
    atomic_int y = 0;
    
    struct targs args = {&x, &y};
    
    pthread_create(&t1, NULL, writer, &args);
    pthread_create(&t2, NULL, reader, &args);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}