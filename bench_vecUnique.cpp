#include <benchmark/benchmark.h>

#include <algorithm>
#include <unordered_set>
#include <set>
#include <vector>

std::vector<int> getVec() {
  std::vector<int> vec = {1, 2, 3, 4, 5, 6, 6, 5, 4, 3, 2, 1, 1, 1, 1, 2, 3, 4, 5};
  return vec;
}

static void SortUnique(benchmark::State& state) {

  for (auto _ : state) {
    auto v = getVec();
    std::sort(v.begin(), v.end());
    unsigned uniqueCount = std::distance(v.begin(), std::unique(v.begin(), v.end()));
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(uniqueCount);
  }
}
// Register the function as a benchmark
BENCHMARK(SortUnique);

static void UnorderedSet(benchmark::State& state) {

  for (auto _ : state) {
    auto v = getVec();
    std::sort(v.begin(), v.end());
    unsigned uniqueCount = std::unique(v.begin(), v.end()) - v.begin();
    // Make sure the variable is not optimized away by compiler
    benchmark::DoNotOptimize(uniqueCount);
  }
}
// Register the function as a benchmark
BENCHMARK(UnorderedSet);

static void Set(benchmark::State& state) {

  for (auto _ : state) {
    auto v = getVec();
    std::set<int> set(v.cbegin(), v.cend());
    unsigned uniqueCount = set.size();
    benchmark::DoNotOptimize(uniqueCount);
  }
}
BENCHMARK(Set);

