#include "qc_math.h"
#include "pd_math.h"
constexpr bool cyclesTest() {
  InputCycles c;
  c.update(1, 0);
  c.update(1, 50); // A stuck HIGH never passes.
  c.update(0, 100);
  c.update(1, 120);
  c.update(1, 170); // Bounce ignored.
  if (c.cycles)
    return false;
  c.update(0, 200);
  c.update(0, 250);
  c.update(1, 300);
  c.update(1, 350);
  if (c.cycles != 1)
    return false;
  c.update(0, 400);
  c.update(0, 450);
  c.update(1, 500);
  c.update(1, 550);
  return c.cycles == 2;
}
static_assert(cyclesTest(), "Two complete debounced H-L-H cycles required");
constexpr double x[] = {1000, 4000, 6500, 9000}, y[] = {4, 10, 15, 20};
constexpr auto f = fitFour(x, y);
static_assert(f.valid && qcAbs(f.gain - .002) < 1e-10 && qcAbs(f.offset - 2) < 1e-10 &&
              f.r2 > .99999);
constexpr double duplicate[] = {1000, 1000, 6500, 9000};
static_assert(!fitFour(duplicate, y).valid);
constexpr double bad[] = {1000, 4000, 6500, 7000};
static_assert(fitFour(bad, y).maxError > 1);
constexpr double reverse[] = {9000, 6000, 3500, 1000};
static_assert(fitFour(reverse, y).valid && fitFour(reverse, y).gain < 0);
static_assert(fixedSinkPDO(12000, 1000) == 0x0003c064);
static_assert(activeSinkCount(0xa3, 12000) == 0xa2 && activeSinkCount(0xa3, 5000) == 0xa1);
constexpr uint32_t src[] = {fixedSinkPDO(5000, 3000), fixedSinkPDO(9000, 3000),
                            fixedSinkPDO(12000, 3000), fixedSinkPDO(20000, 3000)};
// RDO object position is SOURCE index, not local sink PDO number.
static_assert(sourceContractMV(3u << 28, src, 4) == 12000);
static_assert(sourceContractMV(4u << 28, src, 4) == 20000);
static_assert(sourceContractMV(7u << 28, src, 4) == 0 && sourceContractMV(0, src, 4) == 0);
constexpr uint32_t apdo[] = {3u << 30};
static_assert(sourceContractMV(1u << 28, apdo, 1) == 0);
