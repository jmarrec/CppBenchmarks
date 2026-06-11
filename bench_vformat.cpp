#include <benchmark/benchmark.h>

#include <array>
#include <format>

using Real64 = double;

std::string RealToStr_vFormat(bool const formatReals, Real64 const RealIn, int const numDigits) {

  static constexpr std::array<const char*, 10> formDigitsA{"{:#11.0F}", "{:12.1F}", "{:12.2F}", "{:12.3F}", "{:12.4F}",
                                                           "{:12.5F}",  "{:12.6F}", "{:12.7F}", "{:12.8F}", "{:12.9F}"};

  static constexpr std::array<Real64, 10> maxvalDigitsA(
    {9999999999.0, 999999999.0, 99999999.0, 9999999.0, 999999.0, 99999.0, 9999.0, 999.0, 99.0, 9.0});

  // FUNCTION LOCAL VARIABLE DECLARATIONS:

  if (!formatReals) {
    // No rounding
    return std::format("{}", RealIn);
  }
  int nDigits = numDigits;
  if (RealIn < 0.0) {
    --nDigits;
  }
  if (nDigits > 9) {
    nDigits = 9;
  }
  if (nDigits < 0) {
    nDigits = 0;
  }

  if (std::abs(RealIn) > maxvalDigitsA.at(nDigits)) {
    return std::format("{:12.6E}", RealIn);
  }
  return std::vformat(formDigitsA.at(nDigits), std::make_format_args(RealIn));
}

std::string RealToStr_explicit(bool const formatReals, Real64 const RealIn, int const numDigits) {

  static constexpr std::array<Real64, 10> maxvalDigitsA(
    {9999999999.0, 999999999.0, 99999999.0, 9999999.0, 999999.0, 99999.0, 9999.0, 999.0, 99.0, 9.0});

  // FUNCTION LOCAL VARIABLE DECLARATIONS:

  if (!formatReals) {
    // No rounding
    return std::format("{}", RealIn);
  }
  int nDigits = numDigits;
  if (RealIn < 0.0) {
    --nDigits;
  }
  if (nDigits > 9) {
    nDigits = 9;
  }
  if (nDigits < 0) {
    nDigits = 0;
  }

  if (std::abs(RealIn) > maxvalDigitsA.at(nDigits)) {
    return std::format("{:12.6E}", RealIn);
  }
  if (nDigits == 0) {
    return std::format("{:#11.0F}", RealIn);
  } else if (nDigits == 1) {
    return std::format("{:12.1F}", RealIn);
  } else if (nDigits == 2) {
    return std::format("{:12.2F}", RealIn);
  } else if (nDigits == 3) {
    return std::format("{:12.3F}", RealIn);
  } else if (nDigits == 4) {
    return std::format("{:12.4F}", RealIn);
  } else if (nDigits == 5) {
    return std::format("{:12.5F}", RealIn);
  } else if (nDigits == 6) {
    return std::format("{:12.6F}", RealIn);
  } else if (nDigits == 7) {
    return std::format("{:12.7F}", RealIn);
  } else if (nDigits == 8) {
    return std::format("{:12.8F}", RealIn);
  } else {
    return std::format("{:12.9F}", RealIn);
  }
}

std::string RealToStr_FormatDynamic(bool const formatReals, Real64 const RealIn, int const numDigits) {

  static constexpr std::array<Real64, 10> maxvalDigitsA(
    {9999999999.0, 999999999.0, 99999999.0, 9999999.0, 999999.0, 99999.0, 9999.0, 999.0, 99.0, 9.0});

  // FUNCTION LOCAL VARIABLE DECLARATIONS:

  if (!formatReals) {
    // No rounding
    return std::format("{}", RealIn);
  }
  int nDigits = numDigits;
  if (RealIn < 0.0) {
    --nDigits;
  }
  if (nDigits > 9) {
    nDigits = 9;
  }
  if (nDigits < 0) {
    nDigits = 0;
  }

  if (std::abs(RealIn) > maxvalDigitsA.at(nDigits)) {
    return std::format("{:12.6E}", RealIn);
  }
  if (nDigits == 0) {
    return std::format("{:#11.0F}", RealIn);
  }
  return std::format("{:12.{}F}", RealIn, numDigits);
}

static void BM_RealToStr_vFormat(benchmark::State& state) {
  for (auto _ : state) {
    for (int nDigits = 0; nDigits < 10; ++nDigits) {
      benchmark::DoNotOptimize(RealToStr_vFormat(true, 1234.56789, nDigits));
    }
  }
}
BENCHMARK(BM_RealToStr_vFormat);

static void BM_RealToStr_explicit(benchmark::State& state) {
  for (auto _ : state) {
    for (int nDigits = 0; nDigits < 10; ++nDigits) {
      benchmark::DoNotOptimize(RealToStr_explicit(true, 1234.56789, nDigits));
    }
  }
}
BENCHMARK(BM_RealToStr_explicit);

static void BM_RealToStr_FormatDynamic(benchmark::State& state) {
  for (auto _ : state) {
    for (int nDigits = 0; nDigits < 10; ++nDigits) {
      benchmark::DoNotOptimize(RealToStr_FormatDynamic(true, 1234.56789, nDigits));
    }
  }
}
BENCHMARK(BM_RealToStr_FormatDynamic);
