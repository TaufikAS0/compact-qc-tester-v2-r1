#include "qc_hardware.h"
#include "config.h"
#include "qc_session.h"
#include "tpl_watchdog.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Wire.h>
#include <esp_mac.h>

namespace {
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
bool oledReady = false, adcVerified = false, adcReady = false, adcValid = false, ahtValid = false;
float resistance = 0, amplifier = 1, zero = 0, temperature = 0, humidity = 0;
int16_t adcRaw = 0;
uint32_t sampledAt = 0, lastAdc = 0, lastOled = 0, lastAht = 0, ahtStarted = 0;
bool ahtPending = false, ahtVerified = false;
char serial[24] = "TMCUNKN";
String storedTop, storedBottom, identityDetail = "BELUM DIBACA";
uint32_t pdRequested = 0; // No software PDO request has been sent this boot.
bool readReg(uint8_t address, uint8_t reg, uint8_t *data, size_t len) {
  updateTplWatchdog();
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0 || Wire.requestFrom(address, (uint8_t)len) != len)
    return false;
  for (size_t i = 0; i < len; i++)
    data[i] = Wire.read();
  return true;
}
bool writeReg(uint8_t address, uint8_t reg, const uint8_t *data, size_t len) {
  updateTplWatchdog();
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data, len);
  return Wire.endTransmission() == 0;
}
bool eepromRead(uint16_t at, uint8_t *data, size_t len) {
  while (len) {
    size_t n = min(len, (size_t)32);
    updateTplWatchdog();
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write(at >> 8);
    Wire.write(at & 255);
    if (Wire.endTransmission(false) != 0 || Wire.requestFrom(EEPROM_ADDR, (uint8_t)n) != n)
      return false;
    for (size_t i = 0; i < n; i++)
      data[i] = Wire.read();
    data += n;
    at += n;
    len -= n;
  }
  return true;
}
uint32_t crc(const uint8_t *data, size_t n) {
  uint32_t c = 0xffffffff;
  while (n--) {
    c ^= *data++;
    for (int i = 0; i < 8; i++)
      c = (c >> 1) ^ (0xedb88320u & -(c & 1));
  }
  return ~c;
}
// Two 64-byte slots. CRC rejects torn writes; inactive slot is written first.
struct __attribute__((packed)) Identity {
  char magic[4];
  uint32_t generation;
  char top[20];
  char bottom[20];
  uint8_t reserved[12];
  uint32_t checksum;
};
static_assert(sizeof(Identity) == 64);
bool valid(const Identity &r) {
  return memcmp(r.magic, "QC01", 4) == 0 && r.checksum == crc((const uint8_t *)&r, 60) &&
         memchr(r.top, 0, 20) && memchr(r.bottom, 0, 20);
}
bool freeSlot(const Identity &r) {
  const uint8_t *p = (const uint8_t *)&r;
  for (size_t i = 0; i < 64; i++)
    if (p[i] != 255)
      return false;
  return true;
}
void loadIdentity() {
  Identity slots[2];
  if (!eepromRead(IDENTITY_BASE, (uint8_t *)slots, 128)) {
    identityDetail = "EEPROM read failed";
    return;
  }
  bool a = valid(slots[0]), b = valid(slots[1]);
  if (!a && !b) {
    identityDetail = "No valid QC identity";
    return;
  }
  const Identity &r =
      (b && (!a || (int32_t)(slots[1].generation - slots[0].generation) > 0)) ? slots[1] : slots[0];
  storedTop = r.top;
  storedBottom = r.bottom;
  identityDetail = "CRC valid; identity read from EEPROM";
}
const char *knownName(uint8_t a) {
  switch (a) {
  case 0x20:
    return "MCP1 MCP23017";
  case 0x21:
    return "MCP2 MCP23017";
  case 0x28:
    return "STUSB4500";
  case 0x38:
    return "AHT family";
  case 0x3c:
    return "OLED SSD1306";
  case 0x48:
    return "ADS (select model)";
  case 0x57:
    return "M24128 EEPROM";
  case 0x68:
    return "RTC (select model)";
  case 0x77:
    return "ATtiny404";
  default:
    return "Unknown I2C device";
  }
}
} // namespace

bool mcpBit(uint8_t address, uint8_t bit, bool level) {
  if (bit > 15)
    return false;
  uint8_t bank, dir, latch;
  uint8_t port = bit / 8, mask = 1u << (bit % 8);
  if (!readReg(address, 0x0a, &bank, 1) || (bank & 0xa0) ||
      !readReg(address, 0x14 + port, &latch, 1) || !readReg(address, port, &dir, 1))
    return false;
  latch = level ? (latch | mask) : (latch & ~mask);
  if (!writeReg(address, 0x14 + port, &latch, 1))
    return false;
  dir &= ~mask;
  return writeReg(address, port, &dir, 1);
}
size_t inputCount() { return sizeof(INPUT_PINS) / sizeof(INPUT_PINS[0]) + 2; }
const char *inputName(size_t i) {
  if (i < 9)
    return INPUT_NAMES[i];
  return i == 9 ? "Display button" : "Boot button";
}
bool inputPin(size_t i, int &pin) {
  if (i < 9) {
    pin = INPUT_PINS[i];
    return true;
  }
  if (i == 10) {
    pin = 0;
    return true;
  }
  pin = -1;
  return false;
}
bool readInput(size_t i, int &v) {
  if (i >= inputCount())
    return false;
  if (i < 9) {
    v = digitalRead(INPUT_PINS[i]);
    return true;
  }
  if (i == 10) {
    v = digitalRead(0);
    return true;
  }
  uint8_t gpio;
  if (!readReg(MCP1_ADDR, 0x13, &gpio, 1))
    return false;
  v = (gpio >> 3) & 1;
  return true;
}
const char *unitSerial() { return serial; }
void beginQcHardware() {
  uint8_t mac[6] = {};
  if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK)
    snprintf(serial, sizeof(serial), "TMC%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3],
             mac[4], mac[5]);
  pinMode(0, INPUT);
  uint8_t dir;
  if (tplWatchdogReady() && readReg(MCP1_ADDR, 1, &dir, 1)) {
    dir |= 8;
    writeReg(MCP1_ADDR, 1, &dir, 1);
  }
  Wire.beginTransmission(0x3c);
  if (Wire.endTransmission() == 0)
    oledReady = oled.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false);
  loadIdentity();
  qcRecord("OLED", oledReady ? "MENUNGGU USER" : "FAIL",
           oledReady ? "Display initialized; visual operator confirmation required"
                     : "OLED missing or initialization failed");
}
void updateQcHardware() {
  uint32_t now = millis();
  if (adcReady && now - lastAdc >= 150) {
    lastAdc = now;
    uint8_t data[2];
    adcValid = readReg(0x48, 0, data, 2);
    if (adcValid) {
      adcRaw = (int16_t)((data[0] << 8) | data[1]);
      sampledAt = now;
    }
  }
  if (ahtVerified && !ahtPending && now - lastAht >= 2000) {
    lastAht = now;
    uint8_t command[] = {0x33, 0};
    if (writeReg(0x38, 0xac, command, 2)) {
      ahtPending = true;
      ahtStarted = now;
    }
  }
  if (ahtPending && now - ahtStarted >= 90) {
    ahtPending = false;
    updateTplWatchdog();
    ahtValid = false;
    if (Wire.requestFrom((uint8_t)0x38, (uint8_t)7) == 7) {
      uint8_t b[7];
      for (auto &x : b)
        x = Wire.read();
      uint8_t c = 0xff;
      for (int i = 0; i < 6; i++) {
        c ^= b[i];
        for (int k = 0; k < 8; k++)
          c = (c & 0x80) ? (c << 1) ^ 0x31 : c << 1;
      }
      if (!(b[0] & 0x80) && (b[0] & 8) && c == b[6]) {
        humidity = ((uint32_t)b[1] << 12 | b[2] << 4 | b[3] >> 4) * 100.0 / 1048576;
        temperature = (((uint32_t)b[3] & 15) << 16 | b[4] << 8 | b[5]) * 200.0 / 1048576 - 50;
        ahtValid = isfinite(temperature) && humidity >= 0 && humidity <= 100;
      }
    }
  }
  if (oledReady && now - lastOled >= 700) {
    lastOled = now;
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.printf("QC %s\n%s\n", FW_VERSION, serial);
    const QcItem &item = qcDisplayItem();
    oled.printf("%s\n%s\n", item.name.c_str(), item.status.c_str());
    oled.println(qcDisplayDetail());
    oled.println(storedTop.isEmpty() ? HW_TOP : storedTop.c_str());
    oled.println(storedBottom.isEmpty() ? HW_BOTTOM : storedBottom.c_str());
    oled.display();
    updateTplWatchdog();
  }
}
void hardwareState(JsonObject d) {
  d["serial"] = serial;
  d["expectedHWTop"] = HW_TOP;
  d["expectedHWBottom"] = HW_BOTTOM;
  d["hwTop"] = storedTop;
  d["hwBottom"] = storedBottom;
  d["identityDetail"] = identityDetail;
  d["oledReady"] = oledReady;
  d["ahtVerified"] = ahtVerified;
  d["ahtValid"] = ahtValid;
  if (ahtValid) {
    d["temperatureC"] = temperature;
    d["humidityPercent"] = humidity;
  }
  float ma;
  int16_t raw;
  uint32_t age;
  d["adcVerified"] = adcVerified;
  d["shuntOhm"] = resistance;
  d["adcGain"] = amplifier;
  d["adcOffsetV"] = zero;
  d["adcModel"] = adcVerified ? "ADS1115" : "UNCONFIRMED";
  d["adcValid"] = currentSample(ma, raw, age);
  if (adcValid) {
    d["adcRaw"] = raw;
    d["adcAgeMs"] = age;
    d["currentMA"] = ma;
  }
  d["pdRequestedMV"] = pdRequested;
  uint8_t rdo[4], fsm;
  if (readReg(0x28, 0x29, &fsm, 1))
    d["pdState"] = fsm;
  if (readReg(0x28, 0x91, rdo, 4))
    d["pdRdo"] = ((uint32_t)rdo[3] << 24 | rdo[2] << 16 | rdo[1] << 8 | rdo[0]);
}
void scanBus(JsonArray a) {
  for (uint8_t address = 8; address < 120; address++) {
    updateTplWatchdog();
    Wire.beginTransmission(address);
    if (Wire.endTransmission() != 0)
      continue;
    auto v = a.add<JsonObject>();
    v["address"] = address;
    v["name"] = knownName(address);
    v["status"] = "DETECTED";
    uint8_t b[16];
    String detail = "ACK only; operator/protocol verification required";
    if (address == 0x20 || address == 0x21) {
      if (readReg(address, 0x0a, b, 1))
        detail = (b[0] & 0xa0) ? "MCP BANK/SEQOP incompatible"
                               : "MCP register read valid; GPIO requires stimulus";
    }
    if (address == 0x68 && readReg(address, 0, b, 7)) {
      bool plausible = (b[0] & 0x7f) <= 0x59 && (b[1] & 0x7f) <= 0x59;
      detail = plausible ? "RTC clock registers read; verify time and tick (model unconfirmed)"
                         : "RTC register values invalid";
      v["clockRawSeconds"] = b[0];
    }
    if (address == 0x57) {
      loadIdentity();
      detail = identityDetail;
    }
    if (address == 0x38)
      detail = ahtValid ? "AHT20-compatible measurement + CRC valid; compare reference"
                        : "Confirm AHT20 fitted and initialize from web before measurement";
    if (address == 0x48)
      detail = adcVerified ? "ADS1115 A0 selected; use calibrator test"
                           : "Select/verify ADS1115 model and transfer function in 4-20 panel";
    if (address == 0x3c)
      detail = oledReady ? "Display initialized; operator must inspect OLED"
                         : "Display initialization failed";
    if (address == 0x28 && readReg(address, 0x2f, b, 1)) {
      v["deviceId"] = b[0];
      detail = "PD device ID read; use request panel + external voltage measurement";
    }
    if (address == 0x77)
      detail = "ATtiny protocol unspecified; no commands sent; manual feedback";
    v["detail"] = detail;
    String item;
    switch (address) {
    case 0x20:
      item = "MCP1";
      break;
    case 0x21:
      item = "MCP2";
      break;
    case 0x28:
      item = "STUSB4500";
      break;
    case 0x38:
      item = "AHT";
      break;
    case 0x3c:
      item = "OLED";
      break;
    case 0x48:
      item = "ADS / 4-20mA";
      break;
    case 0x57:
      item = "EEPROM";
      break;
    case 0x68:
      item = "RTC";
      break;
    case 0x77:
      item = "ATtiny404";
      break;
    default:
      item = "I2C:" + String(address, HEX);
    }
    qcRecord(item, "DETECTED", detail);
  }
}
bool saveIdentity(const String &top, const String &bottom, String &detail) {
  if (top.length() == 0 || bottom.length() == 0 || top.length() > 19 || bottom.length() > 19) {
    detail = "Version must be 1..19 characters";
    return false;
  }
  Identity old[2];
  if (!eepromRead(IDENTITY_BASE, (uint8_t *)old, 128)) {
    detail = "EEPROM read failed";
    return false;
  }
  // Do not overwrite unrelated production bytes. A torn QC write is recognizable by magic.
  for (auto &r : old)
    if (!freeSlot(r) && memcmp(r.magic, "QC01", 4) != 0) {
      detail = "Reserved region contains foreign data; no write performed";
      return false;
    }
  int latest = -1;
  for (int i = 0; i < 2; i++)
    if (valid(old[i]) && (latest < 0 || (int32_t)(old[i].generation - old[latest].generation) > 0))
      latest = i;
  if (latest >= 0 && top == old[latest].top && bottom == old[latest].bottom) {
    loadIdentity();
    detail = "Identity already matches; no write";
    return true;
  }
  int target = latest == 0 ? 1 : 0;
  Identity record = {};
  memcpy(record.magic, "QC01", 4);
  record.generation = latest < 0 ? 1 : old[latest].generation + 1;
  top.toCharArray(record.top, 20);
  bottom.toCharArray(record.bottom, 20);
  record.checksum = crc((uint8_t *)&record, 60);
  uint16_t at = IDENTITY_BASE + target * 64;
  const uint8_t *data = (uint8_t *)&record;
  // 16-byte transactions never cross a 64-byte page boundary. Bounded ACK polling.
  for (size_t pos = 0; pos < 64; pos += 16) {
    updateTplWatchdog();
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write((at + pos) >> 8);
    Wire.write((at + pos) & 255);
    Wire.write(data + pos, 16);
    if (Wire.endTransmission() != 0) {
      detail = "EEPROM write failed (check WC)";
      return false;
    }
    uint32_t start = millis();
    bool ready = false;
    do {
      updateTplWatchdog();
      Wire.beginTransmission(EEPROM_ADDR);
      ready = Wire.endTransmission() == 0;
      yield();
    } while (!ready && millis() - start < 10);
    if (!ready) {
      detail = "EEPROM write timeout";
      return false;
    }
  }
  Identity check;
  if (!eepromRead(at, (uint8_t *)&check, 64) || memcmp(&record, &check, 64) != 0 || !valid(check)) {
    detail = "EEPROM readback/CRC mismatch";
    return false;
  }
  loadIdentity();
  detail = identityDetail;
  return true;
}
bool configureAht20() {
  uint8_t data[] = {8, 0};
  ahtVerified = writeReg(0x38, 0xbe, data, 2);
  lastAht = millis();
  return ahtVerified;
}
bool configureCurrent(bool verified, float shunt, float gain, float offset) {
  adcReady = adcValid = adcVerified = false;
  if (!verified || !isfinite(shunt) || !isfinite(gain) || !isfinite(offset) || shunt < 1 ||
      shunt > 1000 || gain <= 0 || gain > 100 || fabs(offset) > 6 ||
      0.004f * shunt * gain + offset < 0 || 0.020f * shunt * gain + offset > 3.3f)
    return false;
  uint8_t cfg[] = {0x42, 0x83}; // ADS1115 AIN0-GND, +/-4.096V, continuous 128SPS.
  if (!writeReg(0x48, 1, cfg, 2))
    return false;
  uint8_t readback[2];
  if (!readReg(0x48, 1, readback, 2) || (readback[0] & 0x7f) != cfg[0] || readback[1] != cfg[1])
    return false;
  resistance = shunt;
  amplifier = gain;
  zero = offset;
  lastAdc = millis();
  adcReady = adcVerified = true;
  return true;
}
bool currentSample(float &ma, int16_t &raw, uint32_t &age) {
  age = millis() - sampledAt;
  raw = adcRaw;
  ma = ((raw * 0.000125) - zero) / resistance / amplifier * 1000;
  return adcVerified && adcValid && age < SAMPLE_MAX_AGE_MS && raw != INT16_MAX && raw != INT16_MIN;
}
bool testEthernet(String &detail) {
  uint8_t mac[6];
  if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) {
    detail = "MAC unavailable";
    return false;
  }
  mac[0] = (mac[0] | 2) & ~1;
  digitalWrite(SD_CS, HIGH);
  Ethernet.init(ETH_CS);
  // Static test address avoids blocking DHCP. No web server is started on Ethernet.
  updateTplWatchdog();
  Ethernet.begin(mac, IPAddress(192, 0, 2, 2));
  updateTplWatchdog();
  auto hw = Ethernet.hardwareStatus();
  auto link = Ethernet.linkStatus();
  const char *controller = hw == EthernetW5500   ? "W5500"
                           : hw == EthernetW5200 ? "W5200"
                           : hw == EthernetW5100 ? "W5100"
                                                 : "NONE";
  const char *phy = link == LinkON    ? "ON"
                    : link == LinkOFF ? "OFF"
                                      : "UNKNOWN (manual cable/link fixture required)";
  detail = "Controller=" + String(controller) + ", link=" + String(phy) +
           "; SPI/controller check; no packet exchange";
  digitalWrite(ETH_CS, HIGH);
  return hw != EthernetNoHardware && link != LinkOFF;
}
bool requestPd(uint16_t mv, uint16_t ma, String &detail) {
  if (mv < 5000 || mv > 20000 || mv % 50 || ma < 10 || ma > 5000 || ma % 10) {
    detail = "Fixed PDO: 5000..20000mV step50; 10..5000mA step10";
    return false;
  }
  uint8_t id, port;
  if (!readReg(0x28, 0x2f, &id, 1) || (id != 0x25 && id != 0x21) ||
      !readReg(0x28, 0x0e, &port, 1) || !(port & 1)) {
    detail = "STUSB4500 ID/attachment unavailable";
    return false;
  }
  uint8_t old[12], count;
  if (!readReg(0x28, 0x85, old, 12) || !readReg(0x28, 0x70, &count, 1)) {
    detail = "Cannot snapshot PDO";
    return false;
  }
  uint8_t data[8];
  uint32_t pdo1 = (old[0] | old[1] << 8 | old[2] << 16 | (uint32_t)old[3] << 24);
  pdo1 = (pdo1 & ~0xfffffu) | (100u << 10) | (ma / 10);
  uint32_t pdo2 = (uint32_t)(mv / 50) << 10 | (ma / 10);
  for (int i = 0; i < 4; i++) {
    data[i] = pdo1 >> (8 * i);
    data[i + 4] = pdo2 >> (8 * i);
  }
  uint8_t n = mv == 5000 ? 1 : 2;
  bool ok = writeReg(0x28, 0x85, data, 8) && writeReg(0x28, 0x70, &n, 1);
  uint8_t verify[8], vc;
  ok = ok && readReg(0x28, 0x85, verify, 8) && !memcmp(data, verify, 8) &&
       readReg(0x28, 0x70, &vc, 1) && vc == n;
  if (!ok) {
    writeReg(0x28, 0x85, old, 12);
    writeReg(0x28, 0x70, &count, 1);
    detail = "PDO verification failed; restoration attempted";
    return false;
  }
  uint8_t header[] = {0x0d, 0}, command = 0x26;
  if (!writeReg(0x28, 0x51, header, 2) || !writeReg(0x28, 0x1a, &command, 1)) {
    detail = "PDO updated but renegotiation command failed";
    return false;
  }
  pdRequested = mv;
  detail = "Volatile PDO request sent; NOT NVM. Wait for negotiation, measure actual voltage, "
           "confirm manually. Source can fall back to 5V.";
  return true;
}
