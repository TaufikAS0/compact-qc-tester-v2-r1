#include "qc_math.h"
#include "pd_math.h"
#include "qc_report_math.h"
static_assert(parseVariant("M0") == DeviceVariant::M0 && parseVariant("M1") == DeviceVariant::M1);
static_assert(parseVariant("M2") == DeviceVariant::Unselected);
static_assert(pdApplicable(DeviceVariant::M0) && !pdApplicable(DeviceVariant::M1) &&
              !pdApplicable(DeviceVariant::Unselected));
static_assert(!testApplicable(DeviceVariant::M1, "STUSB4500") &&
              testApplicable(DeviceVariant::M0, "STUSB4500") &&
              testApplicable(DeviceVariant::M1, "OLED"));
static_assert(qcCrc("123456789") == 0xcbf43926u && qcCrc("PASS") != qcCrc("FAIL"));
static_assert(csvFormulaLead('=') && csvFormulaLead('@') && !csvFormulaLead('T'));
constexpr bool csvTests() {
  char out[40] = {};
  qcCsvQuote("a,\"b\"\r\nc", out, sizeof(out));
  if (std::string_view(out) != "\"a,\"\"b\"\"\r\nc\"")
    return false;
  qcCsvQuote(" \t=1+1", out, sizeof(out));
  if (std::string_view(out) != "\"' \t=1+1\"")
    return false;
  if (qcCsvQuote("too long", out, 2))
    return false;
  qcCsvQuote("", out, sizeof(out));
  return std::string_view(out) == "\"\"";
}
static_assert(csvTests(), "CSV quotes/newlines, formula protection and capacity bound");
static_assert(qcIsoDate("2024-02-29") && qcIsoDate("") && !qcIsoDate("2025-02-29") &&
              !qcIsoDate("2100-02-29") && !qcIsoDate("2026-13-01"));
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
