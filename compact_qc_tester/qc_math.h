#pragma once
#include <stdint.h>
// Pure algorithms: tested by static assertions in qc_math_tests.cpp.
struct InputCycles {
  int candidate = -1, stable = -1;
  uint32_t changedAt = 0;
  unsigned cycles = 0;
  bool seenHigh = false, seenLow = false;
  constexpr void update(int level, uint32_t now) {
    if (level < 0) {
      candidate = stable = -1;
      seenHigh = seenLow = false;
      return;
    }
    if (level != candidate) {
      candidate = level;
      changedAt = now;
    }
    if (now - changedAt < 50 || stable == candidate)
      return;
    stable = candidate;
    if (stable == 1) {
      if (seenHigh && seenLow && cycles < 2)
        ++cycles;
      seenHigh = true;
      seenLow = false;
    } else if (seenHigh)
      seenLow = true;
  }
};
struct LinearFit {
  bool valid = false;
  double gain = 0, offset = 0, r2 = 0, maxError = 0;
};
constexpr double qcAbs(double x) { return x < 0 ? -x : x; }
constexpr LinearFit fitFour(const double *x, const double *y) {
  double mx = 0, my = 0;
  for (int i = 0; i < 4; ++i) {
    mx += x[i] / 4;
    my += y[i] / 4;
  }
  double xx = 0, xy = 0, yy = 0;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < i; ++j)
      if (qcAbs(x[i] - x[j]) < 1 || qcAbs(y[i] - y[j]) < 0.1)
        return {};
    xx += (x[i] - mx) * (x[i] - mx);
    xy += (x[i] - mx) * (y[i] - my);
    yy += (y[i] - my) * (y[i] - my);
  }
  if (xx < 100 || yy < 100)
    return {};
  LinearFit f;
  f.gain = xy / xx;
  f.offset = my - f.gain * mx;
  if (qcAbs(f.gain) < 0.000001 || qcAbs(f.gain) > 1)
    return {};
  double ss = 0;
  for (int i = 0; i < 4; ++i) {
    double e = f.gain * x[i] + f.offset - y[i];
    ss += e * e;
    if (qcAbs(e) > f.maxError)
      f.maxError = qcAbs(e);
  }
  f.r2 = 1 - ss / yy;
  f.valid = true;
  return f;
}
