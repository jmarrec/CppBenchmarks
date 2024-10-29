#include <benchmark/benchmark.h>

#include <optional>
#include <string>
#include <vector>

std::optional<std::string> get_optional(bool initialized) {
  if (initialized) {
    return "1ce80ed4-cbac-4bdc-8221-111010195b61";
  }
  return {};
}

static void PushBack(benchmark::State& state) {
  size_t i = 0;
  std::vector<std::string> v;
  for (auto _ : state) {
    if (auto optionalS_ = get_optional(i++ % 2 == 0)) {
      v.push_back(optionalS_.value());
    }
  }
}

static void EmplaceBackGet(benchmark::State& state) {
  size_t i = 0;
  std::vector<std::string> v;
  for (auto _ : state) {
    if (auto optionalS_ = get_optional(i++ % 2 == 0)) {
      v.emplace_back(optionalS_.value());
    }
  }
}

static void EmplaceBackDeref(benchmark::State& state) {
  size_t i = 0;
  std::vector<std::string> v;
  for (auto _ : state) {
    if (auto optionalS_ = get_optional(i++ % 2 == 0)) {
      v.emplace_back(*optionalS_);
    }
  }
}

static void EmplaceBackDerefMove(benchmark::State& state) {
  size_t i = 0;
  std::vector<std::string> v;
  for (auto _ : state) {
    if (auto optionalS_ = get_optional(i++ % 2 == 0)) {
      v.emplace_back(std::move(*optionalS_));
    }
  }
}

// Register the function as a benchmark
BENCHMARK(PushBack);
BENCHMARK(EmplaceBackGet);
BENCHMARK(EmplaceBackDeref);
BENCHMARK(EmplaceBackDerefMove);
