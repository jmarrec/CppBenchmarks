#include <benchmark/benchmark.h>

#include <algorithm>
#include <chrono>
#include <execution>
#include <vector>
#include <thread>

struct A
{
  int64_t i;
  int64_t nsSleep;

  A(int64_t t_i, int64_t t_nsSleep) : i(t_i), nsSleep(t_nsSleep){};

  int64_t reset() {
    if (i > 10) {
      i = 0;
    }
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(std::chrono::nanoseconds{nsSleep});
    return i;
  }

  auto operator<=>(const A&) const = default;
};

std::vector<A> makeVector(int64_t lenVector, int64_t msSleep) {
  std::vector<A> result;
  for (int64_t i = 0; i < lenVector; ++i) {
    result.emplace_back(i, msSleep);
  }
  return result;
}

static void BM_Seq(benchmark::State& state) {
  auto vec = makeVector(state.range(0), state.range(1));

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::for_each(std::execution::seq, vec.begin(), vec.end(), std::mem_fn(&A::reset));
  }
}

static void BM_Par(benchmark::State& state) {
  auto vec = makeVector(state.range(0), state.range(1));

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::for_each(std::execution::par, vec.begin(), vec.end(), std::mem_fn(&A::reset));
  }
}

static void BM_Par_Unseq(benchmark::State& state) {
  auto vec = makeVector(state.range(0), state.range(1));

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    std::for_each(std::execution::par_unseq, vec.begin(), vec.end(), std::mem_fn(&A::reset));
  }
}

// Test with  1, 1000, and 10000 elements
// Test with 1ns, 1µs, 0.1ms
BENCHMARK(BM_Seq)->ArgsProduct({{1, 1000, 10000}, {1, 1000, 100000}})->UseRealTime();
BENCHMARK(BM_Par)->ArgsProduct({{1, 1000, 10000}, {1, 1000, 100000}})->UseRealTime();
BENCHMARK(BM_Par_Unseq)->ArgsProduct({{1, 1000, 10000}, {1, 1000, 100000}})->UseRealTime();

// BENCHMARK(BM_Seq)->ArgsProduct({{1, 1000, 10000}, {1, 1000, 100000}});
// BENCHMARK(BM_Par)->ArgsProduct({{1, 1000, 10000}, {1, 1000, 100000}});
// BENCHMARK(BM_Par_Unseq)->ArgsProduct({{1, 1000, 10000}, {1, 1000, 100000}});
