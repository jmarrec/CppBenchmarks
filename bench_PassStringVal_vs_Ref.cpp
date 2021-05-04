#include <benchmark/benchmark.h>

#include <string>

namespace current {
  bool passX_Impl([[maybe_unused]] std::string x) {
    return true;
  }

  bool passX(std::string x) {
    return passX_Impl(x);
  }
}

static void StringByVal(benchmark::State& state) {

  // std::string testString = "Air Terminal Single Duct Constant Volume No Reheat 1";

  // std::string s(5, 'a'); // s == "aaaaa"
  std::string testString(state.range(0), 'a');
  for (auto _ : state) {
    bool ret = current::passX(testString);
    benchmark::DoNotOptimize(ret);
  }
  state.SetComplexityN(state.range(0));
}



namespace proposed {
  bool passX_Impl([[maybe_unused]] const std::string& x) {
    return true;
  }

  bool passX(const std::string& x) {
    return passX_Impl(x);
  }
}

static void StringByRef(benchmark::State& state) {
  //  std::string testString = "Air Terminal Single Duct Constant Volume No Reheat 1";
  std::string testString(state.range(0), 'a');

  for (auto _ : state) {
    bool ret = proposed::passX(testString);
    benchmark::DoNotOptimize(ret);
  }
  state.SetComplexityN(state.range(0));

}

BENCHMARK(StringByVal)
  ->Range(8, 8<<10)
  ->Complexity();

BENCHMARK(StringByRef)
  ->Range(8, 8<<10)
  ->Complexity();
