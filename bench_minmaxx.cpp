#include <benchmark/benchmark.h>

#include <algorithm>
#include <limits>
#include <ranges>
#include <vector>

class Point3d
{
 public:
  Point3d(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

  [[nodiscard]] double x() const {
    return m_x;
  }

  [[nodiscard]] double y() const {
    return m_y;
  }

  [[nodiscard]] double z() const {
    return m_z;
  }

  static std::vector<Point3d> makeTestCase() {
    constexpr double xmin = 0.0;
    constexpr double xmax = 10.0;
    constexpr double zmin = 0.0;
    constexpr double zmax = 3.0;

    return std::vector<Point3d>{
      {xmin, 0, zmax},
      {xmin, 0, zmin},
      {xmax, 0, zmin},
      {xmax, 0, zmax},
    };
  }

 private:
  double m_x;
  double m_y;
  double m_z;
};

#define NDIMS 3

static void BM_RegularLoop(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    auto vertices = Point3d::makeTestCase();
    // new coordinate system has z' in direction of outward normal, y' is up
    double xmin = std::numeric_limits<double>::max();
    double xmax = std::numeric_limits<double>::lowest();
#if NDIMS > 1
    double ymin = std::numeric_limits<double>::max();
    double ymax = std::numeric_limits<double>::lowest();
#endif
#if NDIMS > 2
    double zmin = std::numeric_limits<double>::max();
    double zmax = std::numeric_limits<double>::lowest();
#endif
    for (const Point3d& v : vertices) {
      xmin = std::min(xmin, v.x());
      xmax = std::max(xmax, v.x());
#if NDIMS > 1
      ymin = std::min(ymin, v.y());
      ymax = std::max(ymax, v.y());
#endif
#if NDIMS > 2
      zmin = std::min(ymin, v.z());
      zmax = std::max(ymax, v.z());
#endif
    }
    benchmark::DoNotOptimize(xmin);
    benchmark::DoNotOptimize(xmax);
#if NDIMS > 1
    benchmark::DoNotOptimize(ymin);
    benchmark::DoNotOptimize(ymax);
#endif
#if NDIMS > 2
    benchmark::DoNotOptimize(zmin);
    benchmark::DoNotOptimize(zmax);
#endif
  }
}

static void BM_Ranges(benchmark::State& state) {
  for (auto _ : state) {
    auto vertices = Point3d::makeTestCase();
    auto [xmin, xmax] = std::ranges::minmax(vertices | std::views::transform([](auto& p) { return p.x(); }));
    benchmark::DoNotOptimize(xmin);
    benchmark::DoNotOptimize(xmax);

#if NDIMS > 1
    auto [ymin, ymax] = std::ranges::minmax(vertices | std::views::transform([](auto& p) { return p.y(); }));
    benchmark::DoNotOptimize(ymin);
    benchmark::DoNotOptimize(ymax);
#endif
#if NDIMS > 2
    auto [zmin, zmax] = std::ranges::minmax(vertices | std::views::transform([](auto& p) { return p.z(); }));
    benchmark::DoNotOptimize(zmin);
    benchmark::DoNotOptimize(zmax);
#endif
  }
}

// Register the function as a benchmark
BENCHMARK(BM_RegularLoop);
BENCHMARK(BM_Ranges);
