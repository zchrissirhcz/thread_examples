#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
int data = 0;
bool data_ready = false;
bool running = false;

void f()
{
    printf("thread starting\n");
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return data_ready || !running; });
        // equivalent to:
        // while (!(data_ready || !running))
        // {
        //     cv.wait(lock);
        // }

        if (!running && !data_ready)
            break;

        if (data_ready)
        {
            data_ready = false;
            if (data % 2 == 0)
            {
                printf("in thread got even data: %d\n", data);
            }
            else
            {
                printf("in thread got odd data: %d\n", data);
            }
        }
    }
    printf("thread exiting\n");
}


int main()
{
    printf("hello from main\n");
    running = true;
    std::thread thread(f);

    int n;
    while (true)
    {
        if (std::cin >> n)
        {
            std::lock_guard<std::mutex> lock(mtx);
            data = n;
            data_ready = true;
            cv.notify_one();
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (n > 100)
        {
            break;
        }
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        running = false;
    }

    thread.join();
    printf("bye from main\n");

    return 0;
}