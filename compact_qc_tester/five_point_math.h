#pragma once
constexpr int fivePointVerdict(const int *verdicts) {
  int passed = 0;
  for (int i = 0; i < 5; ++i) {
    if (verdicts[i] == -1) return -1;
    if (verdicts[i] == 1) ++passed;
  }
  return passed == 5 ? 1 : 0;
}
