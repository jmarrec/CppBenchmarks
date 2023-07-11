#include <benchmark/benchmark.h>

#include <array>
#include <map>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>
#include "IddEnums.hpp"

static constexpr int64_t N_times_each = 5;

int random_number(int max_size) {
  std::random_device rd;                                                              //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd());                                                             //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<std::mt19937::result_type> distrib(0, max_size - 1);  // Generates an int in the closed interval [0, n-1]
  int index = distrib(rd);
  return index;
}

IddObjectType randomIddObjectType() {
  return {random_number(static_cast<int>(IddObjectType::CommentOnly))};
}

static void BM_Current(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::map<IddObjectType, std::string>;
    IddObjectTypeMap m_iddObjectTypeMap;
    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType] = iddObjectType.valueName();
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

static void BM_HashMap(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::unordered_map<IddObjectType, std::string, IddObjectTypeHash>;
    IddObjectTypeMap m_iddObjectTypeMap;
    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType] = iddObjectType.valueName();
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_HashMapReserve(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::unordered_map<IddObjectType, std::string, IddObjectTypeHash>;
    IddObjectTypeMap m_iddObjectTypeMap;
    m_iddObjectTypeMap.reserve(static_cast<int>(IddObjectType::CommentOnly));

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType] = iddObjectType.valueName();
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Vector(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::vector<std::string>;
    IddObjectTypeMap m_iddObjectTypeMap;
    m_iddObjectTypeMap.resize(static_cast<int>(IddObjectType::CommentOnly));

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType.value()] = iddObjectType.valueName();
    }
  }

  state.SetComplexityN(state.range(0));
}

static void BM_Array(benchmark::State& state) {

  for (auto _ : state) {
    using IddObjectTypeMap = std::array<std::string, static_cast<size_t>(IddObjectType::CommentOnly)>;
    IddObjectTypeMap m_iddObjectTypeMap;

    for (int64_t i = 0; i < N_times_each * state.range(0); ++i) {
      IddObjectType iddObjectType = randomIddObjectType();
      m_iddObjectTypeMap[iddObjectType.value()] = iddObjectType.valueName();
    }
  }

  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_Current)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashMap)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_HashMapReserve)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Vector)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Array)->RangeMultiplier(4)->Range(8, 8 << 16)->Complexity()->Unit(benchmark::kMillisecond);
