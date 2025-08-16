## Source

Microsoft STL: 
- code: https://github.com/microsoft/STL/tree/main/stl/inc
- doc: https://learn.microsoft.com/en-us/cpp/standard-library/cpp-standard-library-reference?view=msvc-170

LLVM: 
- code: https://github.com/llvm/llvm-project/tree/main/libcxx/include
- doc: https://libcxx.llvm.org/

each header file provides synopsis in the beginning:
```cpp
/*
    mutex synopsis

namespace std
{

class mutex
{
public:
     constexpr mutex() noexcept;
     ~mutex();

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    void lock();
    bool try_lock();
    void unlock();

    typedef pthread_mutex_t* native_handle_type;
    native_handle_type native_handle();
};
*/
```

libstdc++: 
- code: git://gcc.gnu.org/git/gcc.git
- code: https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/include/std
- doc: https://gcc.gnu.org/onlinedocs/libstdc++/
