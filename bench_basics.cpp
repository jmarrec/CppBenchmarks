#include <benchmark/benchmark.h>
#include <iostream>

static void BM_OutputN(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  std::string s("hello");
  for (auto _ : state) {
    std::cout << s << '\n';
  }
}
// Register the function as a benchmark
BENCHMARK(BM_OutputN);

static void BM_OutputEndl(benchmark::State& state) {
  std::string s("hello");
  for (auto _ : state) {
    std::cout << s << std::endl;
  }
}
// Register the function as a benchmark
BENCHMARK(BM_OutputEndl);

// Run the benchmark
BENCHMARK_MAIN();
