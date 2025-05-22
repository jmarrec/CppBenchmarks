#include <benchmark/benchmark.h>

#include <algorithm>
#include <regex>
#include <ranges>
#include <string>

#include <ctre.hpp>

std::string get_string() {
  return "This is my display Name which has spaces";
}

static void Regex(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = get_string();
    str = std::regex_replace(str, std::regex(" "), "_");
    benchmark::DoNotOptimize(str);
  }
}

static void StaticRegex(benchmark::State& state) {
  static const std::regex spaceRegex(" ");

  for (auto _ : state) {
    std::string str = get_string();
    str = std::regex_replace(str, spaceRegex, "_");
    benchmark::DoNotOptimize(str);
  }
}

static void Replace(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = get_string();
    std::replace(str.begin(), str.end(), ' ', '_');
    benchmark::DoNotOptimize(str);
  }
}

static constexpr auto pattern = ctll::fixed_string{" "};
static void CTRE(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = get_string();
    std::string result;
    result.reserve(str.size());
    bool first = true;
    for (auto match : ctre::split<pattern>(str)) {
      if (!first) {
        result += "_";
      } else {
        first = false;
      }
      result += std::string{match.get<0>()};
    }
    benchmark::DoNotOptimize(result);
  }
}

// Register the function as a benchmark
BENCHMARK(Regex);
BENCHMARK(StaticRegex);
BENCHMARK(Replace);
BENCHMARK(CTRE);

//#if __cplusplus >= 202302L
static void CTRERanges(benchmark::State& state) {
  for (auto _ : state) {
    std::string str = get_string();
    auto r = ctre::split<pattern>(str);
    // C++26
    // str = r | std::views::join_with('_') | std::ranges::to<std::string>();
    std::string result;
    result.reserve(str.size());
    std::ranges::copy(r | std::views::join_with('_'), std::back_inserter(result));
    benchmark::DoNotOptimize(str);
  }
}
BENCHMARK(CTRERanges);
//#endif
