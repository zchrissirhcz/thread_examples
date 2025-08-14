namespace spl {

struct adopt_lock_t {
    explicit adopt_lock_t() = default;
};

template<class Mutex>
class lock_guard
{
public:
    using mutex_type = Mutex;

    explicit lock_guard(mutex_type& mtx) : _mtx(mtx)
    {
        _mtx.lock();
    }

    lock_guard(Mutex& mtx, adopt_lock_t) noexcept : _mtx(mtx)
    {
    }

    ~lock_guard() noexcept
    {
        _mtx.unlock();
    }

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;

private:
    Mutex& _mtx;
};

} // namespace spl

#include <mutex>
#include <iostream>
#include <thread>

std::mutex mtx;
int shared_data = 0;

void increment_data()
{
    spl::lock_guard<std::mutex> lock(mtx);
    shared_data++;
    std::cout << "Thread " << std::this_thread::get_id() 
              << " incremented data to: " << shared_data << std::endl;
}

void increment_data_v2()
{
    mtx.lock();

    spl::lock_guard<std::mutex> lock(mtx, spl::adopt_lock_t{});

    shared_data++;
    std::cout << "Thread " << std::this_thread::get_id() 
              << " incremented data to: " << shared_data << std::endl;
}

void adopt_example()
{
    std::thread t1(increment_data_v2);
    std::thread t2(increment_data_v2);
    std::thread t3(increment_data_v2);
    
    t1.join();
    t2.join();
    t3.join();
    
    std::cout << "Final value of shared_data: " << shared_data << std::endl;
}

void normal_example()
{
    std::thread t1(increment_data);
    std::thread t2(increment_data);
    std::thread t3(increment_data);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Final value of shared_data: " << shared_data << std::endl;
}


int main()
{
    normal_example();
    //adopt_example();
    return 0;
}