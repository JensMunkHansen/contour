template <typename T>
struct point2_t : public std::array<T, 2> {
  point2_t() = default;

  point2_t(point2_t<T>&& t) : std::array<T, 2>(std::move(t)) {}

  point2_t(const point2_t<T>& t) : std::array<T, 2>(t) {}

  point2_t<T>& operator=(const point2_t<T>& other)  {
    return static_cast<point2_t<T>&>(std::array<T, 2>::operator=(other));
  }

  point2_t<T>& operator=(point2_t<T>&& other) {
    return static_cast<std::array<T, 2>&>(
             std::array<T, 2>::operator=(std::move(other)));
  }

#if 0
  // Warning this is more specialized
  template <typename... Args>
  explicit point2_t(Args &&... args) :
    std::array<T, 2>( {
    T(std::forward<Args>(args))...
  }) {}
#endif

  point2_t(std::initializer_list<T> args) :
    std::array<T, 2>(reinterpret_cast<std::array<T, 2> const&>(
                       *(args.begin()))) {}
};
