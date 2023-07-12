#include <benchmark/benchmark.h>

#include "IddEnums_constexpr.hpp"

#include <array>
#include <unordered_map>

void static_test() {

  {
    constexpr constexpr_enum::IddObjectType i(1);
    static_assert(i.value() == 1);
    static_assert(i.value() == constexpr_enum::IddObjectType::UserCustom);
    static_assert(i.valueName() == "UserCustom");
    static_assert(i.valueDescription() == "UserCustom");
  }
  {
    constexpr constexpr_enum::IddObjectType i(constexpr_enum::IddObjectType::SimulationData);
    static_assert(i.value() == 4);
    static_assert(i.value() == constexpr_enum::IddObjectType::SimulationData);
    static_assert(i.valueName() == "SimulationData");
    static_assert(i.valueDescription() == "Simulation Data");
  }
  {
    static_assert(constexpr_enum::IddObjectType::lookupValue("SimuLatIONData") == 4);
    constexpr constexpr_enum::IddObjectType i("SimuLatIONData");
    static_assert(i.value() == 4);
    static_assert(i.value() == constexpr_enum::IddObjectType::SimulationData);
    static_assert(i.valueName() == "SimulationData");
    static_assert(i.valueDescription() == "Simulation Data");

    static_assert(constexpr_enum::IddObjectType::lookupValue("SimuLatIONData") == 4);
  }
}
