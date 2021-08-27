#include <benchmark/benchmark.h>
#include <string_view>

enum struct MyEnum
{
  A,
  B,
  C
};

int byValEnum(MyEnum e) {
  return static_cast<int>(e);
}

int byConstRefEnum(const MyEnum& e) {
  return static_cast<int>(e);
}

static void BM_ByValEnum(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    auto e = MyEnum::A;
    int i = byValEnum(e);
    benchmark::DoNotOptimize(i);
  }
}

static void BM_ByConstRefEnum(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    auto e = MyEnum::A;
    int i = byConstRefEnum(e);
    benchmark::DoNotOptimize(i);
  }
}

// Register the function as a benchmark
BENCHMARK(BM_ByValEnum);
BENCHMARK(BM_ByConstRefEnum);

bool byValStringView(std::string_view e) {
  return e.empty();
}

bool byConstRefStringView(const std::string_view& e) {
  return e.empty();
}

static void BM_ByValStringView(benchmark::State& state) {
  std::string_view sv("hello");

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    bool b = byValStringView(sv);
    benchmark::DoNotOptimize(b);
  }
}

static void BM_ByConstRefStringView(benchmark::State& state) {
  std::string_view sv("hello");

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    bool b = byConstRefStringView(sv);
    benchmark::DoNotOptimize(b);
  }
}

// Register the function as a benchmark
BENCHMARK(BM_ByValStringView);
BENCHMARK(BM_ByConstRefStringView);
