#include <benchmark/benchmark.h>
#include <iterator>     // for make_move_iterator
#include <memory>       // for make_shared, shared_ptr
#include <string>       // for string
#include <type_traits>  // for declval, decay_t, enable_if_t
#include <utility>      // for move, forward
#include <vector>       // for vector

#include <fmt/format.h>  // for print

static constexpr bool DEBUG = false;
static constexpr size_t VECTOR_SIZE = DEBUG ? 2 : 100;

void print_if_debug([[maybe_unused]] std::string_view msg) {
  if constexpr (DEBUG) {
    //  fmt::print(msg);
  }
}

namespace detail {
class X_Impl
{
 public:
  X_Impl() {
    ++count;
  };
  X_Impl(std::string t_i) : i(std::move(t_i)) {
    ++count;
  }
  virtual ~X_Impl() {
    --count;
  }

  static size_t howMany() {
    return count;
  }

  [[nodiscard]] std::string get_i() const {
    return i;
  };
  bool set_i(const std::string& t_i) {
    i = t_i;
    return true;
  };

 private:
  std::string i;
  std::string j;
  std::string k;
  std::string l;
  static size_t count;
};

size_t X_Impl::count = 0;

class Child_Impl : public X_Impl
{
 public:
  Child_Impl(std::string t_i) : X_Impl(std::move(t_i)) {}
};
}  // namespace detail

class X
{
 public:
  X() : m_impl(std::make_shared<detail::X_Impl>()) {
    print_if_debug("Default Ctor\n");
    ++count;
  };

  X(std::string i) : m_impl(std::make_shared<detail::X_Impl>(std::move(i))) {
    print_if_debug("Ctor i\n");
    ++count;
  };

 protected:
  X(std::shared_ptr<detail::X_Impl> impl) : m_impl(std::move(impl)) {
    print_if_debug("X impl Ctor");
    ++count;
  }

 public:
  X(X const& other) : m_impl(other.m_impl) {
    print_if_debug("Copy Ctor\n");
    ++count;
  };
  X& operator=(X const& other) {
    if (this == &other) {
      return *this;
    }
    m_impl = other.m_impl;
    print_if_debug("Copy Assignment Ctor\n");
    ++count;
    return *this;
  };

  X(X&& other) noexcept : m_impl(std::move(other.m_impl)) {
    print_if_debug("Move Ctor\n");
    ++count;
  };
  X& operator=(X&& other) noexcept {
    m_impl = std::move(other.m_impl);
    print_if_debug("Move Assignment Ctor\n");
    ++count;
    return *this;
  };

  static size_t howMany() {
    return count;
  }

  [[nodiscard]] std::string get_i() const {
    return m_impl->get_i();
  };
  bool set_i(const std::string& t_i) {
    return m_impl->set_i(t_i);
  };

  virtual ~X() {
    print_if_debug("Destructor\n");
    --count;
  };

  template <typename T>
  std::shared_ptr<T> getImpl() const {
    return std::dynamic_pointer_cast<T>(m_impl);
  }

  /// cast to type T, can throw std::bad_cast
  template <typename T>
  T cast() const {
    std::shared_ptr<typename T::ImplType> impl = this->getImpl<typename T::ImplType>();
    if (!impl) {
      throw(std::bad_cast());
    }
    return T(std::move(impl));
  }

 protected:
  using ImplType = detail::X_Impl;

 private:
  //  Comment that out, and WithoutReference is faster
  std::shared_ptr<detail::X_Impl> m_impl;
  static size_t count;
};

size_t X::count = 0;

namespace detail {

// Define a helper test whether every predicate in a pack in true
template <typename... Conds>
struct and_ : std::true_type
{
};

template <typename Cond, typename... Conds>
struct and_<Cond, Conds...> : std::conditional<Cond::value, and_<Conds...>, std::false_type>::type
{
};

// Check that 1) it's a container, and 2) it contains elements that are convertible to ElementType
template <typename ElementType, typename Container, typename T = std::decay_t<decltype(*begin(std::declval<Container>()))>>
using VectorConvertible = std::enable_if_t<std::is_convertible_v<T, ElementType>>;

// Same, but for a parameter pack
template <typename ElementType, typename... Vectors>
using AllVectorsConvertible = and_<VectorConvertible<ElementType, Vectors>...>;

// Helper: when lvalue is passed
template <class Container1, class Container2>
void concat_helper(Container1& lhs, Container2& rhs) {
  lhs.insert(lhs.end(), rhs.begin(), rhs.end());
}

// Helper: when rvalue is passed
template <class Container1, class Container2>
void concat_helper(Container1& lhs, Container2&& rhs) {
  lhs.insert(lhs.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
}
}  // namespace detail

template <typename RT, typename... Containers, typename = detail::AllVectorsConvertible<RT, Containers...>>
std::vector<RT> concat(Containers&&... containers) {
  std::size_t s = (containers.size() + ...);
  std::vector<RT> result;
  result.reserve(s);
  (detail::concat_helper(result, std::forward<Containers>(containers)), ...);
  return result;
}

class ChildA : public X
{
 public:
  ChildA(std::string i) : X(std::make_shared<detail::Child_Impl>(std::move(i))) {
    print_if_debug("Child Ctor i\n");
  };

 protected:
  using ImplType = detail::Child_Impl;
};

class ChildB : public X
{
 public:
  ChildB(std::string i) : X(std::make_shared<detail::Child_Impl>(std::move(i))) {
    print_if_debug("Child Ctor i\n");
  };

 protected:
  using ImplType = detail::Child_Impl;
};

class ChildC : public X
{
 public:
  ChildC(std::string i) : X(std::make_shared<detail::Child_Impl>(std::move(i))) {
    print_if_debug("Child Ctor i\n");
  };

 protected:
  using ImplType = detail::Child_Impl;
};

template <class T, typename = std::enable_if_t<std::is_base_of_v<X, T>>>
std::vector<T> getChilds() {
  std::vector<T> result;
  result.reserve(VECTOR_SIZE);
  for (size_t i = 0; i < VECTOR_SIZE; ++i) {
    result.emplace_back("ChildA_" + std::to_string(i));
  }
  return result;
}

// This mimics what the children() method of any OpenStudio object does
static void Current(benchmark::State& state) {

  if (DEBUG) {
    print_if_debug("\n\nCURRENT\n");
    std::vector<X> result;
    auto childAs = getChilds<ChildA>();
    result.insert(result.end(), childAs.begin(), childAs.end());
  } else {

    for (auto _ : state) {
      std::vector<X> result;

      // shading groups
      auto childAs = getChilds<ChildA>();
      result.insert(result.end(), childAs.begin(), childAs.end());
      auto childBs = getChilds<ChildB>();
      result.insert(result.end(), childBs.begin(), childBs.end());
      auto childCs = getChilds<ChildC>();
      result.insert(result.end(), childCs.begin(), childCs.end());
      benchmark::DoNotOptimize(result);
    }
  }
}
BENCHMARK(Current);

static void New(benchmark::State& state) {

  if (DEBUG) {
    print_if_debug("\n\nNEW\n");

    auto childAs = getChilds<ChildA>();
    auto childBs = getChilds<ChildB>();
    std::vector<X> result = concat<X>(std::move(childAs), std::move(childBs), getChilds<ChildC>());

  } else {
    for (auto _ : state) {
      std::vector<X> result = concat<X>(getChilds<ChildA>(), getChilds<ChildB>(), getChilds<ChildC>());
      benchmark::DoNotOptimize(result);
    }
  }
}
BENCHMARK(New);
