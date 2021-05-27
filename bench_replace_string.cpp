#include <benchmark/benchmark.h>

#include <string>
#include <boost/algorithm/string/replace.hpp>


std::string replace(std::string input, const std::string& before, const std::string& after) {
  std::size_t loc = 0;

  while (true) {
    loc = input.find(before, loc);

    if (loc == std::string::npos) {
      break;
    }

    input.replace(loc, before.size(), after);
    loc += after.size();
  }

  return input;
}

std::string replaceNew(std::string input, const std::string& before, const std::string& after) {
  boost::replace_all(input, before, after);
  return input;
}

std::string makeTestStr(size_t len) {
  constexpr std::string_view testStr = "def CreateBaselineBuilding \r\nWith some newlines and co.\n";

  auto result = std::string(len, '\0');
  std::generate_n(begin(result), len, [&testStr, i = 0] () mutable { return testStr[i++ %  testStr.size()]; });
  // fmt::print("{}, {}", result.size(), result);
  return result;
}



static void BM_manual(benchmark::State& state) {

  std::string input = makeTestStr(state.range(0));
  std::string before = "CreateBaselineBuilding";
  std::string after = "CreateBaselineBuildingCopy";

  for (auto _ : state) {
    auto s = replace(input, before, after);
    benchmark::DoNotOptimize(s);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_replaceNew(benchmark::State& state) {

  std::string input = makeTestStr(state.range(0));
  std::string before = "CreateBaselineBuilding";
  std::string after = "CreateBaselineBuildingCopy";

  for (auto _ : state) {
    auto s = replaceNew(input, before, after);
    benchmark::DoNotOptimize(s);
  }

  state.SetComplexityN(state.range(0));
}

static void BM_boost_replace_all_copy_direct(benchmark::State& state) {

  std::string input = makeTestStr(state.range(0));
  std::string before = "CreateBaselineBuilding";
  std::string after = "CreateBaselineBuildingCopy";

  for (auto _ : state) {
    auto s = boost::replace_all_copy(input, before, after);
    benchmark::DoNotOptimize(s);
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_manual)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(BM_replaceNew)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(BM_boost_replace_all_copy_direct)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
