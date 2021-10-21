#include <benchmark/benchmark.h>

#include <iostream>

static void StringCreation(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  std::cout << state.range(0) << ", " << (state.range(1) == 0 ? false : true) << '\n';
  for (auto _ : state) {
    std::string created_string("hello");
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(created_string);
  }
}
// Register the function as a benchmark
BENCHMARK(StringCreation)->Ranges({{1, 512}, {0, 1}});
