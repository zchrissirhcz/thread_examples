#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;                  // mutex
std::condition_variable cv;      // condition variable
bool data_ready = false;         // shared condition
int shared_data;                 // shared data

// producer thread: generate data
void producer() {
    std::unique_lock<std::mutex> lock(mtx);
    
    // generate data
    for (int i = 0; i < 5; i++)
    {
        shared_data = i;
        std::cout << "producer: generated data " << shared_data << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // simulate work
        
        // update state
        data_ready = true;
        
        // notify consumer threads that are waiting
        cv.notify_one();
    }
}

// consumer thread: process data
void consumer() {
    std::unique_lock<std::mutex> lock(mtx);
    
    // 等待数据准备好，如果条件不满足则释放锁并等待
    // cv.wait(lock, []{ return data_ready; });
    while (!data_ready) {
        cv.wait(lock);
    }
    
    // process data
    std::cout << "consumer: receives data " << shared_data << std::endl;
}

int main() {
    // create producer and consumer threads
    std::thread prod(producer);
    std::thread cons(consumer);
    
    // wait for threads finished
    prod.join();
    cons.join();
    
    return 0;
}
