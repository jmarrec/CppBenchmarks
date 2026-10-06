// Does array alignment matter, with and without native optimizations + fast math?
//
// Context: EnergyPlus builds ObjexxFCL arrays with OBJEXXFCL_ALIGN=64 (first element aligned to 64 bytes), a setting from
// ObjexxFCL 4.1 (NatLabRockies/EnergyPlus#5395) meant to help vectorization. Replacing ObjexxFCL arrays by std::vector /
// std::array drops that to the default allocator alignment (16 bytes on 64-bit platforms in practice). And
// NatLabRockies/EnergyPlus#11816 adds an opt-in -march/-mcpu=native + -ffast-math build, which is when vectorization (and
// so maybe alignment) can actually kick in.
//
// The same kernels run on the same data held in:
//   - std::vector<double> (default allocator)
//   - std::vector<double, AlignedAllocator<64>> (like OBJEXXFCL_ALIGN=64, the x86 cache line)
//   - std::vector<double, AlignedAllocator<128>> (Apple Silicon cache line)
//   - a 64-byte aligned buffer viewed from its second element (data + 8 bytes): deliberately misaligned, worst case
// Kernels take a std::span<double>, so the compiler cannot know the alignment, like EnergyPlus going through
// Array::operator(). The "AssumeAligned64" variants use std::assume_aligned<64> to show what an explicit hint gives.
//
// This file is built twice by CMakeLists.txt:
//   bench_alignment_vectorization        : EnergyPlus-like default flags (-O3 -ffp-contract=off)
//   bench_alignment_vectorization_native : -march=native / -mcpu=native -ffast-math -fno-finite-math-only (like #11816)
// Sizes: 1 KiB-ish (L1), 256 KiB (L2), 32 MiB (memory bound).

#include <benchmark/benchmark.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <span>
#include <vector>

template <typename T, std::size_t Alignment>
struct AlignedAllocator
{
  using value_type = T;
  static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be a power of 2");

  AlignedAllocator() noexcept = default;
  template <typename U>
  explicit AlignedAllocator(const AlignedAllocator<U, Alignment>& /*other*/) noexcept {}

  template <typename U>
  struct rebind
  {
    using other = AlignedAllocator<U, Alignment>;
  };

  [[nodiscard]] T* allocate(std::size_t n) {
    return static_cast<T*>(::operator new(n * sizeof(T), std::align_val_t{Alignment}));
  }

  void deallocate(T* p, std::size_t /*n*/) noexcept {
    ::operator delete(p, std::align_val_t{Alignment});
  }

  friend bool operator==(const AlignedAllocator& /*lhs*/, const AlignedAllocator& /*rhs*/) noexcept {
    return true;
  }
};

// Holds the buffer(s) for one benchmark run, and exposes them as spans with the requested alignment
enum class Layout
{
  Default,
  Aligned64,
  Aligned128,
  Misaligned8,  // 64-byte aligned buffer, viewed from its second element
};

class Buffer
{
 public:
  Buffer(Layout layout, std::size_t n) {
    switch (layout) {
      case Layout::Default:
        m_default.resize(n);
        m_span = std::span<double>(m_default);
        break;
      case Layout::Aligned64:
        m_a64.resize(n);
        m_span = std::span<double>(m_a64);
        break;
      case Layout::Aligned128:
        m_a128.resize(n);
        m_span = std::span<double>(m_a128);
        break;
      case Layout::Misaligned8:
        m_a64.resize(n + 1);
        m_span = std::span<double>(m_a64.data() + 1, n);
        break;
    }
    // Deterministic, non-trivial values in [0.5, 1.5)
    for (std::size_t i = 0; i < n; ++i) {
      m_span[i] = 0.5 + static_cast<double>((i * 2654435761u) % 1000u) / 1000.0;
    }
  }

  [[nodiscard]] std::span<double> span() const {
    return m_span;
  }

  // Offset of the first element modulo 128 bytes, reported as a counter so the output shows what each layout got
  [[nodiscard]] double offsetMod128() const {
    return static_cast<double>(reinterpret_cast<std::uintptr_t>(m_span.data()) % 128u);
  }

 private:
  std::vector<double> m_default;
  std::vector<double, AlignedAllocator<double, 64>> m_a64;
  std::vector<double, AlignedAllocator<double, 128>> m_a128;
  std::span<double> m_span;
};

// ---------------------------------------------------------------------------------------------------------------------
// Kernels. noinline so the compiler cannot see the allocation, and so cannot deduce the alignment.
// ---------------------------------------------------------------------------------------------------------------------

// Reduction (like ObjexxFCL sum()): only vectorizes when the compiler may reorder additions (fast math)
[[gnu::noinline]] double kernel_sum(std::span<const double> x) {
  double s = 0.0;
  for (double const v : x) {
    s += v;
  }
  return s;
}

[[gnu::noinline]] double kernel_dot(std::span<const double> x, std::span<const double> y) {
  double s = 0.0;
  for (std::size_t i = 0; i < x.size(); ++i) {
    s += x[i] * y[i];
  }
  return s;
}

// y += a * x: element-wise, vectorizes even without fast math (no reordering needed)
[[gnu::noinline]] void kernel_axpy(double a, std::span<const double> x, std::span<double> y) {
  for (std::size_t i = 0; i < x.size(); ++i) {
    y[i] += a * x[i];
  }
}

// Cubic curve evaluation, like a Curve:Cubic over many points
[[gnu::noinline]] void kernel_cubic(std::span<const double> x, std::span<double> y) {
  constexpr double c0 = 0.94;
  constexpr double c1 = 0.012;
  constexpr double c2 = -0.0003;
  constexpr double c3 = 0.000002;
  for (std::size_t i = 0; i < x.size(); ++i) {
    double const v = x[i];
    y[i] = c0 + v * (c1 + v * (c2 + v * c3));
  }
}

// 5-point Jacobi stencil on a row-major n x n grid (like a finite difference soil grid), interior points only
[[gnu::noinline]] void kernel_stencil(std::span<const double> in, std::span<double> out, std::size_t n) {
  for (std::size_t r = 1; r + 1 < n; ++r) {
    for (std::size_t c = 1; c + 1 < n; ++c) {
      std::size_t const i = r * n + c;
      out[i] = 0.2 * (in[i] + in[i - 1] + in[i + 1] + in[i - n] + in[i + n]);
    }
  }
}

// Same reduction, but promising the compiler a 64-byte aligned start (what ObjexxFCL's ASSUME_ALIGNED_OBJEXXFCL would do)
[[gnu::noinline]] double kernel_sum_assume_aligned64(std::span<const double> x) {
  double const* p = std::assume_aligned<64>(x.data());
  double s = 0.0;
  for (std::size_t i = 0; i < x.size(); ++i) {
    s += p[i];
  }
  return s;
}

[[gnu::noinline]] void kernel_axpy_assume_aligned64(double a, std::span<const double> x, std::span<double> y) {
  double const* px = std::assume_aligned<64>(x.data());
  double* py = std::assume_aligned<64>(y.data());
  for (std::size_t i = 0; i < x.size(); ++i) {
    py[i] += a * px[i];
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// Benchmarks. range(0) = number of doubles.
// ---------------------------------------------------------------------------------------------------------------------

template <Layout L>
static void BM_Sum(benchmark::State& state) {
  auto const n = static_cast<std::size_t>(state.range(0));
  Buffer x(L, n);
  for (auto _ : state) {
    benchmark::DoNotOptimize(kernel_sum(x.span()));
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations() * n * sizeof(double)));
  state.counters["offset_mod_128"] = x.offsetMod128();
}

template <Layout L>
static void BM_Dot(benchmark::State& state) {
  auto const n = static_cast<std::size_t>(state.range(0));
  Buffer x(L, n);
  Buffer y(L, n);
  for (auto _ : state) {
    benchmark::DoNotOptimize(kernel_dot(x.span(), y.span()));
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations() * 2 * n * sizeof(double)));
  state.counters["offset_mod_128"] = x.offsetMod128();
}

template <Layout L>
static void BM_Axpy(benchmark::State& state) {
  auto const n = static_cast<std::size_t>(state.range(0));
  Buffer x(L, n);
  Buffer y(L, n);
  for (auto _ : state) {
    kernel_axpy(1.0e-9, x.span(), y.span());
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations() * 3 * n * sizeof(double)));
  state.counters["offset_mod_128"] = x.offsetMod128();
}

template <Layout L>
static void BM_Cubic(benchmark::State& state) {
  auto const n = static_cast<std::size_t>(state.range(0));
  Buffer x(L, n);
  Buffer y(L, n);
  for (auto _ : state) {
    kernel_cubic(x.span(), y.span());
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations() * 2 * n * sizeof(double)));
  state.counters["offset_mod_128"] = x.offsetMod128();
}

template <Layout L>
static void BM_Stencil(benchmark::State& state) {
  // range(0) is still the number of doubles: use the closest square grid
  auto const side = static_cast<std::size_t>(std::sqrt(static_cast<double>(state.range(0))));
  Buffer in(L, side * side);
  Buffer out(L, side * side);
  for (auto _ : state) {
    kernel_stencil(in.span(), out.span(), side);
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations() * 2 * side * side * sizeof(double)));
  state.counters["offset_mod_128"] = in.offsetMod128();
}

template <Layout L>
static void BM_SumAssumeAligned64(benchmark::State& state) {
  auto const n = static_cast<std::size_t>(state.range(0));
  Buffer x(L, n);
  for (auto _ : state) {
    benchmark::DoNotOptimize(kernel_sum_assume_aligned64(x.span()));
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations() * n * sizeof(double)));
  state.counters["offset_mod_128"] = x.offsetMod128();
}

template <Layout L>
static void BM_AxpyAssumeAligned64(benchmark::State& state) {
  auto const n = static_cast<std::size_t>(state.range(0));
  Buffer x(L, n);
  Buffer y(L, n);
  for (auto _ : state) {
    kernel_axpy_assume_aligned64(1.0e-9, x.span(), y.span());
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations() * 3 * n * sizeof(double)));
  state.counters["offset_mod_128"] = x.offsetMod128();
}

// 1 Ki doubles (8 KiB, L1), 32 Ki doubles (256 KiB, L2), 4 Mi doubles (32 MiB, memory bound)
#define ALIGN_SIZES Arg(1 << 10)->Arg(1 << 15)->Arg(1 << 22)

#define ALIGN_BENCH_ALL_LAYOUTS(BM)                    \
  BENCHMARK_TEMPLATE(BM, Layout::Default)->ALIGN_SIZES;   \
  BENCHMARK_TEMPLATE(BM, Layout::Aligned64)->ALIGN_SIZES; \
  BENCHMARK_TEMPLATE(BM, Layout::Aligned128)->ALIGN_SIZES; \
  BENCHMARK_TEMPLATE(BM, Layout::Misaligned8)->ALIGN_SIZES

ALIGN_BENCH_ALL_LAYOUTS(BM_Sum);
ALIGN_BENCH_ALL_LAYOUTS(BM_Dot);
ALIGN_BENCH_ALL_LAYOUTS(BM_Axpy);
ALIGN_BENCH_ALL_LAYOUTS(BM_Cubic);
ALIGN_BENCH_ALL_LAYOUTS(BM_Stencil);

// assume_aligned is only valid on actually 64-byte aligned data
BENCHMARK_TEMPLATE(BM_SumAssumeAligned64, Layout::Aligned64)->ALIGN_SIZES;
BENCHMARK_TEMPLATE(BM_AxpyAssumeAligned64, Layout::Aligned64)->ALIGN_SIZES;
