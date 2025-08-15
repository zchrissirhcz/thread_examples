#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct BankAccount
{
    double balance;
    pthread_mutex_t mtx;
} BankAccount;

void bank_account_init(BankAccount* account, double initial_balance)
{
    account->balance = initial_balance;
}

void bank_account_deposit_unsafe(BankAccount* account, double amount)
{
    double prev_balance = account->balance;
    sleep(1); // simulate some work
    account->balance = prev_balance + amount;
}

void bank_account_deposit_safe(BankAccount* account, double amount)
{
    pthread_mutex_lock(&account->mtx);
    sleep(1); // simulate some work
    account->balance += amount;
    pthread_mutex_unlock(&account->mtx);
}

void bank_account_get_balance(BankAccount* account, double* balance)
{
    pthread_mutex_lock(&account->mtx);
    *balance = account->balance;
    pthread_mutex_unlock(&account->mtx);
}

typedef struct DepositParam
{
    BankAccount* account;
    double deposit_amount;
} DepositParam;

void* bank_account_deposit_safe_wrapper(void* args)
{
    DepositParam* param = (DepositParam*) args;
    bank_account_deposit_safe(param->account, param->deposit_amount);
    return NULL;
}

void* bank_account_deposit_unsafe_wrapper(void* args)
{
    DepositParam* param = (DepositParam*) args;
    bank_account_deposit_unsafe(param->account, param->deposit_amount);
    return NULL;
}

void test_race_condition(bool use_mutex)
{
    BankAccount account;
    bank_account_init(&account, 0.0);
    const int num_threads = 100;
    const double deposit_amount = 100.0;

    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * num_threads);
    DepositParam* params = (DepositParam*) malloc(sizeof(DepositParam) * num_threads);

    for (int i=0; i<num_threads; i++)
    {
        params[i].account = &account;
        params[i].deposit_amount = deposit_amount;
        if (use_mutex)
        {
            pthread_create(&threads[i], NULL, bank_account_deposit_safe_wrapper, &params[i]);
        }
        else
        {
            pthread_create(&threads[i], NULL, bank_account_deposit_unsafe_wrapper, &params[i]);
        }
    }

    for (int i=0; i<num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    double expected_balance = deposit_amount * num_threads;
    double actual_balance;
    bank_account_get_balance(&account, &actual_balance);

    printf("Expected balance: %f\n", expected_balance);
    printf("Actual balance: %f\n", actual_balance);
    printf("Difference: %f\n", expected_balance - actual_balance);
}

int main()
{
    printf("Testing without mutex (unsafe):\n");
    test_race_condition(false);

    printf("\nTesting with mutex (safe):\n");
    test_race_condition(true);

    return 0;
}