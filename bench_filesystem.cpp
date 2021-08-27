#include <benchmark/benchmark.h>

#include <array>
#include <string_view>
#include <vector>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


const std::vector<fs::path>& approvedFoldersMeyers() {
  static const std::vector<fs::path> result{
    // fs::path{"docs"}, fs::path{"resources"}, fs::path{"tests"}
    "docs", "resources", "tests"
  };
  return result;
}

static constexpr std::array<std::string_view, 3> approvedFolders
//static constexpr const char* const approvedFolders[] =
{{
  "docs", "resources", "tests"
 }};

static void BM_Static(benchmark::State& state) {

  fs::path srcDir = fs::canonical(".");

  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {
      for(const auto& approvedPath : approvedFoldersMeyers()) {
        fs::path p = srcDir / approvedPath;
        benchmark::DoNotOptimize(p);
      }
    }
  }

  state.SetComplexityN(state.range(0));

}

static void BM_ConstExpr(benchmark::State& state) {

  fs::path srcDir = fs::canonical(".");

  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {

      for(const auto& approvedStrView : approvedFolders) {
        fs::path p = srcDir / fs::path{std::string(approvedStrView)};
        benchmark::DoNotOptimize(p);
      }
    }
  }

  state.SetComplexityN(state.range(0));

}

BENCHMARK(BM_Static)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();
BENCHMARK(BM_ConstExpr)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity();

