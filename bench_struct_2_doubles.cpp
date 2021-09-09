#include <benchmark/benchmark.h>

#include <array>

struct A
{
  double i;
  double j;
};

double byRef(const A& a) {
  return a.i + a.j;
}

double byVal(A a) {
  return a.i + a.j;
}

static void BM_ByRef(benchmark::State& state) {
  A a{.i = 10.0, .j = 20.0};
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      double x = byRef(a);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}

static void BM_ByVal(benchmark::State& state) {
  A a{.i = 10.0, .j = 20.0};
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      double x = byVal(a);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}

// Register the function as a benchmark
BENCHMARK(BM_ByRef)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_ByVal)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
