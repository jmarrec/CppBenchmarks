#include <benchmark/benchmark.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/thread/tss.hpp>
#include <boost/optional.hpp>

typedef boost::uuids::uuid UUID;

UUID random_generate() {
  static boost::thread_specific_ptr<boost::uuids::random_generator> gen;

  if (gen.get() == nullptr) {
    gen.reset(new boost::uuids::random_generator);
  }

  return UUID((*gen)());
}

bool byRefUUID(const UUID& uid) {
  bool result = uid > UUID();
  return result;
}

bool byValUUID(UUID uid) {
  bool result = uid > UUID();
  return result;
}

static void BM_ByRefUUID(benchmark::State& state) {
  UUID uid = random_generate();

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      bool x = byRefUUID(uid);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}

static void BM_ByValUUID(benchmark::State& state) {
  UUID uid = random_generate();

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      bool x = byValUUID(uid);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}

// Register the function as a benchmark
BENCHMARK(BM_ByValUUID)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_ByRefUUID)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();

double byValOptional(boost::optional<double> x) {
  if (x) {
    return x.get();
  }
  return -999.0;
}

double byRefOptional(const boost::optional<double>& x) {
  if (x) {
    return x.get();
  }
  return -999.0;
}

static void BM_ByValOptional(benchmark::State& state) {
  boost::optional<double> x_ = 999.0;

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      double x = byValOptional(x_);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}

static void BM_ByRefOptional(benchmark::State& state) {
  boost::optional<double> x_ = 999.0;

  for (auto _ : state) {
    for (auto i = 0; i <= state.range(0); ++i) {
      double x = byRefOptional(x_);
      benchmark::DoNotOptimize(x);
    }
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_ByValOptional)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
BENCHMARK(BM_ByRefOptional)->RangeMultiplier(2)->Range(8, 8 << 10)->Complexity();
