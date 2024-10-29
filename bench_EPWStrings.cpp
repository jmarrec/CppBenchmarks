#include <benchmark/benchmark.h>
#include <string>
#include <vector>

struct EPWString
{
  const int lineNumber;
  const int year;
  const int month;
  const int day;
  const int hour;
  const int currentMinute;
  std::vector<std::string> strings;
};

static void PushBack(benchmark::State& state) {

  std::vector<EPWString> epw_strings;

  int lineNumber = 1;
  int year = 2023;
  int month = 1;
  int day = 1;
  int hour = 1;
  int currentMinute = 0;
  std::vector<std::string> strings{"hello", "world"};

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      EPWString epw_s = {lineNumber, year, month, day, hour, currentMinute, strings};
      epw_strings.push_back(epw_s);
    }
  }
}

static void PushBack2(benchmark::State& state) {

  std::vector<EPWString> epw_strings;

  int lineNumber = 1;
  int year = 2023;
  int month = 1;
  int day = 1;
  int hour = 1;
  int currentMinute = 0;
  std::vector<std::string> strings{"hello", "world"};

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      epw_strings.push_back({lineNumber, year, month, day, hour, currentMinute, strings});
    }
  }
}

static void EmplaceBack(benchmark::State& state) {

  std::vector<EPWString> epw_strings;

  int lineNumber = 1;
  int year = 2023;
  int month = 1;
  int day = 1;
  int hour = 1;
  int currentMinute = 0;
  std::vector<std::string> strings{"hello", "world"};

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      epw_strings.emplace_back(lineNumber, year, month, day, hour, currentMinute, strings);
    }
  }
}

static void EmplaceBackMoveStrings(benchmark::State& state) {

  std::vector<EPWString> epw_strings;

  int lineNumber = 1;
  int year = 2023;
  int month = 1;
  int day = 1;
  int hour = 1;
  int currentMinute = 0;
  std::vector<std::string> strings{"hello", "world"};

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      epw_strings.emplace_back(lineNumber, year, month, day, hour, currentMinute, std::move(strings));
    }
  }
}

static void EmplaceBackReserve(benchmark::State& state) {

  std::vector<EPWString> epw_strings;

  int lineNumber = 1;
  int year = 2023;
  int month = 1;
  int day = 1;
  int hour = 1;
  int currentMinute = 0;
  std::vector<std::string> strings{"hello", "world"};

  epw_strings.reserve(8760);

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      epw_strings.emplace_back(lineNumber, year, month, day, hour, currentMinute, std::move(strings));
    }
  }
}

static constexpr int64_t n_regular = 8760;
static constexpr int64_t n_30min = n_regular * 2;
static constexpr int64_t n_15min = n_regular * 4;
static constexpr int64_t n_10min = n_regular * 6;
static constexpr int64_t n_two_years_15min = n_15min * 2;

// Register the function as a benchmark
BENCHMARK(PushBack)->Arg(n_regular)->Arg(n_30min)->Arg(n_15min)->Arg(n_10min)->Arg(n_two_years_15min)->Unit(benchmark::kMillisecond)->Complexity();
BENCHMARK(PushBack2)->Arg(n_regular)->Arg(n_30min)->Arg(n_15min)->Arg(n_10min)->Arg(n_two_years_15min)->Unit(benchmark::kMillisecond)->Complexity();
BENCHMARK(EmplaceBack)->Arg(n_regular)->Arg(n_30min)->Arg(n_15min)->Arg(n_10min)->Arg(n_two_years_15min)->Unit(benchmark::kMillisecond)->Complexity();
BENCHMARK(EmplaceBackMoveStrings)
  ->Arg(n_regular)
  ->Arg(n_30min)
  ->Arg(n_15min)
  ->Arg(n_10min)
  ->Arg(n_two_years_15min)
  ->Unit(benchmark::kMillisecond)
  ->Complexity();
BENCHMARK(EmplaceBackReserve)
  ->Arg(n_regular)
  ->Arg(n_30min)
  ->Arg(n_15min)
  ->Arg(n_10min)
  ->Arg(n_two_years_15min)
  ->Unit(benchmark::kMillisecond)
  ->Complexity();
