#include "IddEnums_constexpr.hpp"

#include <array>
#include <cassert>
#include <unordered_map>

consteval void static_test() {

  {
    constexpr constexpr_enum::IddObjectType iddObjectType(1);
    static_assert(iddObjectType.value() == 1);
    static_assert(iddObjectType.value() == constexpr_enum::IddObjectType::UserCustom);
    static_assert(iddObjectType.valueName() == "UserCustom");
    static_assert(iddObjectType.valueDescription() == "UserCustom");
  }
  {
    constexpr constexpr_enum::IddObjectType iddObjectType(constexpr_enum::IddObjectType::SimulationData);
    static_assert(iddObjectType.value() == 4);
    static_assert(iddObjectType.value() == constexpr_enum::IddObjectType::SimulationData);
    static_assert(iddObjectType.valueName() == "SimulationData");
    static_assert(iddObjectType.valueDescription() == "Simulation Data");
  }
  {
    static_assert(constexpr_enum::IddObjectType::lookupValue("SimuLatIONData") == 4);
    constexpr constexpr_enum::IddObjectType iddObjectType("SimuLatIONData");
    static_assert(iddObjectType.value() == 4);
    static_assert(iddObjectType.value() == constexpr_enum::IddObjectType::SimulationData);
    static_assert(iddObjectType.valueName() == "SimulationData");
    static_assert(iddObjectType.valueDescription() == "Simulation Data");

    static_assert(constexpr_enum::IddObjectType::lookupValue("SimuLatIONData") == 4);

    // This is the longest one
    static_assert(constexpr_enum::IddObjectType::lookupValue("OS:AirConditioner:VariableRefrigerantFlow:FluidTemperatureControl:LoadingIndex") > 1);
    // static_assert(constexpr_enum::IddObjectType::lookupValue("BOGUS_VALUE") == 4);

    constexpr auto values = constexpr_enum::IddObjectType::getValues();
    static_assert(values.front() == 0);
    static_assert(values[1] == 1);
    static_assert(values[2] == 2);
    static_assert(values.back() == constexpr_enum::IddObjectType::Size - 1);
    static_assert(values.size() == static_cast<size_t>(constexpr_enum::IddObjectType::Size));

    constexpr bool b = []() {
      for (const auto& name : constexpr_enum::IddObjectType::names) {
        if (constexpr_enum::IddObjectType::lookupValue(name) < 0) {
          return false;
        }
      }
      return true;
    }();
    static_assert(b, "At least one name cant be looked up");
  }
}

void runtime_test() {
  std::string name = "OS:AIRCONDITIONER:VARIABLEREFRIGERANTFLOW:FluidTemperatureControl:LoadingIndex";
  auto index = constexpr_enum::IddObjectType::lookupValue(name);
  fmt::print("'{}' has index {}\n", name, index);
  for (const auto& name : constexpr_enum::IddObjectType::names) {
    if (constexpr_enum::IddObjectType::lookupValue(name) < 0) {
      throw std::runtime_error(fmt::format("Failed for {}", name));
    }
  }
  for (const auto& name : constexpr_enum::IddObjectType::namesUC) {
    if (constexpr_enum::IddObjectType::lookupValue(name) < 0) {
      throw std::runtime_error(fmt::format("Failed for {}", name));
    }
  }
  for (const auto& name : constexpr_enum::IddObjectType::descriptions) {
    if (constexpr_enum::IddObjectType::lookupValue(name) < 0) {
      throw std::runtime_error(fmt::format("Failed for {}", name));
    }
  }
  for (const auto& name : constexpr_enum::IddObjectType::descriptionsUC) {
    if (constexpr_enum::IddObjectType::lookupValue(name) < 0) {
      throw std::runtime_error(fmt::format("Failed for {}", name));
    }
  }
  for (int value : constexpr_enum::IddObjectType::getValues()) {
    auto iddObject = constexpr_enum::IddObjectType(value);
    if (iddObject.value() != value) {
      throw std::runtime_error(fmt::format("Failed for {}", value));
    }
  }
}

int main() {
  runtime_test();
}
