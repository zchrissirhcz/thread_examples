#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>

class LockGuard {
public:
    explicit LockGuard(std::mutex& mtx) : mtx_(mtx) { mtx_.lock(); }
    ~LockGuard() { mtx_.unlock(); }
private:
    std::mutex& mtx_;
    LockGuard(const LockGuard&);
    LockGuard& operator=(const LockGuard&);
};

class BankAccount
{
public:
    BankAccount(double initial_balance): balance(initial_balance) {}

    void deposit_unsafe(double amount)
    {
        double prev_balance = balance;
        std::this_thread::sleep_for(std::chrono::microseconds(1)); // simulate some work
        balance = prev_balance + amount;
    }

    void deposit_safe(double amount)
    {
        mtx.lock();
        std::this_thread::sleep_for(std::chrono::microseconds(1)); // simulate some work
        balance += amount;
        mtx.unlock();
    }

    double get_balance()
    {
        LockGuard lock_guard(mtx);
        return balance;
    }

private:
    double balance{};
    std::mutex mtx;
};

template<bool use_mutex>
void test_race_condition()
{
    BankAccount account(0.0);
    std::vector<std::thread> threads;
    const int num_threads = 100;
    const double deposit_amount = 100.0;

    for (int i = 0; i < num_threads; ++i)
    {
        if (use_mutex)
        {
            threads.emplace_back([&account, deposit_amount]
                                 { account.deposit_safe(deposit_amount); });
        }
        else
        {
            threads.emplace_back([&account, deposit_amount]
                                 { account.deposit_unsafe(deposit_amount); });
        }
    }

    for (auto &thread : threads)
        thread.join();

    double expected_balance = deposit_amount * num_threads;
    double actual_balance = account.get_balance();

    std::cout << "Expected balance: " << expected_balance << std::endl;
    std::cout << "Actual balance: " << actual_balance << std::endl;
    std::cout << "Difference: " << expected_balance - actual_balance << std::endl;
}

int main()
{
    std::cout << "Testing without mutex (unsafe):" << std::endl;
    test_race_condition<false>();

    std::cout << "\nTesting with mutex (safe):" << std::endl;
    test_race_condition<true>();

    return 0;
}