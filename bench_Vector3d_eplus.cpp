#include <benchmark/benchmark.h>

#include <algorithm>
#include <cmath>
#include <iterator>
#include <vector>

template <typename T>
class Vector3
{
 public:
  // Value Constructor
  explicit Vector3(T t_x, T t_y, T t_z) : x(t_x), y(t_y), z(t_z) {}

  template <typename U, class = typename std::enable_if<std::is_constructible<T, U>::value>::type>
  Vector3(std::initializer_list<U> const l) : x(*l.begin()), y(*(l.begin() + 1)), z(*(l.begin() + 2)) {
    assert(l.size() == 3);
  }

  T x, y, z;
};

using Real64 = double;
using Vector = Vector3<Real64>;

// test if two points in space are in the same position based on a small
// tolerance
bool isAlmostEqual3dPt(Vector v1, Vector v2) {
  // J. Glazer - March 2017

  Real64 tol = 0.0127;  //  1.27 cm = 1/2 inch
  return ((std::abs(v1.x - v2.x) < tol) && (std::abs(v1.y - v2.y) < tol) && (std::abs(v1.z - v2.z) < tol));
}

struct EdgeOfSurf
{
  int surfNum = 0;
  Vector start;
  Vector end;
  std::vector<int> otherSurfNums;
  int count = 0;
  EdgeOfSurf() : start(Vector(0., 0., 0.)), end(Vector(0., 0., 0.)) {}

  bool operator==(const EdgeOfSurf& other) const;
  bool operator!=(const EdgeOfSurf& other) const;
  // bool containsPoints(const Vector &vertex) const;
  double length() const;
};

// returns the distance between two points in space
Real64 distance(Vector v1, Vector v2) {
  // J. Glazer - March 2017

  return std::sqrt(std::pow(v1.x - v2.x, 2) + std::pow(v1.y - v2.y, 2) + std::pow(v1.z - v2.z, 2));
}

bool EdgeOfSurf::operator==(const EdgeOfSurf& other) const {
  return ((isAlmostEqual3dPt(this->start, other.start) && isAlmostEqual3dPt(this->end, other.end))
          || (isAlmostEqual3dPt(this->start, other.end) && isAlmostEqual3dPt(this->end, other.start)));
}

bool EdgeOfSurf::operator!=(const EdgeOfSurf& other) const {
  return !(*this == other);
}

double EdgeOfSurf::length() const {
  return distance(this->start, this->end);
}

static void Ori(benchmark::State& state) {

  std::vector<Vector> vertices{
    // Surface 1
    {0.0, 0.0, 0.0},
    {10.0, 0.0, 0.0},
    {10.0, 10.0, 0.0},
    {0.0, 10.0, 0.0},

    // Surface 2
    {10.0, 0.0, 0.0},
    {20.0, 0.0, 0.0},
    {20.0, 10.0, 0.0},
    {1.0, 10.0, 0.0},
  };

  for (auto _ : state) {
    std::vector<Vector> uniqueRoofVertices;
    std::vector<EdgeOfSurf> uniqEdgeOfSurfs;
    for (auto it = vertices.begin(); it != vertices.end(); ++it) {

      auto itnext = std::next(it);
      if (itnext == std::end(vertices)) {
        itnext = std::begin(vertices);
      }

      auto curVertex = *it;
      auto nextVertex = *itnext;
      auto it2 = std::find_if(uniqueRoofVertices.begin(), uniqueRoofVertices.end(),
                              [&curVertex](const auto& unqV) { return isAlmostEqual3dPt(curVertex, unqV); });
      if (it2 == std::end(uniqueRoofVertices)) {
        uniqueRoofVertices.emplace_back(curVertex);
      }

      EdgeOfSurf thisEdge;
      thisEdge.start = curVertex;
      thisEdge.end = nextVertex;
      thisEdge.count = 1;
      // Uses the custom operator== that uses isAlmostEqual3dPt internally and
      // doesn't care about order of the start/end
      auto itEdge = std::find(uniqEdgeOfSurfs.begin(), uniqEdgeOfSurfs.end(), thisEdge);
      if (itEdge == uniqEdgeOfSurfs.end()) {
        uniqEdgeOfSurfs.emplace_back(std::move(thisEdge));
      } else {
        ++(itEdge->count);
      }
    }

    // benchmark::DoNotOptimize(created_string);
  }
}

// Register the function as a benchmark
BENCHMARK(Ori);

static void New_Move(benchmark::State& state) {

  std::vector<Vector> vertices{
    // Surface 1
    {0.0, 0.0, 0.0},
    {10.0, 0.0, 0.0},
    {10.0, 10.0, 0.0},
    {0.0, 10.0, 0.0},

    // Surface 2
    {10.0, 0.0, 0.0},
    {20.0, 0.0, 0.0},
    {20.0, 10.0, 0.0},
    {1.0, 10.0, 0.0},
  };

  for (auto _ : state) {
    std::vector<Vector> uniqueRoofVertices;
    std::vector<EdgeOfSurf> uniqEdgeOfSurfs;
    for (auto it = vertices.begin(); it != vertices.end(); ++it) {

      auto itnext = std::next(it);
      if (itnext == std::end(vertices)) {
        itnext = std::begin(vertices);
      }

      auto curVertex = *it;
      auto nextVertex = *itnext;
      auto it2 = std::find_if(uniqueRoofVertices.begin(), uniqueRoofVertices.end(),
                              [&curVertex](const auto& unqV) { return isAlmostEqual3dPt(curVertex, unqV); });
      if (it2 == std::end(uniqueRoofVertices)) {
        uniqueRoofVertices.emplace_back(curVertex);
      }

      EdgeOfSurf thisEdge;
      thisEdge.start = std::move(curVertex);
      thisEdge.end = std::move(nextVertex);
      thisEdge.count = 1;
      // Uses the custom operator== that uses isAlmostEqual3dPt internally and
      // doesn't care about order of the start/end
      auto itEdge = std::find(uniqEdgeOfSurfs.begin(), uniqEdgeOfSurfs.end(), thisEdge);
      if (itEdge == uniqEdgeOfSurfs.end()) {
        uniqEdgeOfSurfs.emplace_back(std::move(thisEdge));
      } else {
        ++(itEdge->count);
      }
    }

    // benchmark::DoNotOptimize(created_string);
  }
}

BENCHMARK(New_Move);

static void New_Iterator(benchmark::State& state) {

  std::vector<Vector> vertices{
    // Surface 1
    {0.0, 0.0, 0.0},
    {10.0, 0.0, 0.0},
    {10.0, 10.0, 0.0},
    {0.0, 10.0, 0.0},

    // Surface 2
    {10.0, 0.0, 0.0},
    {20.0, 0.0, 0.0},
    {20.0, 10.0, 0.0},
    {1.0, 10.0, 0.0},
  };

  for (auto _ : state) {
    std::vector<Vector> uniqueRoofVertices;
    std::vector<EdgeOfSurf> uniqEdgeOfSurfs;
    for (auto it = vertices.begin(); it != vertices.end(); ++it) {

      auto itnext = std::next(it);
      if (itnext == std::end(vertices)) {
        itnext = std::begin(vertices);
      }

      auto it2 = std::find_if(uniqueRoofVertices.begin(), uniqueRoofVertices.end(), [&it](const auto& unqV) { return isAlmostEqual3dPt(*it, unqV); });
      if (it2 == std::end(uniqueRoofVertices)) {
        uniqueRoofVertices.emplace_back(*it);
      }

      EdgeOfSurf thisEdge;
      thisEdge.start = *it;
      thisEdge.end = *itnext;
      thisEdge.count = 1;
      // Uses the custom operator== that uses isAlmostEqual3dPt internally and
      // doesn't care about order of the start/end
      auto itEdge = std::find(uniqEdgeOfSurfs.begin(), uniqEdgeOfSurfs.end(), thisEdge);
      if (itEdge == uniqEdgeOfSurfs.end()) {
        uniqEdgeOfSurfs.emplace_back(std::move(thisEdge));
      } else {
        ++(itEdge->count);
      }
    }

    // benchmark::DoNotOptimize(created_string);
  }
}

BENCHMARK(New_Iterator);
