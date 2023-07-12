#include <benchmark/benchmark.h>

#include "IddEnums_constexpr.hpp"

static void BM_Constexpr(benchmark::State& /*state*/) {
  {
    constexpr IddObjectType i(1);
    static_assert(i.value() == 1);
    static_assert(i.value() == IddObjectType::UserCustom);
    static_assert(i.valueName() == "UserCustom");
    static_assert(i.valueDescription() == "UserCustom");
  }
  {
    constexpr IddObjectType i(IddObjectType::SimulationData);
    static_assert(i.value() == 4);
    static_assert(i.value() == IddObjectType::SimulationData);
    static_assert(i.valueName() == "SimulationData");
    static_assert(i.valueDescription() == "Simulation Data");
  }
}

BENCHMARK(BM_Constexpr)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);

