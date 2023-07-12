#include <benchmark/benchmark.h>

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

bool isKeyRegexLike(std::string_view key) {
  return key.find_first_of("()*+?()|[]\\") != std::string_view::npos;
}

bool isKeyRegexLikeOri(std::string_view key) {
  bool is_simple_string = true;
  for (auto const& c : key) {
    if (c == ' ' || c == '_' || (std::isalnum(c) != 0)) {
      continue;
    }
    is_simple_string = false;
    break;
  }
  return !is_simple_string;
}

static void is_simple_string(benchmark::State& state) {
  std::vector<std::string> test_strings{
    "This is the first one",
    "This is another.*one",
    "This is (a|some) ones",
  };

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    int count = 0;
    for (const auto& test_string : test_strings) {
      if (!isKeyRegexLikeOri(test_string)) {
        ++count;
      }
    }
    assert(count == 2);
    benchmark::DoNotOptimize(count);
  }
}

static void keyRegexLike(benchmark::State& state) {

  std::vector<std::string> test_strings{
    "This is the first one",
    "This is another.*one",
    "This is (a|some) ones",
  };

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    int count = 0;
    for (const auto& test_string : test_strings) {
      if (isKeyRegexLikeOri(test_string)) {
        ++count;
      }
    }
    assert(count == 2);
    benchmark::DoNotOptimize(count);
  }
}

BENCHMARK(is_simple_string);
BENCHMARK(keyRegexLike);
