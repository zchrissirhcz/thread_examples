#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <process.h>

unsigned int __stdcall f(void* arg)
{
    printf("hello from thread\n");
    Sleep(1000); // simulate some work
    printf("bye from thread\n");
    return 0;
}

int main()
{
    printf("hello from main\n");

    unsigned int thread_id;
    HANDLE thread = (HANDLE)_beginthreadex(
        NULL,       // default security attributes
        0,          // use default stack size
        f,          // thread function name
        NULL,       // argument to thread function
        0,          // use default creation flags
        &thread_id  // returns the thread identifier
    );

    if (thread == NULL)
    {
        printf("Error creating thread: %lu\n", GetLastError());
        return 1;
    }

    // wait for the thread to finish
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    printf("bye from main\n");
}
