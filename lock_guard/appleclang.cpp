struct _LIBCPP_EXPORTED_FROM_ABI adopt_lock_t {
  explicit adopt_lock_t() = default;
};

template <class _Mutex>
class _LIBCPP_TEMPLATE_VIS _LIBCPP_THREAD_SAFETY_ANNOTATION(scoped_lockable) lock_guard {
public:
  typedef _Mutex mutex_type;

private:
  mutex_type& __m_;

public:
  _LIBCPP_NODISCARD
  _LIBCPP_HIDE_FROM_ABI explicit lock_guard(mutex_type& __m) _LIBCPP_THREAD_SAFETY_ANNOTATION(acquire_capability(__m))
      : __m_(__m) {
    __m_.lock();
  }

  _LIBCPP_NODISCARD _LIBCPP_HIDE_FROM_ABI lock_guard(mutex_type& __m, adopt_lock_t)
      _LIBCPP_THREAD_SAFETY_ANNOTATION(requires_capability(__m))
      : __m_(__m) {}
  _LIBCPP_HIDE_FROM_ABI ~lock_guard() _LIBCPP_THREAD_SAFETY_ANNOTATION(release_capability()) { __m_.unlock(); }

  lock_guard(lock_guard const&)            = delete;
  lock_guard& operator=(lock_guard const&) = delete;
};