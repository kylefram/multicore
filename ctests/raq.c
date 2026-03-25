#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

struct targs{
    atomic_int *x;
    atomic_int *y;
};

void* writer(void *arg)
{
    struct targs *args = (struct targs *)arg;
    *(args->x) = 1;
    atomic_thread_fence(memory_order_release);
    *(args->y) = 1;

    return NULL;
}

void* reader(void *arg)
{
    struct targs *args = (struct targs *)arg;

    while (*(args->y) == 0);
    
    atomic_thread_fence(memory_order_acquire);

    return NULL;
}

int main(void)
{   
    pthread_t t1, t2;
    atomic_int x = 0;
    atomic_int y = 0;
    struct targs args;
    args.x = &x;
    args.y = &y;
    
    pthread_create(&t1, NULL,writer,&args);
    pthread_create(&t2, NULL,reader,&args);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("x: %d\n", x);
    printf("y: %d\n", y);

    pthread_detach(t1);
    pthread_detach(t2);
    return 0;

}