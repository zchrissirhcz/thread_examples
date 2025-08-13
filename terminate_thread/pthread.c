#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

bool running = false;
pthread_mutex_t mutex;

void* f(void* arg)
{
    printf("hello from thread\n");
    int cnt = 0;

    while (true)
    {
        cnt++;
        pthread_mutex_lock(&mutex);
        if (!running)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        printf("thread is running, cnt = %d\n", cnt);
        pthread_mutex_unlock(&mutex);
        sleep(1); // simulate some work
    }

    printf("bye from thread\n");

    return NULL;
}

int main()
{
    printf("hello from main\n");

    pthread_mutex_init(&mutex, NULL);
    running = true;

    pthread_t new_thread;
    pthread_create(&new_thread, NULL, f, NULL);

    sleep(5);
    pthread_mutex_lock(&mutex);
    running = false;
    pthread_mutex_unlock(&mutex);

    pthread_join(new_thread, NULL);
    pthread_mutex_destroy(&mutex);

    printf("bye from main\n");
}

