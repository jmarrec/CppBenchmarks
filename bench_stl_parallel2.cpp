#include <benchmark/benchmark.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <execution>
#include <iterator>
#include <numbers>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

std::vector<double> makeVector(int64_t lenVector) {

  std::vector<double> randValues;
  randValues.reserve(lenVector);

  std::mt19937 engine;
  std::uniform_real_distribution<> uniformDist(0, std::numbers::pi / 2);
  for (long long i = 0; i < lenVector; ++i) {
    randValues.push_back(uniformDist(engine));
  }

  return randValues;
}

double calculateTotal(const std::vector<double>& vec) {
  double total = 0.0;
  for (double d : vec) {
    total += std::tan(d);
  }
  return total;
}

static void BM_Manual(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double checkTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double total = calculateTotal(vec);
    benchmark::DoNotOptimize(total);
    if (std::abs(checkTotal - total) > 0.0001) {
      std::string msg = "Wrong calculation: error is " + std::to_string(std::abs(checkTotal - total));
      state.SkipWithError(msg.c_str());
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Seq(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double checkTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double total = std::reduce(std::execution::seq, vec.begin(), vec.end(), 0.0, [](double sum, double arg) { return sum + std::tan(arg); });
    benchmark::DoNotOptimize(total);

    if (std::abs(checkTotal - total) > 0.0001) {
      std::string msg = "Wrong calculation: error is " + std::to_string(std::abs(checkTotal - total));
      state.SkipWithError(msg.c_str());
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Par(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double checkTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double total = std::reduce(std::execution::unseq, vec.begin(), vec.end(), 0.0, [](double sum, double arg) { return sum + std::tan(arg); });
    benchmark::DoNotOptimize(total);

    if (std::abs(checkTotal - total) > 0.0001) {
      std::string msg = "Wrong calculation: error is " + std::to_string(std::abs(checkTotal - total));
      state.SkipWithError(msg.c_str());
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Par_Unseq(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double checkTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double total = std::reduce(std::execution::par_unseq, vec.begin(), vec.end(), 0.0, [](double sum, double arg) { return sum + std::tan(arg); });
    benchmark::DoNotOptimize(total);

    if (std::abs(checkTotal - total) > 0.0001) {
      std::string msg = "Wrong calculation: error is " + std::to_string(std::abs(checkTotal - total));
      state.SkipWithError(msg.c_str());
    }
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Manual)->Range(8, 8 << 17)->UseRealTime()->Complexity();
BENCHMARK(BM_Seq)->Range(8, 8 << 17)->UseRealTime()->Complexity();
BENCHMARK(BM_Par)->Range(8, 8 << 17)->UseRealTime()->Complexity();
BENCHMARK(BM_Par_Unseq)->Range(8, 8 << 17)->UseRealTime()->Complexity();
