#include <thread>
#include <stdio.h>
#include <chrono>

int main()
{
    std::thread new_thread([]() {
        printf("hello from thread\n");
        std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate some work
        printf("bye from thread\n");
    });

    printf("hello from main\n");
    new_thread.join();
    printf("bye from main\n");
}
