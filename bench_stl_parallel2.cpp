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

// NOTE: you have to be VERY carful with std::reduce. This looks ok, doesn't it?
// double total = std::reduce(std::execution::seq, vec.begin(), vec.end(), 0.0, [](double sum, double arg) { return sum + std::tan(arg); });
// Well it's **terribly** wrong. Unlike accumulate, the order of the two args to the binary op are unspecified, so you MUST use something that is
// fully associative and commutative
// REDUCE: BinaryOp applies the function in an unspecified order!
// binary_op - binary FunctionObject that will be applied in unspecified order
// to the result of dereferencing the input iterators,
// the results of other binary_op and init.

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

void checkTotal(const double& expectedTotal, const double& total, benchmark::State& state) {
  if (std::abs(expectedTotal - total) > 0.0001) {
    std::string msg = "Wrong calculation: Expected total=" + std::to_string(expectedTotal) + ", got " + std::to_string(total) + ", error is "
                      + std::to_string(std::abs(expectedTotal - total));
    state.SkipWithError(msg.c_str());
  }
}

static void BM_Manual(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double expectedTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double total = calculateTotal(vec);

    checkTotal(expectedTotal, total, state);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Accumulate(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double expectedTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double total = std::accumulate(vec.begin(), vec.end(), 0.0, [](double sum, double arg) { return sum + std::tan(arg); });

    checkTotal(expectedTotal, total, state);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Seq(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double expectedTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    auto workVec = vec;  // That must be an expensive copy
    std::transform(std::execution::seq, workVec.begin(), workVec.end(), workVec.begin(), [](double arg) { return std::tan(arg); });
    double total = std::reduce(std::execution::seq, workVec.begin(), workVec.end(), 0.0);

    checkTotal(expectedTotal, total, state);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Par(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double expectedTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    auto workVec = vec;  // That must be an expensive copy
    std::transform(std::execution::unseq, workVec.begin(), workVec.end(), workVec.begin(), [](double arg) { return std::tan(arg); });
    double total = std::reduce(std::execution::seq, workVec.begin(), workVec.end(), 0.0);

    checkTotal(expectedTotal, total, state);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Par_Unseq(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double expectedTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    auto workVec = vec;  // That must be an expensive copy
    std::transform(std::execution::par_unseq, workVec.begin(), workVec.end(), workVec.begin(), [](double arg) { return std::tan(arg); });
    double total = std::reduce(std::execution::seq, workVec.begin(), workVec.end(), 0.0);

    checkTotal(expectedTotal, total, state);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Par_Unseq_Transform_Reduce(benchmark::State& state) {
  auto vec = makeVector(state.range(0));
  double expectedTotal = calculateTotal(vec);

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double total = std::transform_reduce(std::execution::par_unseq, vec.begin(), vec.end(), 0.0,
                                         // Reduce operation:
                                         std::plus<>(),
                                         // transform operation
                                         [](double arg) { return std::tan(arg); });

    checkTotal(expectedTotal, total, state);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Manual)->Range(8, 8 << 17)->UseRealTime()->Complexity();
BENCHMARK(BM_Accumulate)->Range(8, 8 << 17)->UseRealTime()->Complexity();
BENCHMARK(BM_Seq)->Range(8, 8 << 17)->UseRealTime()->Complexity();
BENCHMARK(BM_Par)->Range(8, 8 << 17)->UseRealTime()->Complexity();
BENCHMARK(BM_Par_Unseq)->Range(8, 8 << 17)->UseRealTime()->Complexity();
BENCHMARK(BM_Par_Unseq_Transform_Reduce)->Range(8, 8 << 17)->UseRealTime()->Complexity();
