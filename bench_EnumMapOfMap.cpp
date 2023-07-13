#include <benchmark/benchmark.h>

#include <array>
#include <map>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include "IddEnums.hpp"
#include "IddEnums_constexpr.hpp"

#include <frozen/unordered_map.h>
#include <frozen/bits/elsa.h>

static constexpr int64_t N_times_each = 5;

using MapValueType = std::unordered_map<std::string, std::string>;

int random_number(int max_size) {
  std::random_device rd;                                                          //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd());                                                         //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<std::mt19937::result_type> distrib(0, max_size);  // Generates an int in the closed interval [0, n]
  int index = distrib(rd);
  return index;
}

std::string getKey(const auto& iddObjectType) {
  return fmt::format("{}_{}", iddObjectType.valueName(), random_number(2));
}

IddObjectType randomIddObjectType() {
  return {random_number(static_cast<int>(IddObjectType::CommentOnly))};
}

constexpr_enum::IddObjectType randomIddObjectTypeConstexpr() {
  return {random_number(static_cast<int>(constexpr_enum::IddObjectType::Size - 1))};
}

static void BM_Current(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::map<IddObjectType, MapValueType>;
    IddObjectTypeMap m_iddObjectTypeMap;
    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType].insert(std::make_pair(getKey(iddObjectType), iddObjectType.valueName()));
    }
  }

  state.SetComplexityN(state.range(0));
}

struct IddObjectTypeHash
{
  /* constexpr */ std::size_t operator()(IddObjectType const& iddObjectType) const noexcept {
    return std::hash<int>{}(iddObjectType.value());
  }
};

struct IddObjectTypeHashConstexpr
{
  constexpr std::size_t operator()(constexpr_enum::IddObjectType const& iddObjectType) const noexcept {
    // Gcc 10 and 11: this isn't constexpr
    // return std::hash<int>{}(iddObjectType.value());
    return static_cast<size_t>(iddObjectType.value());
  }
};

static void BM_HashMap(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::unordered_map<IddObjectType, MapValueType, IddObjectTypeHash>;
    IddObjectTypeMap m_iddObjectTypeMap;
    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType].insert(std::make_pair(getKey(iddObjectType), iddObjectType.valueName()));
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_HashMapReserve(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::unordered_map<IddObjectType, MapValueType, IddObjectTypeHash>;
    IddObjectTypeMap m_iddObjectTypeMap;
    m_iddObjectTypeMap.reserve(static_cast<int>(IddObjectType::CommentOnly) + 1);

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType].insert(std::make_pair(getKey(iddObjectType), iddObjectType.valueName()));
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Vector(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::vector<MapValueType>;
    IddObjectTypeMap m_iddObjectTypeMap;
    m_iddObjectTypeMap.resize(static_cast<int>(IddObjectType::CommentOnly) + 1);

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType.value()].insert(std::make_pair(getKey(iddObjectType), iddObjectType.valueName()));
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Array(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::array<MapValueType, static_cast<size_t>(IddObjectType::CommentOnly) + 1>;
    IddObjectTypeMap m_iddObjectTypeMap;

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType.value()].insert(std::make_pair(getKey(iddObjectType), iddObjectType.valueName()));
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_HashMapConstexpr(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::unordered_map<constexpr_enum::IddObjectType, MapValueType, IddObjectTypeHashConstexpr>;
    IddObjectTypeMap m_iddObjectTypeMap;
    m_iddObjectTypeMap.reserve(static_cast<int>(constexpr_enum::IddObjectType::Size));

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      const auto iddObjectType = randomIddObjectTypeConstexpr();
      m_iddObjectTypeMap[iddObjectType].insert(std::make_pair(getKey(iddObjectType), iddObjectType.valueName()));
    }
  }

  state.SetComplexityN(state.range(0));
}

struct IddObjectTypeHashFrozen
{
  constexpr std::size_t operator()(constexpr_enum::IddObjectType const& iddObjectType, std::size_t seed) const noexcept {
    return frozen::elsa<>{}(iddObjectType.value(), seed);
  }
};

auto getFrozenHashMap() {
  using Key = constexpr_enum::IddObjectType;
  std::array<std::pair<Key, MapValueType>, constexpr_enum::IddObjectType::Size> init;
  for (constexpr_enum::IddObjectType::SizeType i = 0; i < constexpr_enum::IddObjectType::Size; ++i) {
    init[i] = std::make_pair(constexpr_enum::IddObjectType(i), MapValueType{});
  }
  return frozen::unordered_map<constexpr_enum::IddObjectType, MapValueType, constexpr_enum::IddObjectType::Size, IddObjectTypeHashFrozen>(init);
}

static void BM_HashMapConstexprFrozen(benchmark::State& state) {

  for (auto _ : state) {
    auto m_iddObjectTypeMap = getFrozenHashMap();

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      const auto iddObjectType = randomIddObjectTypeConstexpr();
      m_iddObjectTypeMap.at(iddObjectType).insert(std::make_pair(getKey(iddObjectType), iddObjectType.valueName()));
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_ArrayConstexpr(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::array<MapValueType, static_cast<size_t>(constexpr_enum::IddObjectType::Size)>;
    IddObjectTypeMap m_iddObjectTypeMap;

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      const auto iddObjectType = randomIddObjectTypeConstexpr();
      m_iddObjectTypeMap[iddObjectType.value()].insert(std::make_pair(getKey(iddObjectType), iddObjectType.valueName()));
    }
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Current)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashMap)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashMapReserve)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Vector)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Array)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashMapConstexpr)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashMapConstexprFrozen)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_ArrayConstexpr)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
