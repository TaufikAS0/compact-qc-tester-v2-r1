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

#include "serial_qc_protocol.h"
constexpr bool serialProtocolTests() {
  using namespace SerialQc;
  const uint8_t digits[] = {'1','2','3','4','5','6','7','8','9'};
  if (crc16(digits, 9) != 0x29b1) return false;
  uint8_t req[MAX_FRAME] = {}, reply[MAX_FRAME] = {};
  for (uint16_t seq = 0; seq < 4; ++seq) {
    size_t n = frame(req, REQUEST, 0x12345678, seq);
    size_t m = frame(reply, RESPONSE, 0x12345678, seq);
    if (!valid(req, n) || !valid(reply, m) || n != m || get16(req+10) != payloadSize(seq)) return false;
    for (size_t i = HEADER; i < n - 2; ++i) if ((req[i] ^ reply[i]) != 0xa5) return false;
    reply[HEADER] ^= 1;
    if (valid(reply, m)) return false;
    if (valid(req, n-1)) return false;
  }
  return frame(req, ACK, 1, 0) == 14 && valid(req, 14);
}
static_assert(serialProtocolTests(), "RS232 CRC/frame/transformed-payload invariants");

#include "output_state.h"
constexpr bool independentOutputsTest() {
  OutputState bank;
  if (bank.any()) return false;
  bank.set(11, true, 100);
  bank.set(12, true, 200);
  if (!bank.on[11] || !bank.on[12]) return false;
  bank.set(11, false, 300);
  if (bank.on[11] || !bank.on[12]) return false;
  bank.set(11, true, 400);
  if (!bank.expired(12, 1200, 1000) || bank.expired(11, 1200, 1000)) return false;
  bank.set(12, false, 1200);
  if (!bank.on[11]) return false;
  bank.clear();
  if (bank.any()) return false;
  bank.set(0, true, 0xfffffff0u);
  return bank.expired(0, 0x20u, 48u) && !bank.expired(0, 0x20u, 49u);
}
static_assert(independentOutputsTest(), "Independent output timers, OFF and millis wrap");

#include "five_point_math.h"
constexpr int qcFourPass[5]={1,1,1,1,0}, qcFivePass[5]={1,1,1,1,1}, qcOneFail[5]={1,1,-1,1,0};
static_assert(fivePointVerdict(qcFourPass)==0, "Four points cannot PASS five-point QC");
static_assert(fivePointVerdict(qcFivePass)==1, "Five confirmed points PASS");
static_assert(fivePointVerdict(qcOneFail)==-1, "One failed point fails aggregate");

#include "modbus_math.h"
constexpr uint8_t mbReadVector[]={1,3,0,0x6b,0,3};
static_assert(QcModbus::crc(mbReadVector,6)==0x1774, "Modbus published request CRC vector");
constexpr uint8_t mbValidResponse[]={1,4,2,0xff,0xff,0xb8,0x80};
constexpr uint8_t mbBadResponse[]={1,4,2,0xfe,0xff,0xb8,0x80};
static_assert(QcModbus::valid(mbValidResponse,7), "CRC-valid Modbus register frame");
static_assert(!QcModbus::valid(mbBadResponse,7), "Corruption rejects Modbus data");
static_assert(QcModbus::bytes(1,2)==1 && QcModbus::bytes(3,2)==4, "Bit vs register byte count");

#include "mcp_math.h"
static_assert(mcpMaskedByte(0x80,0x30,0x30)==0xb0, "Radio restoration preserves high TPL B7");
static_assert(mcpMaskedByte(0x50,0x30,0)==0x40, "Radio restoration preserves unrelated B6 and low TPL");
static_assert(mcpMaskedByte(0x8f,1,0)==0x8e, "LED update preserves other LEDs and watchdog");

static_assert(QcModbus::validate(mbValidResponse,7,1,4,1)==QcModbus::Response::Valid, "Matching request passes");
static_assert(QcModbus::validate(mbValidResponse,7,2,4,1)==QcModbus::Response::Slave, "Wrong slave rejected");
static_assert(QcModbus::validate(mbValidResponse,7,1,3,1)==QcModbus::Response::Function, "Wrong function rejected");
static_assert(QcModbus::validate(mbValidResponse,7,1,4,2)==QcModbus::Response::Length, "Wrong register count rejected");
static_assert(QcModbus::validate(mbValidResponse,4,1,4,1)==QcModbus::Response::Short, "Partial response cannot PASS");

#include "relay_profile.h"
constexpr bool relayInitializationTest() {
  // Model core3: Arduino write is ignored until GPIO ownership is configured.
  for (int index = 0; index < 2; ++index) {
    int latch = -1, enabledLevel = -1, phase = 0;
    bool configured = false;
    if (!initializeRelay(index,
        [&](int pin, int level) { if (pin != RELAY_PINS[index] || phase++ != 0) return false; latch = level; return true; },
        [&](int pin) { enabledLevel = latch; configured = pin == RELAY_PINS[index] && phase++ == 1; },
        [&](int pin, int level) { if (!configured || phase++ != 2) return false; latch = level; return pin == RELAY_PINS[index]; })) return false;
    if (phase != 3 || enabledLevel != relayLevel(index, false) || latch != enabledLevel) return false;
  }
  int callbacks = 0;
  bool accepted = initializeRelay(0, [](int, int) { return false; },
      [&](int) { ++callbacks; }, [&](int, int) { ++callbacks; return true; });
  return !accepted && callbacks == 0;
}
static_assert(relayInitializationTest(), "OFF preloaded before enable and reasserted after core3 GPIO registration");
static_assert(relayLevel(0,false)==0 && relayLevel(0,true)==1, "DO1 correction active HIGH");
static_assert(relayLevel(1,false)==0 && relayLevel(1,true)==1, "DO2 correction active HIGH");
