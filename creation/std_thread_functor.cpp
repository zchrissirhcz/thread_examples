#include <thread>
#include <chrono>

class Hello
{
public:
    void operator()()
    {
        printf("hello from thread\n");
        std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate for some work
        printf("bye from thread\n");
    }
};

int main()
{
    Hello hello;
    std::thread new_thread(hello);
 
    printf("hello from main\n");
    new_thread.join();
    printf("bye from main\n");

    return 0;
}