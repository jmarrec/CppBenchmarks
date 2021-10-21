#include <benchmark/benchmark.h>
#include <cmath>
#include <initializer_list>

constexpr double normalizeAngleto0to360manual(double angleDegrees) {
  while (angleDegrees < 0.0) {
    angleDegrees += 360.0;
  }
  while (angleDegrees > 360.0) {
    angleDegrees -= 360.0;
  }
  return angleDegrees;
}

constexpr double normalizeAngleto0to360fmod(double angleDegrees) {
  while (angleDegrees < 0.0) {
    angleDegrees += 360.0;
  }
  return std::fmod(angleDegrees, 360.0);
}

static void BM_manual(benchmark::State& state) {
  for (auto _ : state) {
    for (double d : {-3000.0, -350.0, -10.0, 10.0, 350.0, 3000.0}) {
      double mod = normalizeAngleto0to360manual(d);
      benchmark::DoNotOptimize(mod);
    }
  }
}

static void BM_fmod(benchmark::State& state) {
  for (auto _ : state) {
    for (double d : {-3000.0, -350.0, -10.0, 10.0, 350.0, 3000.0}) {
      double mod = normalizeAngleto0to360fmod(d);
      benchmark::DoNotOptimize(mod);
    }
  }
}
// Register the function as a benchmark
BENCHMARK(BM_manual);
BENCHMARK(BM_fmod);
