#include "current_calibration.h"
#include "qc_hardware.h"
#include "qc_session.h"
#include "qc_math.h"
#include "current_qc.h"
#include <Preferences.h>
namespace {
struct Point {
  bool captured = false;
  double raw = 0, ma = 0, noise = 0;
  uint32_t at = 0;
};
Point points[4];
int pending = -1, samples = 0;
double sum = 0, squares = 0;
float reference = 0;
uint32_t started = 0, lastSample = 0;
String calDetail = "Belum dikalibrasi";
// Dedicated single NVS blob; the EEPROM PCB identity region is unchanged.
struct Record {
  uint32_t magic;
  double raw[4], ma[4], gain, offset;
  uint32_t checksum;
};
Record saved = {};
bool savedValid = false;
Preferences prefs;
uint32_t checksum(const Record &r) {
  const uint8_t *p = (const uint8_t *)&r;
  uint32_t c = 0xffffffff;
  for (size_t i = 0; i < offsetof(Record, checksum); ++i) {
    c ^= p[i];
    for (int b = 0; b < 8; ++b)
      c = (c >> 1) ^ (0xedb88320u & -(c & 1));
  }
  return ~c;
}
bool valid(const Record &r) {
  if (r.magic != 0x41444331 || r.checksum != checksum(r))
    return false;
  for (int i = 0; i < 4; ++i)
    if (!isfinite(r.raw[i]) || !isfinite(r.ma[i]) || r.ma[i] < 4 || r.ma[i] > 20)
      return false;
  auto f = fitFour(r.raw, r.ma);
  return f.valid && isfinite(r.gain) && isfinite(r.offset) && fabs(f.gain - r.gain) < 1e-10 &&
         fabs(f.offset - r.offset) < 1e-7;
}
} // namespace
void beginCurrentCalibration() {
  if (prefs.begin("qc-adc", false) && prefs.getBytesLength("cal-v1") == sizeof(saved)) {
    prefs.getBytes("cal-v1", &saved, sizeof(saved));
    savedValid = valid(saved);
    if (savedValid) {
      for (int i = 0; i < 4; ++i)
        points[i] = {true, saved.raw[i], saved.ma[i], 0, 0};
      calDetail = "Kalibrasi NVS dimuat; verifikasi dengan calibrator independen";
    } else
      calDetail = "CRC/model kalibrasi NVS tidak valid";
  }
}
bool calibratedCurrent(float raw, float &ma) {
  if (!savedValid)
    return false;
  ma = saved.gain * raw + saved.offset;
  return isfinite(ma);
}
void resetCurrentPoints() {
  for (auto &p : points)
    p = {};
  pending = -1;
  calDetail = "Titik capture direset; kalibrasi aktif tetap dipertahankan";
}
bool captureCurrentPoint(int index, float ma, String &out) {
  int16_t raw;
  uint32_t at;
  if (currentQcBusy() || index < 0 || index > 3 || !isfinite(ma) || ma < 4 || ma > 20 || pending >= 0 ||
      !rawCurrentSample(raw, at)) {
    out = "Capture ditolak: index/reference, ADC tidak segar, atau capture sedang berjalan";
    return false;
  }
  pending = index;
  samples = 0;
  sum = squares = 0;
  reference = ma;
  started = millis();
  lastSample = at;
  points[index] = {};
  calDetail = "Capture 16 sampel baru (~2.4 detik), tahan sinyal calibrator";
  out = calDetail;
  return true;
}
void updateCurrentCalibration() {
  if (pending < 0)
    return;
  if (millis() - started > 6000) {
    pending = -1;
    calDetail = "Capture timeout; periksa ADC";
    return;
  }
  int16_t raw;
  uint32_t at;
  if (!rawCurrentSample(raw, at) || at == lastSample)
    return;
  lastSample = at;
  sum += raw;
  squares += (double)raw * raw;
  ++samples;
  if (samples == 16) {
    double mean = sum / samples, noise = sqrt(fmax(0, squares / samples - mean * mean));
    if (noise > 30)
      calDetail = "Capture ditolak: sinyal tidak stabil (SD ADC >30 count)";
    else {
      points[pending] = {true, mean, reference, noise, millis()};
      calDetail = "Titik captured; belum menjadi bukti akurasi independen";
    }
    pending = -1;
  }
}
bool saveCurrentCalibration(float maximum, String &out) {
  if (currentQcBusy() || pending >= 0 || !isfinite(maximum) || maximum < 0.01 || maximum > 1) {
    out = "Capture aktif atau batas residual tidak valid";
    return false;
  }
  Record r = {};
  r.magic = 0x41444331;
  for (int i = 0; i < 4; ++i) {
    if (!points[i].captured) {
      out = "Capture keempat titik dahulu";
      return false;
    }
    r.raw[i] = points[i].raw;
    r.ma[i] = points[i].ma;
  }
  auto f = fitFour(r.raw, r.ma);
  if (!f.valid || f.maxError > maximum || f.r2 < 0.995) {
    out = "Fit ditolak: span/duplikasi/residual/R2 tidak memenuhi batas";
    return false;
  }
  r.gain = f.gain;
  r.offset = f.offset;
  r.checksum = checksum(r);
  Record check = {};
  if (prefs.putBytes("cal-v1", &r, sizeof(r)) != sizeof(r) ||
      prefs.getBytes("cal-v1", &check, sizeof(check)) != sizeof(check) || !valid(check) ||
      memcmp(&r, &check, sizeof(r))) {
    out = "NVS write/readback failed; kalibrasi RAM sebelumnya dipertahankan";
    return false;
  }
  saved = r;
  savedValid = true;
  calDetail =
      "Gain/offset tersimpan NVS + CRC/readback. Uji ulang titik independen untuk QC akurasi.";
  out = calDetail;
  qcRecord("ADS / 4-20mA", "CALIBRATED", calDetail);
  return true;
}
void calibrationState(JsonObject d) {
  d["active"] = savedValid;
  d["pending"] = pending;
  d["samples"] = samples;
  d["detail"] = calDetail;
  if (savedValid) {
    d["gain"] = saved.gain;
    d["offset"] = saved.offset;
  }
  double x[4], y[4];
  bool complete = true;
  auto a = d["points"].to<JsonArray>();
  for (int i = 0; i < 4; ++i) {
    auto p = a.add<JsonObject>();
    p["captured"] = points[i].captured;
    p["raw"] = x[i] = points[i].raw;
    p["ma"] = y[i] = points[i].ma;
    p["noise"] = points[i].noise;
    p["uptimeMs"] = points[i].at;
    complete &= points[i].captured;
  }
  if (complete) {
    auto f = fitFour(x, y);
    d["fitValid"] = f.valid;
    d["fitGain"] = f.gain;
    d["fitOffset"] = f.offset;
    d["r2"] = f.r2;
    d["maxResidualMA"] = f.maxError;
  }
}

uint32_t currentCalibrationId() { return savedValid ? saved.checksum : 0; }
