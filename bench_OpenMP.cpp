#include <benchmark/benchmark.h>

#include <iostream>
#include <numeric>
#include <vector>

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
#pragma omp parallel for num_threads(16)
    for (int i = 0; i < 1000000000; ++i) {
      //std::cout << i << '\n';
      int j = i;
      benchmark::DoNotOptimize(j);
    }
  }
}

// ----- pairwise add+multiply: plain vs #pragma omp declare simd -----

template <typename T>
std::pair<std::vector<T>, std::vector<T>> getInputVectors(size_t size) {
  std::vector<T> a(size);
  std::vector<T> b(size);
  std::iota(a.begin(), a.end(), static_cast<T>(0));
  std::iota(b.begin(), b.end(), static_cast<T>(1));
  return {a, b};
}

static constexpr size_t VEC_SIZE = 1 << 16;


// Baseline: noinline prevents auto-vectorization through inlining
[[gnu::noinline]] float plain_op(float a, float b) { return a * b + a; }
[[gnu::noinline]] double plain_op(double a, double b) { return a * b + a; }

// declare simd on a template is poorly supported — use concrete overloads so
// the compiler generates a correct SIMD variant for each type.
#pragma omp declare simd
[[gnu::noinline]] float simd_op(float a, float b) { return a * b + a; }

#pragma omp declare simd
[[gnu::noinline]] double simd_op(double a, double b) { return a * b + a; }

static void BM_PairwisePlainFloat(benchmark::State& state) {
  const auto [a, b] = getInputVectors<float>(VEC_SIZE);
  std::vector<float> result(a.size());

  for (auto _ : state) {
    for (size_t i = 0; i < a.size(); ++i) {
      result[i] = plain_op(a[i], b[i]);
    }
    benchmark::DoNotOptimize(result.data());
  }
}

static void BM_PairwiseDeclareSimdFloat(benchmark::State& state) {
  const auto [a, b] = getInputVectors<float>(VEC_SIZE);
  std::vector<float> result(a.size());
  for (auto _ : state) {
#pragma omp simd
    for (size_t i = 0; i < a.size(); ++i) {
      result[i] = simd_op(a[i], b[i]);
    }
    benchmark::DoNotOptimize(result.data());
  }
}

static void BM_PairwisePlainDouble(benchmark::State& state) {
  const auto [a, b] = getInputVectors<double>(VEC_SIZE);
  std::vector<double> result(a.size());
  for (auto _ : state) {
    for (size_t i = 0; i < a.size(); ++i) {
      result[i] = plain_op(a[i], b[i]);
    }
    benchmark::DoNotOptimize(result.data());
  }
}

static void BM_PairwiseDeclareSimdDouble(benchmark::State& state) {
  const auto [a, b] = getInputVectors<double>(VEC_SIZE);
  std::vector<double> result(a.size());
  for (auto _ : state) {
#pragma omp simd
    for (size_t i = 0; i < a.size(); ++i) {
      result[i] = simd_op(a[i], b[i]);
    }
    benchmark::DoNotOptimize(result.data());
  }
}

// Register the function as a benchmark
BENCHMARK(BM_Regular)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_OpenMP)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_PairwisePlainFloat)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_PairwiseDeclareSimdFloat)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_PairwisePlainDouble)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_PairwiseDeclareSimdDouble)->Unit(benchmark::kMicrosecond);
