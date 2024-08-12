#include <benchmark/benchmark.h>

#include <algorithm>
#include <cassert>
#include <array>
#include <string>

using Real64 = double;

Real64 constexpr minX = 2.0;
Real64 constexpr maxX = 8.0;
Real64 constexpr minY = 3.0;
Real64 constexpr maxY = 6.0;

Real64 constexpr below_x = 0.0;
Real64 constexpr center_x = 5.0;
Real64 constexpr greater_x = 10.0;

Real64 constexpr below_y = 1.0;
Real64 constexpr center_y = 4.5;
Real64 constexpr greater_y = 9.0;

struct Point
{
  Real64 x = 0.0;
  Real64 y = 0.0;
};

struct Line
{
  Point p0{};
  Point p1{};
};

struct TestCase
{
  Line line_ori;
  bool visible = false;
  Line line_new{};  // Only defined if visible
};

constexpr std::array<TestCase, 72> regular_test_cases{{
  // From 0 to 3
  TestCase{Line{Point{below_x, below_y}, Point{below_x, center_y}}, false},
  // From 0 to 6
  TestCase{Line{Point{below_x, below_y}, Point{below_x, greater_y}}, false},
  // From 0 to 1
  TestCase{Line{Point{below_x, below_y}, Point{center_x, below_y}}, false},
  // From 0 to 4
  TestCase{Line{Point{below_x, below_y}, Point{center_x, center_y}}, true, Line{Point{2.8571428571428568, minY}, Point{center_x, center_y}}},
  // From 0 to 7
  TestCase{Line{Point{below_x, below_y}, Point{center_x, greater_y}}, true, Line{Point{minX, 4.2}, Point{3.125, maxY}}},
  // From 0 to 2
  TestCase{Line{Point{below_x, below_y}, Point{greater_x, below_y}}, false},
  // From 0 to 5
  TestCase{Line{Point{below_x, below_y}, Point{greater_x, center_y}}, true, Line{Point{5.7142857142857135, minY}, Point{maxX, 3.8000000000000003}}},
  // From 0 to 8
  TestCase{Line{Point{below_x, below_y}, Point{greater_x, greater_y}}, true, Line{Point{2.5, minY}, Point{6.25, maxY}}},
  // From 3 to 0
  TestCase{Line{Point{below_x, center_y}, Point{below_x, below_y}}, false},
  // From 3 to 6
  TestCase{Line{Point{below_x, center_y}, Point{below_x, greater_y}}, false},
  // From 3 to 1
  TestCase{Line{Point{below_x, center_y}, Point{center_x, below_y}}, true, Line{Point{minX, 3.0999999999999996}, Point{2.142857142857143, minY}}},
  // From 3 to 4
  TestCase{Line{Point{below_x, center_y}, Point{center_x, center_y}}, true, Line{Point{minX, center_y}, Point{center_x, center_y}}},
  // From 3 to 7
  TestCase{Line{Point{below_x, center_y}, Point{center_x, greater_y}}, false},
  // From 3 to 2
  TestCase{Line{Point{below_x, center_y}, Point{greater_x, below_y}}, true, Line{Point{minX, 3.8}, Point{4.285714285714286, minY}}},
  // From 3 to 5
  TestCase{Line{Point{below_x, center_y}, Point{greater_x, center_y}}, true, Line{Point{minX, center_y}, Point{maxX, center_y}}},
  // From 3 to 8
  TestCase{Line{Point{below_x, center_y}, Point{greater_x, greater_y}}, true, Line{Point{minX, 5.4}, Point{3.333333333333333, maxY}}},
  // From 6 to 0
  TestCase{Line{Point{below_x, greater_y}, Point{below_x, below_y}}, false},
  // From 6 to 3
  TestCase{Line{Point{below_x, greater_y}, Point{below_x, center_y}}, false},
  // From 6 to 1
  TestCase{Line{Point{below_x, greater_y}, Point{center_x, below_y}}, true, Line{Point{minX, 5.8}, Point{3.75, minY}}},
  // From 6 to 4
  TestCase{Line{Point{below_x, greater_y}, Point{center_x, center_y}}, true, Line{Point{3.333333333333333, maxY}, Point{center_x, center_y}}},
  // From 6 to 7
  TestCase{Line{Point{below_x, greater_y}, Point{center_x, greater_y}}, false},
  // From 6 to 2
  TestCase{Line{Point{below_x, greater_y}, Point{greater_x, below_y}}, true, Line{Point{3.75, maxY}, Point{7.5, minY}}},
  // From 6 to 5
  TestCase{Line{Point{below_x, greater_y}, Point{greater_x, center_y}}, true, Line{Point{6.666666666666666, maxY}, Point{maxX, 5.4}}},
  // From 6 to 8
  TestCase{Line{Point{below_x, greater_y}, Point{greater_x, greater_y}}, false},
  // From 1 to 0
  TestCase{Line{Point{center_x, below_y}, Point{below_x, below_y}}, false},
  // From 1 to 3
  TestCase{Line{Point{center_x, below_y}, Point{below_x, center_y}}, true, Line{Point{2.1428571428571432, minY}, Point{minX, 3.1}}},
  // From 1 to 6
  TestCase{Line{Point{center_x, below_y}, Point{below_x, greater_y}}, true, Line{Point{3.75, minY}, Point{minX, 5.8}}},
  // From 1 to 4
  TestCase{Line{Point{center_x, below_y}, Point{center_x, center_y}}, true, Line{Point{center_x, minY}, Point{center_x, center_y}}},
  // From 1 to 7
  TestCase{Line{Point{center_x, below_y}, Point{center_x, greater_y}}, true, Line{Point{center_x, minY}, Point{center_x, maxY}}},
  // From 1 to 2
  TestCase{Line{Point{center_x, below_y}, Point{greater_x, below_y}}, false},
  // From 1 to 5
  TestCase{Line{Point{center_x, below_y}, Point{greater_x, center_y}}, true, Line{Point{7.857142857142857, minY}, Point{maxX, 3.1}}},
  // From 1 to 8
  TestCase{Line{Point{center_x, below_y}, Point{greater_x, greater_y}}, true, Line{Point{6.25, minY}, Point{maxX, 5.8}}},
  // From 4 to 0
  TestCase{Line{Point{center_x, center_y}, Point{below_x, below_y}}, true, Line{Point{center_x, center_y}, Point{2.857142857142857, minY}}},
  // From 4 to 3
  TestCase{Line{Point{center_x, center_y}, Point{below_x, center_y}}, true, Line{Point{center_x, center_y}, Point{minX, center_y}}},
  // From 4 to 6
  TestCase{Line{Point{center_x, center_y}, Point{below_x, greater_y}}, true, Line{Point{center_x, center_y}, Point{3.3333333333333335, maxY}}},
  // From 4 to 1
  TestCase{Line{Point{center_x, center_y}, Point{center_x, below_y}}, true, Line{Point{center_x, center_y}, Point{center_x, minY}}},
  // From 4 to 7
  TestCase{Line{Point{center_x, center_y}, Point{center_x, greater_y}}, true, Line{Point{center_x, center_y}, Point{center_x, maxY}}},
  // From 4 to 2
  TestCase{Line{Point{center_x, center_y}, Point{greater_x, below_y}}, true, Line{Point{center_x, center_y}, Point{7.142857142857142, minY}}},
  // From 4 to 5
  TestCase{Line{Point{center_x, center_y}, Point{greater_x, center_y}}, true, Line{Point{center_x, center_y}, Point{maxX, center_y}}},
  // From 4 to 8
  TestCase{Line{Point{center_x, center_y}, Point{greater_x, greater_y}}, true, Line{Point{center_x, center_y}, Point{6.666666666666666, maxY}}},
  // From 7 to 0
  TestCase{Line{Point{center_x, greater_y}, Point{below_x, below_y}}, true, Line{Point{3.125, maxY}, Point{minX, 4.2}}},
  // From 7 to 3
  TestCase{Line{Point{center_x, greater_y}, Point{below_x, center_y}}, false},
  // From 7 to 6
  TestCase{Line{Point{center_x, greater_y}, Point{below_x, greater_y}}, false},
  // From 7 to 1
  TestCase{Line{Point{center_x, greater_y}, Point{center_x, below_y}}, true, Line{Point{center_x, maxY}, Point{center_x, minY}}},
  // From 7 to 4
  TestCase{Line{Point{center_x, greater_y}, Point{center_x, center_y}}, true, Line{Point{center_x, maxY}, Point{center_x, center_y}}},
  // From 7 to 2
  TestCase{Line{Point{center_x, greater_y}, Point{greater_x, below_y}}, true, Line{Point{6.875, maxY}, Point{maxX, 4.2}}},
  // From 7 to 5
  TestCase{Line{Point{center_x, greater_y}, Point{greater_x, center_y}}, false},
  // From 7 to 8
  TestCase{Line{Point{center_x, greater_y}, Point{greater_x, greater_y}}, false},
  // From 2 to 0
  TestCase{Line{Point{greater_x, below_y}, Point{below_x, below_y}}, false},
  // From 2 to 3
  TestCase{Line{Point{greater_x, below_y}, Point{below_x, center_y}}, true, Line{Point{4.2857142857142865, minY}, Point{minX, 3.8000000000000003}}},
  // From 2 to 6
  TestCase{Line{Point{greater_x, below_y}, Point{below_x, greater_y}}, true, Line{Point{7.5, minY}, Point{3.75, maxY}}},
  // From 2 to 1
  TestCase{Line{Point{greater_x, below_y}, Point{center_x, below_y}}, false},
  // From 2 to 4
  TestCase{Line{Point{greater_x, below_y}, Point{center_x, center_y}}, true, Line{Point{7.142857142857143, minY}, Point{center_x, center_y}}},
  // From 2 to 7
  TestCase{Line{Point{greater_x, below_y}, Point{center_x, greater_y}}, true, Line{Point{maxX, 4.2}, Point{6.875, maxY}}},
  // From 2 to 5
  TestCase{Line{Point{greater_x, below_y}, Point{greater_x, center_y}}, false},
  // From 2 to 8
  TestCase{Line{Point{greater_x, below_y}, Point{greater_x, greater_y}}, false},
  // From 5 to 0
  TestCase{Line{Point{greater_x, center_y}, Point{below_x, below_y}}, true, Line{Point{maxX, 3.8}, Point{5.714285714285714, minY}}},
  // From 5 to 3
  TestCase{Line{Point{greater_x, center_y}, Point{below_x, center_y}}, true, Line{Point{maxX, center_y}, Point{minX, center_y}}},
  // From 5 to 6
  TestCase{Line{Point{greater_x, center_y}, Point{below_x, greater_y}}, true, Line{Point{maxX, 5.4}, Point{6.666666666666667, maxY}}},
  // From 5 to 1
  TestCase{Line{Point{greater_x, center_y}, Point{center_x, below_y}}, true, Line{Point{maxX, 3.0999999999999996}, Point{7.857142857142858, minY}}},
  // From 5 to 4
  TestCase{Line{Point{greater_x, center_y}, Point{center_x, center_y}}, true, Line{Point{maxX, center_y}, Point{center_x, center_y}}},
  // From 5 to 7
  TestCase{Line{Point{greater_x, center_y}, Point{center_x, greater_y}}, false},
  // From 5 to 2
  TestCase{Line{Point{greater_x, center_y}, Point{greater_x, below_y}}, false},
  // From 5 to 8
  TestCase{Line{Point{greater_x, center_y}, Point{greater_x, greater_y}}, false},
  // From 8 to 0
  TestCase{Line{Point{greater_x, greater_y}, Point{below_x, below_y}}, true, Line{Point{6.25, maxY}, Point{2.5, minY}}},
  // From 8 to 3
  TestCase{Line{Point{greater_x, greater_y}, Point{below_x, center_y}}, true, Line{Point{3.333333333333334, maxY}, Point{minX, 5.4}}},
  // From 8 to 6
  TestCase{Line{Point{greater_x, greater_y}, Point{below_x, greater_y}}, false},
  // From 8 to 1
  TestCase{Line{Point{greater_x, greater_y}, Point{center_x, below_y}}, true, Line{Point{maxX, 5.8}, Point{6.25, minY}}},
  // From 8 to 4
  TestCase{Line{Point{greater_x, greater_y}, Point{center_x, center_y}}, true, Line{Point{6.666666666666667, maxY}, Point{center_x, center_y}}},
  // From 8 to 7
  TestCase{Line{Point{greater_x, greater_y}, Point{center_x, greater_y}}, false},
  // From 8 to 2
  TestCase{Line{Point{greater_x, greater_y}, Point{greater_x, below_y}}, false},
  // From 8 to 5
  TestCase{Line{Point{greater_x, greater_y}, Point{greater_x, center_y}}, false},
}};

void CLIPLINE(Real64& x1, Real64& x2, Real64& y1, Real64& y2, Real64 maxX, Real64 minX, Real64 maxY, Real64 minY, bool& visible, bool& rev) {
  // Line segment clipping
  // Reference:
  // Slater, M., Barsky, B.A.
  // 2D line and polygon clipping based on space subdivision.
  // The Visual Computer 10, 407–422 (1994).
  Real64 dx, dy, e, xinc, yinc, tempVar;
  bool needX = true, needY = true;
  int c1, c2;

  if (x1 > x2) {  // reverse for efficiency
    tempVar = x1;
    x1 = x2;
    x2 = tempVar;
    tempVar = y1;
    y1 = y2;
    y2 = tempVar;
    rev = true;
  }
  if (x1 > maxX || x2 < minX) return;  // x is positive
  if (x1 < minX) {
    if (y1 < minY) {
      if (y2 < minY) return;
      c1 = 0;
      dx = x2 - x1;
      dy = y2 - y1;
      e = dy * (minX - x1) + dx * (y1 - minY);
    } else if (y1 > maxY) {
      if (y2 > maxY) return;
      c1 = 6;
      dx = x2 - x1;
      dy = y2 - y1;
      e = dy * (minX - x1) + dx * (y1 - maxY);
    } else {
      c1 = 3;
      dx = x2 - x1;
      dy = y2 - y1;
      if (dy > 0) {
        e = dy * (minX - x1) + dx * (y1 - maxY);
      } else {
        e = dy * (minX - x1) + dx * (y1 - minY);
      }
    }
  } else {
    if (y1 < minY) {
      if (y2 < minY) return;
      c1 = 1;
      dx = x2 - x1;
      dy = y2 - y1;
      e = dy * (maxX - x1) + dx * (y1 - minY);
    } else if (y1 > maxY) {
      if (y2 > maxY) return;
      c1 = 7;
      dx = x2 - x1;
      dy = y2 - y1;
      e = dy * (maxX - x1) + dx * (y1 - maxY);
    } else {
      visible = true;
      if (x2 <= maxX && (y2 >= minY && y2 <= maxY)) return;
      c1 = 4;
      dx = x2 - x1;
      dy = y2 - y1;
      if (dy > 0) {
        e = dy * (maxX - x1) + dx * (y1 - maxY);
      } else {
        e = dy * (maxX - x1) + dx * (y1 - minY);
      }
    }
  }
  c2 = c1;
  if (dy > 0) {
    while (true) {
      if (e < 0.0) {
        if (c2 == 1)
          return;
        else if (c2 == 3) {
          visible = true;
          x1 = minX;
          y1 = maxY + e / dx;
          if (x2 <= maxX && y2 <= maxY) return;
        } else if (c2 == 4) {
          x2 = maxX;
          y2 = maxY + e / dx;
          return;
        }
        if (needX) {
          xinc = dy * (maxX - minX);
          needX = false;
        }
        e += xinc;
        c2 += 1;
      } else {
        if (c2 == 3)
          return;
        else if (c2 == 1) {
          visible = true;
          x1 = maxX - e / dy;
          y1 = minY;
          if (x2 <= maxX && y2 <= maxY) return;
        } else if (c2 == 4) {
          x2 = maxX - e / dy;
          y2 = maxY;
          return;
        }
        if (needY) {
          yinc = dx * (maxY - minY);
          needY = false;
        }
        e -= yinc;
        c2 += 3;
      }
    }
  } else {
    while (true) {
      if (e >= 0.0) {
        if (c2 == 7)
          return;
        else if (c2 == 3) {
          visible = true;
          x1 = minX;
          y1 = minY + e / dx;
          if (x2 <= maxX && y2 >= minY) return;
        } else if (c2 == 4) {
          x2 = maxX;
          y2 = minY + e / dx;
          return;
        }
        if (needX) {
          xinc = dy * (maxX - minX);
          needX = false;
        }
        e += xinc;
        c2 += 1;
      } else {
        if (c2 == 3)
          return;
        else if (c2 == 7) {
          visible = true;
          x1 = maxX - e / dy;
          y1 = maxY;
          if (x2 <= maxX && y2 >= minY) return;
        } else if (c2 == 4) {
          x2 = maxX - e / dy;
          y2 = minY;
          return;
        }
        if (needY) {
          yinc = dx * (maxY - minY);
          needY = false;
        }
        e += yinc;
        c2 -= 3;
      }
    }
  }
}

void clip(Real64& x0, Real64& x1, Real64& y0, Real64& y1, Real64 maxX, Real64 minX, Real64 maxY, Real64 minY, bool& visible) {
  Real64 dx = x1 - x0;
  Real64 dy = y1 - y0;
  std::array<Real64, 4> p{};
  std::array<Real64, 4> q{};

  Real64 u1 = 0;
  Real64 u2 = 1;

  p[0] = -dx;
  q[0] = x0 - minX;
  p[1] = dx;
  q[1] = maxX - x0;
  p[2] = -dy;
  q[2] = y0 - minY;
  p[3] = dy;
  q[3] = maxY - y0;

  Real64 const xmin = std::min(x0, x1);
  Real64 const xmax = std::max(x0, x1);
  Real64 const ymin = std::min(y0, y1);
  Real64 const ymax = std::max(y0, y1);
  if ((xmax < minX) || (xmin > maxX) || (ymax < minY) || (ymin > maxY)) {
    // Line does not cross the clip window
    return;
  }

  for (size_t i = 0; i < 4; i++) {
    if (p[i] == 0 && q[i] < 0) {
      // Line is parallel to clipping window
      return;
    }
    if (p[i] < 0) {
      Real64 const t = (q[i]) / (p[i]);
      if (t > u1 && t < u2) {
        u1 = t;
      }
    } else if (p[i] > 0) {
      Real64 const t = (q[i]) / (p[i]);
      if (t > u1 && t < u2) {
        u2 = t;
      }
    }
  }
  if (u1 < u2) {
    visible = true;

    Real64 const xn0 = x0 + u1 * dx;
    Real64 const yn0 = y0 + u1 * dy;

    Real64 const xn1 = x0 + u2 * dx;
    Real64 const yn1 = y0 + u2 * dy;

    x0 = xn0;
    y0 = yn0;
    x1 = xn1;
    y1 = yn1;
  }
}

void liang_barsky_clipper(Real64& x0, Real64& x1, Real64& y0, Real64& y1, Real64 maxX, Real64 minX, Real64 maxY, Real64 minY, bool& visible) {
  // defining variables
  Real64 const p1 = -(x1 - x0);
  Real64 const p2 = -p1;
  Real64 const p3 = -(y1 - y0);
  Real64 const p4 = -p3;

  Real64 const q1 = x0 - minX;
  Real64 const q2 = maxX - x0;
  Real64 const q3 = y0 - minY;
  Real64 const q4 = maxY - y0;

  std::array<Real64, 2> posarr{1, 1};
  std::array<Real64, 2> negarr{0, 0};

  size_t posind = 0;
  size_t negind = 0;

  if ((p1 == 0 && q1 < 0) || (p2 == 0 && q2 < 0) || (p3 == 0 && q3 < 0) || (p4 == 0 && q4 < 0)) {
    // Line is parallel to clipping window
    return;
  }
  if (p1 != 0) {
    Real64 r1 = q1 / p1;
    Real64 r2 = q2 / p2;
    if (p1 < 0) {
      negarr[negind++] = r1;  // for negative p1, add it to negative array
      posarr[posind++] = r2;  // and add p2 to positive array
    } else if (p1 > 0) {
      negarr[negind++] = r2;
      posarr[posind++] = r1;
    }
  }
  if (p3 != 0) {
    Real64 r3 = q3 / p3;
    Real64 r4 = q4 / p4;
    if (p3 < 0) {
      negarr[negind++] = r3;
      posarr[posind++] = r4;
    } else if (p3 > 0) {
      negarr[negind++] = r4;
      posarr[posind++] = r3;
    }
  }

  Real64 const rn1 = *std::max_element(negarr.begin(), negarr.end());  // maximum of negative array
  Real64 const rn2 = *std::min_element(posarr.begin(), posarr.end());  // minimum of positive array

  if (rn1 > rn2) {  // reject
    // Line is outside the clipping window
    return;
  }

  visible = true;

  Real64 const xn0 = x0 + p2 * rn1;
  Real64 const yn0 = y0 + p4 * rn1;  // computing new points

  Real64 const xn1 = x0 + p2 * rn2;
  Real64 const yn1 = y0 + p4 * rn2;

  x0 = xn0;
  y0 = yn0;
  x1 = xn1;
  y1 = yn1;
}

void liang_barsky_clipper2(Real64& x0, Real64& x1, Real64& y0, Real64& y1, Real64 maxX, Real64 minX, Real64 maxY, Real64 minY, bool& visible,
                           bool& rev) {
  if (x0 > x1) {  // reverse for efficiency
    std::swap(x0, x1);
    std::swap(y0, y1);
    rev = true;
  }

  if (x0 > maxX || x1 < minX) {
    // Both points are outside the clip window, so they can't cross it
    return;
  }

  // defining variables
  Real64 const dx = x1 - x0;  // >= 0
  Real64 const dy = y1 - y0;

  Real64 const q1 = x0 - minX;
  Real64 const q2 = maxX - x0;
  Real64 const q3 = y0 - minY;
  Real64 const q4 = maxY - y0;

  Real64 u1 = 0;
  Real64 u2 = 1;

  if ((dx == 0 && (q1 < 0 || q2 < 0)) || (dy == 0 && (q3 < 0 || q4 < 0))) {
    // Line is parallel to clipping window
    return;
  }
  if (dx != 0) {
    Real64 const r1 = q1 / -dx;
    if (r1 > u1) {
      u1 = r1;
    }
    Real64 const r2 = q2 / dx;
    if (r2 < u2) {
      u2 = r2;
    }
  }
  if (dy != 0) {
    Real64 const r3 = q3 / -dy;
    Real64 const r4 = q4 / dy;
    if (dy > 0) {
      if (r3 > u1) {
        u1 = r3;
      }
      if (r4 < u2) {
        u2 = r4;
      }
    } else {
      if (r4 > u1) {
        u1 = r4;
      }
      if (r3 < u2) {
        u2 = r3;
      }
    }
  }

  if (u1 > u2) {  // reject
    // Line is outside the clipping window
    return;
  }

  visible = true;

  Real64 const xn0 = x0 + dx * u1;
  Real64 const yn0 = y0 + dy * u1;  // computing new points

  Real64 const xn1 = x0 + dx * u2;
  Real64 const yn1 = y0 + dy * u2;

  x0 = xn0;
  y0 = yn0;
  x1 = xn1;
  y1 = yn1;
}

void liang_barsky_clipper3(Real64& x0, Real64& x1, Real64& y0, Real64& y1, Real64 maxX, Real64 minX, Real64 maxY, Real64 minY, bool& visible) {
  bool rev = false;
  if (x0 > x1) {  // reverse for efficiency
    std::swap(x0, x1);
    std::swap(y0, y1);
    rev = true;
  }

  if (x0 > maxX || x1 < minX) {
    // Both points are outside the clip window, so they can't cross it
    return;
  }

  // defining variables
  Real64 const dx = x1 - x0;  // >= 0
  Real64 const dy = y1 - y0;

  Real64 const q1 = x0 - minX;
  Real64 const q2 = maxX - x0;
  Real64 const q3 = y0 - minY;
  Real64 const q4 = maxY - y0;

  Real64 u1 = 0;
  Real64 u2 = 1;

  if ((dx == 0 && (q1 < 0 || q2 < 0)) || (dy == 0 && (q3 < 0 || q4 < 0))) {
    // Line is parallel to clipping window
    return;
  }
  if (dx != 0) {
    Real64 const r1 = q1 / -dx;
    if (r1 > u1) {
      u1 = r1;
    }
    Real64 const r2 = q2 / dx;
    if (r2 < u2) {
      u2 = r2;
    }
  }
  if (dy != 0) {
    Real64 const r3 = q3 / -dy;
    Real64 const r4 = q4 / dy;
    if (dy > 0) {
      if (r3 > u1) {
        u1 = r3;
      }
      if (r4 < u2) {
        u2 = r4;
      }
    } else {
      if (r4 > u1) {
        u1 = r4;
      }
      if (r3 < u2) {
        u2 = r3;
      }
    }
  }

  if (u1 > u2) {  // reject
    // Line is outside the clipping window
    return;
  }

  visible = true;

  Real64 const xn0 = x0 + dx * u1;
  Real64 const yn0 = y0 + dy * u1;  // computing new points

  Real64 const xn1 = x0 + dx * u2;
  Real64 const yn1 = y0 + dy * u2;

  if (rev) {
    x0 = xn1;
    y0 = yn1;
    x1 = xn0;
    y1 = yn0;
  } else {
    x0 = xn0;
    y0 = yn0;
    x1 = xn1;
    y1 = yn1;
  }
}

static void BM_CLIPLINE(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (const auto& t : regular_test_cases) {
      Real64 x0 = t.line_ori.p0.x;
      Real64 y0 = t.line_ori.p0.y;

      Real64 x1 = t.line_ori.p1.x;
      Real64 y1 = t.line_ori.p1.y;

      bool visible = false;
      bool rev = false;
      CLIPLINE(x0, x1, y0, y1, maxX, minX, maxY, minY, visible, rev);
      if (rev) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }
      assert(std::abs(x0 - t.line_new.p0.x) < 0.000001);
      assert(std::abs(y0 - t.line_new.p0.y) < 0.000001);
      assert(std::abs(x1 - t.line_new.p1.x) < 0.000001);
      assert(std::abs(y1 - t.line_new.p1.y) < 0.000001);
    }
  }
}

static void BM_Clip2(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (const auto& t : regular_test_cases) {
      Real64 x0 = t.line_ori.p0.x;
      Real64 y0 = t.line_ori.p0.y;

      Real64 x1 = t.line_ori.p1.x;
      Real64 y1 = t.line_ori.p1.y;

      bool visible = false;
      clip(x0, x1, y0, y1, maxX, minX, maxY, minY, visible);
      assert(std::abs(x0 - t.line_new.p0.x) < 0.000001);
      assert(std::abs(y0 - t.line_new.p0.y) < 0.000001);
      assert(std::abs(x1 - t.line_new.p1.x) < 0.000001);
      assert(std::abs(y1 - t.line_new.p1.y) < 0.000001);
    }
  }
}

static void BM_liang_barsky_clipper(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (const auto& t : regular_test_cases) {
      Real64 x0 = t.line_ori.p0.x;
      Real64 y0 = t.line_ori.p0.y;

      Real64 x1 = t.line_ori.p1.x;
      Real64 y1 = t.line_ori.p1.y;

      bool visible = false;
      liang_barsky_clipper(x0, x1, y0, y1, maxX, minX, maxY, minY, visible);
      assert(std::abs(x0 - t.line_new.p0.x) < 0.000001);
      assert(std::abs(y0 - t.line_new.p0.y) < 0.000001);
      assert(std::abs(x1 - t.line_new.p1.x) < 0.000001);
      assert(std::abs(y1 - t.line_new.p1.y) < 0.000001);
    }
  }
}
static void BM_liang_barsky_clipper2(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (const auto& t : regular_test_cases) {
      Real64 x0 = t.line_ori.p0.x;
      Real64 y0 = t.line_ori.p0.y;

      Real64 x1 = t.line_ori.p1.x;
      Real64 y1 = t.line_ori.p1.y;

      bool visible = false;
      bool rev = false;
      liang_barsky_clipper2(x0, x1, y0, y1, maxX, minX, maxY, minY, visible, rev);
      if (rev) {
        std::swap(x0, x1);
        std::swap(y0, y1);
      }
      assert(std::abs(x0 - t.line_new.p0.x) < 0.000001);
      assert(std::abs(y0 - t.line_new.p0.y) < 0.000001);
      assert(std::abs(x1 - t.line_new.p1.x) < 0.000001);
      assert(std::abs(y1 - t.line_new.p1.y) < 0.000001);
    }
  }
}

static void BM_liang_barsky_clipper3(benchmark::State& state) {
  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    for (const auto& t : regular_test_cases) {
      Real64 x0 = t.line_ori.p0.x;
      Real64 y0 = t.line_ori.p0.y;

      Real64 x1 = t.line_ori.p1.x;
      Real64 y1 = t.line_ori.p1.y;

      bool visible = false;
      liang_barsky_clipper3(x0, x1, y0, y1, maxX, minX, maxY, minY, visible);
      assert(std::abs(x0 - t.line_new.p0.x) < 0.000001);
      assert(std::abs(y0 - t.line_new.p0.y) < 0.000001);
      assert(std::abs(x1 - t.line_new.p1.x) < 0.000001);
      assert(std::abs(y1 - t.line_new.p1.y) < 0.000001);
    }
  }
}

BENCHMARK(BM_CLIPLINE);
BENCHMARK(BM_Clip2);
BENCHMARK(BM_liang_barsky_clipper);
BENCHMARK(BM_liang_barsky_clipper2);
BENCHMARK(BM_liang_barsky_clipper3);
