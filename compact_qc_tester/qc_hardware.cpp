#include "qc_hardware.h"
#include "aht10_service.h"
#include "current_qc.h"
#include "mcp_math.h"
#include "config.h"
#include "qc_session.h"
#include "current_calibration.h"
#include "tpl_watchdog.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Wire.h>
#include <esp_mac.h>

namespace {
String identityRawHex, identitySlots[2];
String eepromError;
bool identityReadable = false;
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
bool oledReady = false, adcVerified = false, adcReady = false, adcValid = false, ahtValid = false;
float resistance = 0, amplifier = 1, zero = 0, temperature = 0, humidity = 0;
int16_t adcRaw = 0;
uint32_t sampledAt = 0, lastAdc = 0, lastOled = 0, lastAht = 0, ahtStarted = 0;
bool ahtPending = false, ahtVerified = false;
char serial[24] = "TMCUNKN";
bool identityValid = false;
uint32_t identityGeneration = 0;
String storedTop, storedBottom, identityDetail = "BELUM DIBACA";
bool rawReadReg(uint8_t address, uint8_t reg, uint8_t *data, size_t len) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0 || Wire.requestFrom(address, (uint8_t)len) != len)
    return false;
  for (size_t i = 0; i < len; i++)
    data[i] = Wire.read();
  return true;
}
bool rawWriteReg(uint8_t address, uint8_t reg, const uint8_t *data, size_t len) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data, len);
  return Wire.endTransmission() == 0;
}
bool readReg(uint8_t address, uint8_t reg, uint8_t *data, size_t len) {
  updateTplWatchdog(); return rawReadReg(address,reg,data,len);
}
bool writeReg(uint8_t address,uint8_t reg,const uint8_t *data,size_t len) {
  updateTplWatchdog(); return rawWriteReg(address,reg,data,len);
}
bool eepromRead(uint16_t at, uint8_t *data, size_t len) {
  eepromError="";
  if(size_t(at)+len>EEPROM_BYTES) { eepromError="EEPROM read address outside16KB"; return false; }
  while (len) {
    size_t n = min(len, (size_t)32);
    updateTplWatchdog();
    Wire.beginTransmission(EEPROM_ADDR);
    Wire.write(at >> 8);
    Wire.write(at & 255);
    uint8_t wireError=Wire.endTransmission(false);
    if(wireError) { eepromError="EEPROM0x57 address phase failed at0x"+String(at,HEX)+"; Wire error="+String(wireError); return false; }
    size_t got=Wire.requestFrom(EEPROM_ADDR,(uint8_t)n);
    if(got!=n) { while(Wire.available()) Wire.read(); eepromError="EEPROM short read at0x"+String(at,HEX)+": "+String(got)+"/"+String(n)+" bytes"; return false; }
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
  identityReadable = false; identityRawHex=""; identitySlots[0]=identitySlots[1]="READ FAILED";
  identityValid = false;
  storedTop = "";
  storedBottom = "";
  identityGeneration = 0;
  Identity slots[2];
  if (!eepromRead(IDENTITY_BASE, (uint8_t *)slots, 128)) {
    identityDetail = eepromError;
    return;
  }
  identityReadable = true;
  for(int i=0;i<2;++i){ identitySlots[i]=valid(slots[i])?"CRC VALID":freeSlot(slots[i])?"EMPTY FF":memcmp(slots[i].magic,"QC01",4)==0?"QC RECORD CRC INVALID":"FOREIGN DATA - PROTECTED"; const uint8_t *bytes=(const uint8_t *)&slots[i]; for(int n=0;n<64;++n){char hex[4];snprintf(hex,sizeof(hex),"%02X ",bytes[n]);identityRawHex+=hex;} identityRawHex+="\n"; }
  bool a = valid(slots[0]), b = valid(slots[1]);
  if (!a && !b) {
    identityDetail = "EEPROM read OK; no valid QC identity. Slot1="+identitySlots[0]+"; Slot2="+identitySlots[1]+". Write identity if empty; foreign bytes preserved.";
    return;
  }
  const Identity &r =
      (b && (!a || (int32_t)(slots[1].generation - slots[0].generation) > 0)) ? slots[1] : slots[0];
  identityValid = true;
  identityGeneration = r.generation;
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
  updateTplWatchdog(); // One service before the complete cooperative RMW transaction.
  if (bit > 15)
    return false;
  uint8_t bank, dir, latch;
  uint8_t port = bit / 8, mask = 1u << (bit % 8);
  if (!rawReadReg(address, 0x0a, &bank, 1) || (bank & 0xa0) ||
      !rawReadReg(address, 0x14 + port, &latch, 1) || !rawReadReg(address, port, &dir, 1))
    return false;
  latch = level ? (latch | mask) : (latch & ~mask);
  if (!rawWriteReg(address, 0x14 + port, &latch, 1))
    return false;
  dir &= ~mask;
  if (!rawWriteReg(address, port, &dir, 1)) return false;
  uint8_t actualLatch, actualDir;
  return rawReadReg(address, 0x14 + port, &actualLatch, 1) &&
         rawReadReg(address, port, &actualDir, 1) &&
         (actualLatch & mask) == (latch & mask) && !(actualDir & mask);
}
bool mcpRestorePortBits(uint8_t address,uint8_t port,uint8_t mask,uint8_t oldLatch,uint8_t oldDir) {
  if(port>1) return false;
  updateTplWatchdog(); // No TPL callback between snapshot and masked writes/readback.
  uint8_t bank,latch,dir,checkLatch,checkDir;
  if(!rawReadReg(address,0x0a,&bank,1)||(bank&0xa0)||!rawReadReg(address,0x14+port,&latch,1)||!rawReadReg(address,port,&dir,1))return false;
  latch=mcpMaskedByte(latch,mask,oldLatch);dir=mcpMaskedByte(dir,mask,oldDir);
  return rawWriteReg(address,0x14+port,&latch,1)&&rawWriteReg(address,port,&dir,1)&&rawReadReg(address,0x14+port,&checkLatch,1)&&rawReadReg(address,port,&checkDir,1)&&(checkLatch&mask)==(oldLatch&mask)&&(checkDir&mask)==(oldDir&mask);
}
size_t inputCount() { return 7; }
const char *inputName(size_t i) {
  static const char *names[] = {"DI1",      "DI2",         "DI3",           "DI4",
                                "DMM mode", "Normal mode", "Webserver mode"};
  return i < 7 ? names[i] : "Invalid";
}
bool inputPin(size_t i, int &pin) {
  if (i < 4)
    pin = INPUT_PINS[i];
  else if (i == 4)
    pin = 2;
  else if (i == 6)
    pin = 39;
  else
    return false;
  return true;
}
bool readInput(size_t i, int &v) {
  if (i >= 7)
    return false;
  if (i == 5)
    v = digitalRead(2) == LOW && digitalRead(39) == LOW;
  else {
    int pin;
    if (!inputPin(i, pin))
      return false;
    v = digitalRead(pin);
  }
  return true;
}
bool readIdentity(String &detail) {
  loadIdentity();
  detail = identityDetail;
  return identityValid;
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
  configureAht10();
  configureCurrent(true, 100, 1,
                   0); // User confirmed ADS1115; provisional conversion is labelled uncalibrated.
  beginCurrentCalibration();
  qcRecord("OLED", oledReady ? "MENUNGGU USER" : "FAIL",
           oledReady ? "Display initialized; visual operator confirmation required"
                     : "OLED missing or initialization failed");
}
void updateQcHardware() {
  updatePd();
  if (pdNegotiating())
    return; // Prioritize short-lived Source_Capabilities over OLED/ADC work.
  uint32_t now = millis();
  if (adcReady && qcTestEnabled("ADS / 4-20mA") && now - lastAdc >= 150) {
    lastAdc = now;
    uint8_t data[2];
    adcValid = readReg(0x48, 0, data, 2);
    if (adcValid) {
      adcRaw = (int16_t)((data[0] << 8) | data[1]);
      sampledAt = now;
    }
  }
  if(qcTestEnabled("ADS / 4-20mA")) { updateCurrentCalibration(); updateCurrentQc(); }
  updateAht10();
  if (oledReady && now - lastOled >= 700) {
    lastOled = now;
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.printf("QC %s\n%s\n", FW_VERSION, serial);
    const QcItem &item = qcDisplayItem();
    oled.printf("%s\n%s\n", item.name.c_str(), item.status.c_str());
    // Build UTC rotates onto its own screen so all 20 timestamp characters fit.
    if ((now / 7000) % 2) {
      oled.clearDisplay();
      oled.setCursor(0, 0);
      oled.printf("QC %s\nBuild UTC\n%s\n", FW_VERSION, FW_BUILD_UTC);
    } else
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
  d["identityValid"] = identityValid;
  d["identityGeneration"] = identityGeneration;
  d["identityReadable"] = identityReadable; d["identityRegionHex"] = identityRawHex;
  auto slots = d["identitySlots"].to<JsonArray>(); for(auto &slot:identitySlots) slots.add(slot);
  if(qcTestEnabled("RTC")) rtcState(d["rtc"].to<JsonObject>());
  else { auto rtc=d["rtc"].to<JsonObject>(); rtc["valid"]=false; rtc["detail"]="RTC test OFF in selected profile"; }
  calibrationState(d["calibration"].to<JsonObject>());
  d["oledReady"] = oledReady;
  aht10State(d);
  currentQcState(d["currentQc"].to<JsonObject>());
  float ma;
  int16_t raw;
  uint32_t age;
  d["adcVerified"] = adcVerified;
  d["shuntOhm"] = resistance;
  d["adcGain"] = amplifier;
  d["adcOffsetV"] = zero;
  d["adcModel"] = adcVerified ? "ADS1115" : "UNCONFIRMED";
  uint32_t sampleAt;
  bool rawValid = rawCurrentSample(raw, sampleAt);
  d["adcValid"] = rawValid;
  bool converted = currentSample(ma, raw, age);
  if (rawValid) {
    d["adcRaw"] = raw;
    d["adcAgeMs"] = age;
    d["adcVoltage"] = raw * 0.000125;
    if (converted)
      d["currentMA"] = ma;
    d["currentCalibrated"] = calibratedCurrent(raw, ma);
  }
  pdState(d["pd"].to<JsonObject>());
}
void scanBus(JsonArray a) {
  for (uint8_t address = 8; address < 120; ++address) {
    updateTplWatchdog();
    Wire.beginTransmission(address);
    if (Wire.endTransmission() != 0)
      continue;
    auto v = a.add<JsonObject>();
    v["address"] = address;
    v["name"] = knownName(address);
    v["status"] = "DETECTED";
  }
}

bool saveIdentity(const String &top, const String &bottom, String &detail) {
  if (top.length() == 0 || bottom.length() == 0 || top.length() > 19 || bottom.length() > 19) {
    detail = "Version must be 1..19 characters";
    return false;
  }
  Identity old[2];
  if (!eepromRead(IDENTITY_BASE, (uint8_t *)old, 128)) {
    detail = eepromError;
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
    size_t queued=Wire.write(data+pos,16);
    uint8_t wireError=Wire.endTransmission();
    if (queued!=16 || wireError) {
      detail = "EEPROM write transfer failed at0x"+String(at+pos,HEX)+"; Wire error="+String(wireError)+"; queued="+String(queued)+"/16; check WC/write-protect, model and power (WC level not measured)";
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
      detail = "EEPROM write ACK timeout10ms at0x"+String(at+pos,HEX)+"; check power/bus";
      return false;
    }
  }
  Identity check;
  if (!eepromRead(at, (uint8_t *)&check, 64)) {
    detail = eepromError;
    loadIdentity();
    return false;
  }
  if(memcmp(&record,&check,64)!=0 || !valid(check)) {
    detail="EEPROM write acknowledged but readback/CRC mismatched; check WC/write-protect and power";
    loadIdentity();
    return false;
  }
  loadIdentity();
  detail = identityDetail;
  return true;
}
bool configureAht10() { return startAht10(); }
bool configureCurrent(bool verified, float shunt, float gain, float offset) {
  if (currentQcBusy()) return false;
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
bool rawCurrentSample(int16_t &raw, uint32_t &at) {
  raw = adcRaw;
  at = sampledAt;
  return adcVerified && adcValid && millis() - sampledAt < SAMPLE_MAX_AGE_MS && raw >= 0 &&
         raw < 26400; // AIN0 conservative 0..3.3 V.
}
bool currentSample(float &ma, int16_t &raw, uint32_t &age) {
  uint32_t at;
  bool ok = rawCurrentSample(raw, at);
  age = millis() - at;
  if (!calibratedCurrent(raw, ma))
    ma = ((raw * 0.000125) - zero) / resistance / amplifier * 1000;
  return ok && isfinite(ma);
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
  return hw != EthernetNoHardware && link == LinkON;
}
bool qcReadReg(uint8_t a, uint8_t r, uint8_t *data, size_t n) { return readReg(a, r, data, n); }
bool qcWriteReg(uint8_t a, uint8_t r, const uint8_t *data, size_t n) {
  return writeReg(a, r, data, n);
}
void rtcState(JsonObject d) {
  uint8_t b[19];
  if (!readReg(0x68, 0, b, 19)) {
    d["valid"] = false;
    d["detail"] = "DS3231 read failed";
    qcRecord("RTC", "FAIL", "DS3231 read failed");
    return;
  }
  auto dec = [](uint8_t x) {
    return (x >> 4) <= 9 && (x & 15) <= 9 ? (x >> 4) * 10 + (x & 15) : -1;
  };
  int sec = dec(b[0] & 127), minute = dec(b[1] & 127), hour;
  if (b[2] & 64) {
    int h = dec(b[2] & 31);
    hour = h < 1 || h > 12 ? -1 : h % 12 + ((b[2] & 32) ? 12 : 0);
  } else
    hour = dec(b[2] & 63);
  int day = dec(b[4] & 63), month = dec(b[5] & 31), year = dec(b[6]);
  bool ok = sec >= 0 && sec < 60 && minute >= 0 && minute < 60 && hour >= 0 && hour < 24 &&
            day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 0;
  d["valid"] = ok;
  qcRecord("RTC", ok ? "PASS" : "FAIL",
           ok ? "DS3231 BCD clock and temperature read; OSF status in RTC card"
              : "Invalid BCD clock");
  d["oscillatorStopped"] = (b[15] & 128) != 0;
  d["temperatureC"] = (int8_t)b[17] + (b[18] >> 6) * .25;
  if (ok) {
    char t[24];
    snprintf(t, sizeof(t), "%04d-%02d-%02d %02d:%02d:%02d", 2000 + year + ((b[5] & 128) ? 100 : 0),
             month, day, hour, minute, sec);
    d["time"] = t;
  }
  d["detail"] = !ok             ? "Invalid BCD clock"
                : (b[15] & 128) ? "OSF set: time may be invalid; check battery/time"
                                : "DS3231 clock/temperature read; timezone is not stored by RTC";
}

bool identityBusReadable() { return identityReadable; }
