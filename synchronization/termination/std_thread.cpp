#include <thread>
#include <mutex>
#include <stdio.h>

bool running = false;
std::mutex mutex;

void f()
{
    printf("hello from thread\n");
    int cnt = 0;
    while (true)
    {
        cnt++;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!running)
                break;
            printf("thread is running, cnt = %d\n", cnt);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate some work
    }
}

int main()
{
    printf("hello from main\n");
    running = true;

    std::thread new_thread(f);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    {
        std::lock_guard<std::mutex> lock(mutex);
        running = false;
    }

    new_thread.join();
    printf("bye from main\n");
}
