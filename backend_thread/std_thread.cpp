#include <thread>
#include <chrono>
#include <mutex>

class Engine
{
public:
    Engine();
    void process(int frame_id);
    ~Engine();
private:
    void foo();
private:
    std::thread foo_thread;

    bool foo_stop = false;

    std::mutex foo_mutex;

    std::condition_variable foo_cv;
    bool foo_data_ready = false;
};

Engine::Engine()
{
    foo_thread = std::thread(&Engine::foo, this);
}

Engine::~Engine()
{
    {
        std::lock_guard<std::mutex> lock(foo_mutex);
        foo_stop = true;
        foo_cv.notify_one();
    }

    foo_thread.join();
}

void Engine::process(int frame_id)
{
    printf("--- processing frame %d\n", frame_id);
    {
        std::lock_guard<std::mutex> lock(foo_mutex);
        foo_data_ready = true;
        foo_cv.notify_one();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate some work
}

void Engine::foo()
{
    //printf("hello from thread foo\n");
    int cnt = 0;
    while (true)
    {
        std::unique_lock<std::mutex> lock(foo_mutex);
        foo_cv.wait(lock, [this] { 
            // printf("wait ...\n");
            return foo_data_ready || foo_stop;
        });

        if (foo_stop && !foo_data_ready)
            break;

        if (foo_data_ready)
        {
            cnt++;
            printf("foo thread process %d begin\n", cnt);
            // Process the data
            std::this_thread::sleep_for(std::chrono::milliseconds(150)); // simulate some work
            foo_data_ready = false;
            printf("foo thread process %d end\n", cnt);
        }
    }
}

int main()
{
    Engine engine;

    const int num_frames = 10;
    
    for (int i=0; i<num_frames; i++)
    {
        engine.process(i);
    }

    return 0;
}