#include <benchmark/benchmark.h>

#include <memory>
#include <string>
#include <vector>

static constexpr size_t numSPMs = 50;

struct SPMBase
{
  std::string name;
  virtual ~SPMBase() = default;
  virtual void calculate() = 0;
};

struct SPMScheduled : SPMBase  // Derived type for Scheduled Setpoint Manager data
{
  int schedNum = 0;

  void calculate() {
    ++schedNum;
  }
};

struct SPMScheduledDual : SPMBase  // Derived type for Scheduled Dual Setpoint Manager
{
  int schedNumHi = 0;
  int schedNumLo = 0;

  void calculate() {
    ++schedNumLo;
  }
};

static void BM_RawPtr(benchmark::State& state) {
  for (auto _ : state) {
    // Array1D lookalike
    std::vector<SPMBase*> spms;
    spms.resize(numSPMs);
    for (size_t i = 0; i < numSPMs; ++i) {
      if (i % 2 == 0) {
        spms.at(i) = new SPMScheduled();
      } else {
        spms.at(i) = new SPMScheduledDual();
      }
    }

    for (auto i = 0; i <= state.range(0); ++i) {
      for (auto& spm : spms) {
        spm->calculate();
      }
    }

    for (size_t i = 0; i < numSPMs; ++i) {
      delete spms[i];
    }
  }
  state.SetComplexityN(state.range(0));
}

static void BM_UniquePtr(benchmark::State& state) {
  for (auto _ : state) {
    std::vector<std::unique_ptr<SPMBase>> spms;
    spms.resize(numSPMs);
    for (size_t i = 0; i < numSPMs; ++i) {
      if (i % 2 == 0) {
        spms.at(i) = std::make_unique<SPMScheduled>();
      } else {
        spms.at(i) = std::make_unique<SPMScheduledDual>();
      }
    }

    for (auto i = 0; i <= state.range(0); ++i) {
      for (auto& spm : spms) {
        spm->calculate();
      }
    }
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_RawPtr)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_UniquePtr)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
