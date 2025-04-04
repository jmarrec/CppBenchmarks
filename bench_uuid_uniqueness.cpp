#include <benchmark/benchmark.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/thread/tss.hpp>
#include <boost/optional.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/functional/hash.hpp>

#include <unordered_set>
using UUID = boost::uuids::uuid;
using Handle = UUID;

UUID random_generate() {
  static boost::thread_specific_ptr<boost::uuids::random_generator> gen;

  if (gen.get() == nullptr) {
    gen.reset(new boost::uuids::random_generator);
  }

  return UUID((*gen)());
}

static void BM_Current(benchmark::State& state) {
  std::vector<UUID> handles;
  handles.reserve(state.range(0));
  for (auto i = 0; i <= state.range(0); ++i) {
    handles.emplace_back(random_generate());
  }

  [[maybe_unused]] bool result = true;

  for (auto _ : state) {
    std::set<UUID> setToCheckUniqueness;
    for (const Handle& h : handles) {
      auto insertResult = setToCheckUniqueness.insert(h);
      if (!insertResult.second) {
        result = false;
      }
    }
  }
}

static void BM_set_size(benchmark::State& state) {
  std::vector<UUID> handles;
  handles.reserve(state.range(0));
  for (auto i = 0; i <= state.range(0); ++i) {
    handles.emplace_back(random_generate());
  }

  [[maybe_unused]] bool result = true;

  for (auto _ : state) {
    std::set<UUID> setToCheckUniqueness(handles.begin(), handles.end());
    result = setToCheckUniqueness.size() == handles.size();
  }
}

static void BM_flat_set_size(benchmark::State& state) {
  std::vector<UUID> handles;
  handles.reserve(state.range(0));
  for (auto i = 0; i <= state.range(0); ++i) {
    handles.emplace_back(random_generate());
  }

  [[maybe_unused]] bool result = true;

  for (auto _ : state) {
    boost::container::flat_set<UUID> setToCheckUniqueness(handles.begin(), handles.end());
    result = setToCheckUniqueness.size() == handles.size();
  }
}

static void BM_unordered_set_size(benchmark::State& state) {
  std::vector<UUID> handles;
  handles.reserve(state.range(0));
  for (auto i = 0; i <= state.range(0); ++i) {
    handles.emplace_back(random_generate());
  }

  [[maybe_unused]] bool result = true;

  for (auto _ : state) {
    std::unordered_set<UUID, boost::hash<boost::uuids::uuid>> setToCheckUniqueness(handles.begin(), handles.end());
    result = setToCheckUniqueness.size() == handles.size();
  }
}

BENCHMARK(BM_Current)->RangeMultiplier(8)->Range(8, 8 << 15)->Unit(benchmark::kMillisecond)->Complexity();
BENCHMARK(BM_set_size)->RangeMultiplier(8)->Range(8, 8 << 15)->Unit(benchmark::kMillisecond)->Complexity();
BENCHMARK(BM_flat_set_size)->RangeMultiplier(8)->Range(8, 8 << 15)->Unit(benchmark::kMillisecond)->Complexity();
BENCHMARK(BM_unordered_set_size)->RangeMultiplier(8)->Range(8, 8 << 15)->Unit(benchmark::kMillisecond)->Complexity();
