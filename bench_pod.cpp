#include <benchmark/benchmark.h>

double byRef(const double& d) {
  double x = d;
  x += 1.0;
  return x;
}

double byVal(double d) {
  d += 1.0;
  return d;
}

static void BM_ByRef(benchmark::State& state) {
  double d = 10.0;
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      double x = byRef(d);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}


static void BM_ByVal(benchmark::State& state) {
  double d = 10.0;
  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      double x = byVal(d);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));

}

// Register the function as a benchmark
BENCHMARK(BM_ByRef)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_ByVal)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
