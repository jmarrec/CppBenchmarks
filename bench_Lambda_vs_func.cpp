#include <benchmark/benchmark.h>

#include <string>
#include <vector>

static constexpr int N_ITER = 1'000'000;

int PreDefTableEntry(int x) {
  [[maybe_unused]] int ret = x * x;
  return ret;
}

int func(int base, int x) {
  auto ret = base + x;
  if (ret % 2 == 0) {
    ret += PreDefTableEntry(ret);
  }
  return ret;
}

static void BM_Func(benchmark::State& state) {
  int base = 10;
  int total = 0;
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (int i = 0; i < N_ITER; ++i) {
      total += func(base, i);
    }
  }
  benchmark::DoNotOptimize(total);
}

static void BM_Lambda(benchmark::State& state) {
  int base = 10;
  int total = 0;
  auto doStuff = [&base](int x) {
    auto ret = base + x;
    if (ret % 2 == 0) {
      ret += PreDefTableEntry(ret);
    }
    return ret;
  };
  for (auto _ : state) {
    for (int i = 0; i < N_ITER; ++i) {
      total += doStuff(i);
    }
  }
  benchmark::DoNotOptimize(total);
}

BENCHMARK(BM_Func);
BENCHMARK(BM_Lambda);
