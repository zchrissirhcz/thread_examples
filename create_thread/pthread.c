#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* f(void* arg)
{
    printf("Hello from thread\n");
    sleep(1); // simulate some work
    printf("bye from thread\n");
    return NULL;
}

int main()
{
    printf("Hello from main\n");

    pthread_t new_thread;
    pthread_create(&new_thread, NULL, f, NULL);

    pthread_join(new_thread, NULL);
    
    printf("bye from main\n");
}
