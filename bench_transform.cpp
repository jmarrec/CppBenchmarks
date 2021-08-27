#include <benchmark/benchmark.h>

#include <algorithm>
#include <string>

std::vector<std::string> makeVector(int64_t len) {
  return std::vector<std::string>(len, "something something something");
}

static void BM_NoReserve(benchmark::State& state) {

  std::vector<std::string> vectorOfStrings = makeVector(state.range(0));

  for (auto _ : state) {

    std::vector<std::string> result;
    std::transform(vectorOfStrings.cbegin(), vectorOfStrings.cend(), std::back_inserter(result),
                   [](const auto& s) { return s; });

  }
  state.SetComplexityN(state.range(0));
}

static void BM_Reserve(benchmark::State& state) {

  std::vector<std::string> vectorOfStrings = makeVector(state.range(0));

  for (auto _ : state) {

    std::vector<std::string> result;
    result.reserve(vectorOfStrings.size());
    std::transform(vectorOfStrings.cbegin(), vectorOfStrings.cend(), std::back_inserter(result),
                   [](const auto& s) { return s; });

  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_NoReserve)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(BM_Reserve)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
