#include <benchmark/benchmark.h>

#include <vector>
#include <memory>
#include <string>

class X_Impl
{
 public:
  [[nodiscard]] std::string get_i() const {
    return i;
  };

  void set_i(const std::string& new_i) {
    i = new_i;
  };

 private:
  std::string i;
  std::string j;
  std::string k;
  std::string l;
};

class X
{
 public:
  X() : impl{std::make_shared<X_Impl>()} {}
  [[nodiscard]] std::string get_i() const {
    return impl->get_i();
  }

  void set_i(const std::string& new_i) const {
    return impl->set_i(new_i);
  }

 private:
  std::shared_ptr<X_Impl> impl;
};

void passByValue(X x) {
  return x.set_i("new string is assigned");
}

void passByRef(X& x) {
  return x.set_i("new string is assigned");
}

static void BenchByVal(benchmark::State& state) {

  X x;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {
      passByValue(x);
    }
  }

  state.SetComplexityN(state.range(0));
}
// Register the function as a benchmark
BENCHMARK(BenchByVal)->Range(8, 8 << 10)->Complexity();

static void BenchByRef(benchmark::State& state) {

  X x;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {
      passByRef(x);
    }
  }

  state.SetComplexityN(state.range(0));
}
// Register the function as a benchmark
BENCHMARK(BenchByRef)->Range(8, 8 << 10)->Complexity();
