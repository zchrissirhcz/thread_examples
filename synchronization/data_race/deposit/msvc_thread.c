#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <process.h>

typedef struct {
    double balance;
    SRWLOCK srw_lock;  // SRW Lock object
} BankAccount;

typedef struct {
    BankAccount* account;
    double deposit_amount;
    BOOL use_mutex;
    int thread_num;  // Thread number for debugging
} ThreadData;

void init_account(BankAccount* account, double initial_balance) {
    account->balance = initial_balance;
    InitializeSRWLock(&account->srw_lock);
}

void cleanup_account(BankAccount* account) {
    // SRW Lock does not need explicit cleanup
}

// Unsafe deposit operation (may cause data race)
void deposit_unsafe(BankAccount* account, double amount) {
    double prev_balance = account->balance;
    Sleep(10); // Simulate work - longer delay to increase race chance
    account->balance = prev_balance + amount;
}

// Safe deposit operation (protected by SRW Lock)
void deposit_safe(BankAccount* account, double amount) {
    // Acquire exclusive lock
    AcquireSRWLockExclusive(&account->srw_lock);
    
    // Critical section: read-modify-write must be atomic
    double prev_balance = account->balance;
    Sleep(10); // Simulate work
    account->balance = prev_balance + amount;
    
    // Release the lock
    ReleaseSRWLockExclusive(&account->srw_lock);
}

double get_balance(BankAccount* account) {
    double balance;
    AcquireSRWLockExclusive(&account->srw_lock);
    balance = account->balance;
    ReleaseSRWLockExclusive(&account->srw_lock);
    return balance;
}

unsigned int __stdcall deposit_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    if (data->use_mutex) {
        deposit_safe(data->account, data->deposit_amount);
    } else {
        deposit_unsafe(data->account, data->deposit_amount);
    }
    
    return 0;
}

// Test race condition
void test_race_condition(BOOL use_mutex) {
    BankAccount account;
    init_account(&account, 0.0);
    
    const int num_threads = 64; // no more than MAXIMUM_WAIT_OBJECTS
    const double deposit_amount = 100.0;
    
    HANDLE* threads = malloc(num_threads * sizeof(HANDLE));
    ThreadData* thread_data = malloc(num_threads * sizeof(ThreadData));
    unsigned int* thread_ids = malloc(num_threads * sizeof(unsigned int));
    
    if (!threads || !thread_data || !thread_ids) {
        printf("Failed to allocate memory\n");
        return;
    }
    
    // Create threads
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].account = &account;
        thread_data[i].deposit_amount = deposit_amount;
        thread_data[i].use_mutex = use_mutex;
        thread_data[i].thread_num = i;
        
        threads[i] = (HANDLE)_beginthreadex(
            NULL,                    // Default security attributes
            0,                       // Default stack size
            deposit_thread,          // Thread function
            &thread_data[i],         // Argument to thread function
            0,                       // Default creation flags
            &thread_ids[i]           // Return thread identifier
        );
        
        if (threads[i] == NULL) {
            printf("Failed to create thread %d: %lu\n", i, GetLastError());
            // Wait for created threads to finish
            for (int j = 0; j < i; ++j) {
                WaitForSingleObject(threads[j], INFINITE);
                CloseHandle(threads[j]);
            }
            free(threads);
            free(thread_data);
            free(thread_ids);
            cleanup_account(&account);
            return;
        }
    }
    
    // Wait for all threads to finish
    WaitForMultipleObjects(num_threads, threads, TRUE, INFINITE);
    
    // Close thread handles
    for (int i = 0; i < num_threads; ++i) {
        CloseHandle(threads[i]);
    }
    
    double expected_balance = deposit_amount * num_threads;
    double actual_balance = get_balance(&account);
    
    printf("Expected balance: %.2f\n", expected_balance);
    printf("Actual balance: %.2f\n", actual_balance);
    printf("Difference: %.2f\n", expected_balance - actual_balance);
    
    // Cleanup resources
    free(threads);
    free(thread_data);
    free(thread_ids);
    cleanup_account(&account);
}

int main() {
    printf("MAXIMUM_WAIT_OBJECTS: %d\n", MAXIMUM_WAIT_OBJECTS);

    printf("Testing without SRW Lock (unsafe):\n");
    test_race_condition(FALSE);

    printf("\nTesting with SRW Lock (safe):\n");
    test_race_condition(TRUE);
    
    return 0;
}
