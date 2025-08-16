// file: condition_variable
template <class _Lock>
struct _NODISCARD _Unlock_guard {
    explicit _Unlock_guard(_Lock& _Mtx_) : _Mtx(_Mtx_) {
        _Mtx.unlock();
    }

    ~_Unlock_guard() noexcept /* terminates */ {
        // relock mutex or terminate()
        // condition_variable_any wait functions are required to terminate if
        // the mutex cannot be relocked;
        // we slam into noexcept here for easier user debugging.
        _Mtx.lock();
    }

    _Unlock_guard(const _Unlock_guard&)            = delete;
    _Unlock_guard& operator=(const _Unlock_guard&) = delete;

private:
    _Lock& _Mtx;
};

// file: mutex
_EXPORT_STD class condition_variable { // class for waiting for conditions
public:
#ifdef _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
    condition_variable() noexcept /* strengthened */ {
        _Cnd_init_in_situ(_Mycnd());
    }
#else // ^^^ defined(_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR) / !defined(_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR) vvv
    condition_variable() noexcept /* strengthened */ = default;
#endif // ^^^ !defined(_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR) ^^^

    ~condition_variable() noexcept = default;

    condition_variable(const condition_variable&)            = delete;
    condition_variable& operator=(const condition_variable&) = delete;

    void notify_one() noexcept { // wake up one waiter
        _Cnd_signal(_Mycnd());
    }

    void notify_all() noexcept { // wake up all waiters
        _Cnd_broadcast(_Mycnd());
    }

    void wait(unique_lock<mutex>& _Lck) noexcept /* strengthened */ { // wait for signal
        // Nothing to do to comply with LWG-2135 because std::mutex lock/unlock are nothrow
        _Cnd_wait(_Mycnd(), _Lck.mutex()->_Mymtx());
    }

    template <class _Predicate>
    void wait(unique_lock<mutex>& _Lck, _Predicate _Pred) { // wait for signal and test predicate
        while (!_Pred()) {
            wait(_Lck);
        }
    }

    template <class _Rep, class _Period>
    cv_status wait_for(unique_lock<mutex>& _Lck, const chrono::duration<_Rep, _Period>& _Rel_time) {
        // wait for duration
        if (_Rel_time <= chrono::duration<_Rep, _Period>::zero()) {
            // we don't unlock-and-relock _Lck for this case because it's not observable
            return cv_status::timeout;
        }
        return wait_until(_Lck, _To_absolute_time(_Rel_time));
    }

    template <class _Rep, class _Period, class _Predicate>
    bool wait_for(unique_lock<mutex>& _Lck, const chrono::duration<_Rep, _Period>& _Rel_time, _Predicate _Pred) {
        // wait for signal with timeout and check predicate
        return wait_until(_Lck, _To_absolute_time(_Rel_time), _STD _Pass_fn(_Pred));
    }

    template <class _Clock, class _Duration>
    cv_status wait_until(unique_lock<mutex>& _Lck, const chrono::time_point<_Clock, _Duration>& _Abs_time) {
        // wait until time point
        static_assert(chrono::_Is_clock_v<_Clock>, "Clock type required");
#if _ITERATOR_DEBUG_LEVEL != 0
        _STL_VERIFY(
            _Lck.owns_lock(), "wait_until's caller must own the lock argument (N4958 [thread.condition.condvar]/17)");
        _STL_VERIFY(_Mtx_current_owns(_Lck.mutex()->_Mymtx()),
            "wait_until's calling thread must hold the lock argument's mutex (N4958 [thread.condition.condvar]/17)");
#endif // _ITERATOR_DEBUG_LEVEL != 0
        for (;;) {
            const auto _Now = _Clock::now();
            if (_Abs_time <= _Now) {
                // we don't unlock-and-relock _Lck for this case because it's not observable
                return cv_status::timeout;
            }

            const unsigned long _Rel_ms_count = _Clamped_rel_time_ms_count(_Abs_time - _Now)._Count;

            const _Thrd_result _Res = _Cnd_timedwait_for_unchecked(_Mycnd(), _Lck.mutex()->_Mymtx(), _Rel_ms_count);
            if (_Res == _Thrd_result::_Success) {
                return cv_status::no_timeout;
            }
        }
    }

    template <class _Clock, class _Duration, class _Predicate>
    bool wait_until(
        unique_lock<mutex>& _Lck, const chrono::time_point<_Clock, _Duration>& _Abs_time, _Predicate _Pred) {
        // wait for signal with timeout and check predicate
        static_assert(chrono::_Is_clock_v<_Clock>, "Clock type required");
        while (!_Pred()) {
            if (wait_until(_Lck, _Abs_time) == cv_status::timeout) {
                return _Pred();
            }
        }

        return true;
    }

    // native_handle_type and native_handle() have intentionally been removed. See GH-3820.

    void _Register(unique_lock<mutex>& _Lck, int* _Ready) noexcept { // register this object for release at thread exit
        _Cnd_register_at_thread_exit(_Mycnd(), _Lck.release()->_Mymtx(), _Ready);
    }

    void _Unregister(mutex& _Mtx) noexcept { // unregister this object for release at thread exit
        _Cnd_unregister_at_thread_exit(_Mtx._Mymtx());
    }

private:
    _Cnd_internal_imp_t _Cnd_storage{};

    _Cnd_t _Mycnd() noexcept {
        return &_Cnd_storage;
    }
};

