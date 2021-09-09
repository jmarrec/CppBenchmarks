#include <benchmark/benchmark.h>

#include <array>

double byRef(const std::array<double, 2>& arr) {
  return arr[0] + arr[1];
}

double byVal(std::array<double, 2> arr) {
  return arr[0] + arr[1];
}

static void BM_ByRef(benchmark::State& state) {
  std::array<double, 2> arr{10.0, 20.0};
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      double x = byRef(arr);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}

static void BM_ByVal(benchmark::State& state) {
  std::array<double, 2> arr{10.0, 20.0};
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      double x = byVal(arr);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}

// Register the function as a benchmark
BENCHMARK(BM_ByRef)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_ByVal)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
