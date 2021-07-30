#include <benchmark/benchmark.h>
#include <iostream>

static void BM_Regular(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (size_t i = 0; i < 100000000000; ++i) {
      // std::cout << i << '\n';
      size_t j = i;
      benchmark::DoNotOptimize(j);
    }
  }
}

static void BM_OpenMP(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    # pragma omp parallel for num_threads(16)
    for (size_t i = 0; i < 10000000000; ++i) {
      //std::cout << i << '\n';
      size_t j = i;
      benchmark::DoNotOptimize(j);
    }
  }
}

// Register the function as a benchmark
BENCHMARK(BM_Regular);
BENCHMARK(BM_OpenMP);
