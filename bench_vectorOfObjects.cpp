#include <benchmark/benchmark.h>

#include <vector>
#include <memory>
#include <string>

#include <fmt/format.h>

class X_Impl
{
 private:
  std::string i;
  std::string j;
  std::string k;
  std::string l;
};

// Note that this one is kind of broken because
// it doesn't have the cost of the copy/move operations for
// the shared_ptr, so I avoided using it further on.
class X
{
 public:
  X() : m_impl(std::make_shared<X_Impl>()) {
    fmt::print("Default Ctor\n");
  };

  X(X const& other) : m_impl(other.m_impl) {
    fmt::print("Copy Ctor\n");
  };
  X& operator=(X const& other) {
    m_impl = other.m_impl;
    fmt::print("Copy Assignment Ctor\n");
    return *this;
  };

  X(X&& other) : m_impl(std::move(other.m_impl)) {
    fmt::print("Move Ctor\n");
  };
  X& operator=(X&& other) {
    m_impl = std::move(other.m_impl);
    fmt::print("Move Assignment Ctor\n");
    return *this;
  };

  //~X() { fmt::print("Destructor\n"); };
  ~X() = default;

 private:
  // Comment that out, and WithoutReference is faster
  std::shared_ptr<X_Impl> m_impl;
};

class X_RuleOf0
{
 public:
  X_RuleOf0() {
    impl = std::make_shared<X_Impl>();
  }

 private:
  std::shared_ptr<X_Impl> impl;
};

class X_RuleOf0_MemberInit
{
 public:
  X_RuleOf0_MemberInit() : impl{std::make_shared<X_Impl>()} {}

 private:
  std::shared_ptr<X_Impl> impl;
};

class X_NoRuleOf0_MemberInit
{
 public:
  X_NoRuleOf0_MemberInit() : impl{std::make_shared<X_Impl>()} {}

  // break performance for the fun of it!!
  ~X_NoRuleOf0_MemberInit() {}

 private:
  std::shared_ptr<X_Impl> impl;
};

static void WithReference(benchmark::State& state) {

  struct MyEntry
  {
    MyEntry(X_RuleOf0& t_x) : x(t_x){};
    X_RuleOf0& x;
  };

  for (auto _ : state) {
    std::vector<MyEntry> vec;
    X_RuleOf0 x1;
    vec.emplace_back(x1);
    X_RuleOf0 x2;
    vec.emplace_back(x2);
  }
}
BENCHMARK(WithReference);

static void WithOutReference(benchmark::State& state) {

  struct MyEntry
  {
    MyEntry(const X_RuleOf0& t_x) : x(t_x){};
    X_RuleOf0 x;
  };

  for (auto _ : state) {
    std::vector<MyEntry> vec;
    X_RuleOf0 x1;
    vec.emplace_back(x1);
    X_RuleOf0 x2;
    vec.emplace_back(x2);
  }
}
BENCHMARK(WithOutReference);

static void WithOutTemporaries(benchmark::State& state) {

  struct MyEntry
  {
    MyEntry(const X_RuleOf0& t_x) : x(t_x){};
    X_RuleOf0 x;
  };

  for (auto _ : state) {
    std::vector<MyEntry> vec;
    // we are avoiding the expensive shared_ptr copy here
    vec.emplace_back(X_RuleOf0{});
    vec.emplace_back(X_RuleOf0{});
  }
}
BENCHMARK(WithOutTemporaries);

static void WithMoves(benchmark::State& state) {

  struct MyEntry
  {
    // follow the copy-move idiom for construction
    MyEntry(X_RuleOf0 t_x) : x(std::move(t_x)){};
    X_RuleOf0 x;
  };

  for (auto _ : state) {
    std::vector<MyEntry> vec;
    vec.emplace_back(X_RuleOf0{});
    vec.emplace_back(X_RuleOf0{});
  }
}
BENCHMARK(WithMoves);

static void WithRuleOf0MemberInit(benchmark::State& state) {

  struct MyEntry
  {
    MyEntry(X_RuleOf0_MemberInit t_x) : x(std::move(t_x)){};
    X_RuleOf0_MemberInit x;
  };

  for (auto _ : state) {
    std::vector<MyEntry> vec;
    vec.emplace_back(X_RuleOf0_MemberInit{});
    vec.emplace_back(X_RuleOf0_MemberInit{});
  }
}
BENCHMARK(WithRuleOf0MemberInit);

// this example shows what happens if you break the rule of 0
// by providing an empty destructor
static void WithNoRuleOf0MemberInit(benchmark::State& state) {

  struct MyEntry
  {
    MyEntry(X_NoRuleOf0_MemberInit t_x) : x(std::move(t_x)){};
    X_NoRuleOf0_MemberInit x;

    // break performance for the fun of it!!
    ~MyEntry() {}
  };

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::vector<MyEntry> vec;
    vec.emplace_back(X_NoRuleOf0_MemberInit{});
    vec.emplace_back(X_NoRuleOf0_MemberInit{});
  }
}
// Register the function as a benchmark
BENCHMARK(WithNoRuleOf0MemberInit);

static void WithRuleOf0MemberInit_reserve(benchmark::State& state) {

  struct MyEntry
  {
    // 0 copies in this version, but one move per object's construction
    MyEntry(X_RuleOf0_MemberInit t_x) : x(std::move(t_x)){};
    X_RuleOf0_MemberInit x;
  };

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::vector<MyEntry> vec;
    // by reserving ahead of time we know that there will be no data moves or copies
    vec.reserve(2);
    // direct-init the t_x parameter for the constructor
    vec.emplace_back(X_RuleOf0_MemberInit{});
    vec.emplace_back(X_RuleOf0_MemberInit{});
  }
}
// Register the function as a benchmark
BENCHMARK(WithRuleOf0MemberInit_reserve);

static void WithRuleOf0MemberInit_reserve_rvref_constructor(benchmark::State& state) {

  struct MyEntry
  {
    // illustrate how avoiding one more move operation can help
    MyEntry(X_RuleOf0_MemberInit&& t_x) : x(std::move(t_x)){};
    X_RuleOf0_MemberInit x;
  };

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::vector<MyEntry> vec;
    vec.reserve(2);
    vec.emplace_back(X_RuleOf0_MemberInit{});
    vec.emplace_back(X_RuleOf0_MemberInit{});
  }
}
// Register the function as a benchmark
BENCHMARK(WithRuleOf0MemberInit_reserve_rvref_constructor);

static void WithRuleOf0MemberInit_reserve_direct_init(benchmark::State& state) {

  // why use a constructor at all? We only have 1 public member!
  struct MyEntry
  {
    X_RuleOf0_MemberInit x;
  };

  for (auto _ : state) {
    std::vector<MyEntry> vec;
    vec.reserve(2);
    // note that emplace_back requires something constructible with ()
    // and because we don't provide a constructor, it is not,
    // but push_back will work just as well
    vec.push_back(MyEntry{X_RuleOf0_MemberInit{}});
    vec.push_back(MyEntry{X_RuleOf0_MemberInit{}});
  }
}
BENCHMARK(WithRuleOf0MemberInit_reserve_direct_init);
