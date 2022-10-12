#include <benchmark/benchmark.h>
#include <string_view>
#include <array>

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
  std::string_view sv("hello this is something that is longer than the small string optimization");

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    bool b = byValStringView(sv);
    benchmark::DoNotOptimize(b);
  }
}

static void BM_ByConstRefStringView(benchmark::State& state) {
  std::string_view sv("hello this is something that is longer than the small string optimization");

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    bool b = byConstRefStringView(sv);
    benchmark::DoNotOptimize(b);
  }
}

// Register the function as a benchmark
BENCHMARK(BM_ByValStringView);
BENCHMARK(BM_ByConstRefStringView);

double byValArray2Doubles(std::array<double, 2> Par) {
  return Par[0] + 2 * Par[1];
}

double byConstRefArray2Doubles(const std::array<double, 2>& Par) {
  return Par[0] + 2 * Par[1];
}

static void BM_ByValArray2Doubles(benchmark::State& state) {
  std::array<double, 2> Par{10.0, 20.0};

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double d = byValArray2Doubles(Par);
    benchmark::DoNotOptimize(d);
  }
}

static void BM_ByConstRefArray2Doubles(benchmark::State& state) {
  std::array<double, 2> Par{10.0, 20.0};

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    double d = byConstRefArray2Doubles(Par);
    benchmark::DoNotOptimize(d);
  }
}

// Register the function as a benchmark
BENCHMARK(BM_ByValArray2Doubles);
BENCHMARK(BM_ByConstRefArray2Doubles);
