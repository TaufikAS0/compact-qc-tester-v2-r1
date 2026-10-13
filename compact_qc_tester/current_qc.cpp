#include "current_qc.h"
#include "five_point_math.h"
#include "current_calibration.h"
#include "qc_hardware.h"
#include "qc_session.h"
#include "ota_manager.h"
namespace {
struct Point { bool captured = false; int verdict = 0; double raw = 0, measured = 0, target = 0, noise = 0, tolerance = 0; uint32_t at = 0, calibrationId = 0; };
Point points[5];
int pending = -1, samples = 0;
uint32_t started = 0, sampleAt = 0, calibrationId = 0;
double sum = 0, squares = 0;
float target = 0, tolerance = .1;
String analogDetail = "Apply calibrator; capture five independent QC points";
void record() {
  int passed = 0, verdicts[5] = {}; int at=0;
  for (auto &p : points) { passed += p.verdict == 1; verdicts[at++]=p.verdict; }
  int result = fivePointVerdict(verdicts);
  String evidence; for (int i=0;i<5;++i) if(points[i].captured) { auto &p=points[i]; evidence += "; P"+String(i+1)+" target="+String(p.target,3)+" measured="+String(p.measured,3)+" raw="+String(p.raw,2)+" tol="+String(p.tolerance,3)+" verdict="+String(p.verdict)+" cal="+String(p.calibrationId,HEX)+" uptimeMs="+String(p.at); }
  qcRecord("ADS / 4-20mA", result < 0 ? "FAIL" : result > 0 ? "PASS" : "MENUNGGU USER",
           String(passed) + "/5 points sesuai; " + analogDetail + evidence);
}
void response(WebServer &web, bool ok, const String &message) {
  JsonDocument d; d[ok ? "analogDetail" : "error"] = message; d["ok"] = ok;
  String body; serializeJson(d, body); web.send(ok ? 200 : 409, "application/json", body);
}
bool index(WebServer &web, int &n) {
  String s = web.arg("index"); if (s.length() != 1 || s[0] < '0' || s[0] > '4') return false;
  n = s[0] - '0'; return true;
}
bool number(WebServer &web, const char *key, float low, float high, float &n) {
  String s = web.arg(key); char *end; n = strtof(s.c_str(), &end);
  return end != s.c_str() && !*end && isfinite(n) && n >= low && n <= high;
}
}
void resetCurrentQc() {
  for (auto &p : points) p = {};
  pending = -1; samples = 0; analogDetail = "QC points reset; active calibration retained";
}
void beginCurrentQc(WebServer &web, bool (*authorize)()) {
  web.on("/api/current/qc/capture", HTTP_POST, [&web, authorize] {
    if (!authorize()) return;
    if (webOtaBusy()) { response(web, false, "OTA in progress"); return; }
    int n; int16_t raw; uint32_t at; float ma, tol, calibrated;
    JsonDocument cal; calibrationState(cal.to<JsonObject>());
    if (!index(web, n) || !number(web, "ma", 4, 20, ma) || !number(web, "tolerance", .01, 1, tol) ||
        pending >= 0 || cal["pending"].as<int>() >= 0 || pdNegotiating() ||
        !rawCurrentSample(raw, at) || !calibratedCurrent(raw, calibrated)) {
      response(web, false, "QC capture requires fresh ADC, saved calibration, valid point and no competing capture/PD"); return;
    }
    // Independent targets: duplicate points cannot certify five-point span.
    for (int i = 0; i < 5; ++i) if (i != n && points[i].captured && fabs(points[i].target - ma) < .01) {
      response(web, false, "QC reference must differ from other captured points"); return;
    }
    pending = n; points[n] = {}; samples = 0; sum = squares = 0;
    target = ma; tolerance = tol; started = millis(); sampleAt = at;
    calibrationId = currentCalibrationId(); analogDetail = "Capturing16 fresh ADC readings; hold calibrator stable";
    record(); response(web, true, analogDetail);
  });
  web.on("/api/current/qc/confirm", HTTP_POST, [&web, authorize] {
    if (!authorize()) return;
    if (webOtaBusy()) { response(web, false, "OTA in progress"); return; }
    int n; String verdict = web.arg("verdict");
    if (!index(web, n) || pending >= 0 || !points[n].captured || points[n].calibrationId != currentCalibrationId() ||
        (verdict != "PASS" && verdict != "FAIL")) {
      response(web, false, "Capture this point using active calibration before confirmation"); return;
    }
    bool within = fabs(points[n].measured - points[n].target) <= points[n].tolerance;
    if (verdict == "PASS" && !within) {
      points[n].verdict = -1; analogDetail = "Measured value outside captured tolerance"; record();
      response(web, false, analogDetail); return;
    }
    points[n].verdict = verdict == "PASS" ? 1 : -1;
    analogDetail = "User " + verdict + " point" + String(n + 1); record(); response(web, true, analogDetail);
  });
  web.on("/api/current/qc/reset", HTTP_POST, [&web, authorize] {
    if (!authorize()) return;
    if (webOtaBusy()) { response(web, false, "OTA in progress"); return; }
    resetCurrentQc(); record(); response(web, true, analogDetail);
  });
}
void updateCurrentQc() {
  uint32_t id = currentCalibrationId();
  bool changed = pending >= 0 && id != calibrationId;
  for (auto &p : points) changed |= p.captured && p.calibrationId != id;
  if (changed) { resetCurrentQc(); analogDetail = "Calibration changed; all prior QC evidence invalidated"; record(); return; }
  if (pending < 0) return;
  if (millis() - started >= 6000) {
    pending = -1; analogDetail = "QC capture timeout; check ADC"; qcRecord("ADS / 4-20mA", "FAIL", analogDetail); return;
  }
  int16_t raw; uint32_t at;
  if (!rawCurrentSample(raw, at) || at == sampleAt) return;
  sampleAt = at; sum += raw; squares += double(raw) * raw;
  if (++samples < 16) return;
  double mean = sum / samples, noise = sqrt(fmax(0, squares / samples - mean * mean));
  float measured;
  if (noise > 30 || !calibratedCurrent(mean, measured)) {
    pending = -1; analogDetail = "QC capture rejected: unstable ADC/calibration";
    qcRecord("ADS / 4-20mA", "FAIL", analogDetail); return;
  }
  points[pending] = {true, 0, mean, measured, target, noise, tolerance, millis(), calibrationId};
  pending = -1; analogDetail = "Captured; confirm against external calibrator"; record();
}
void currentQcState(JsonObject d) {
  d["pending"] = pending; d["samples"] = samples; d["detail"] = analogDetail;
  auto list = d["points"].to<JsonArray>();
  for (auto &p : points) {
    auto q = list.add<JsonObject>(); q["captured"] = p.captured;
    q["status"] = p.verdict == 1 ? "PASS" : p.verdict == -1 ? "FAIL" : "BELUM DIUJI";
    if (!p.captured) continue;
    q["targetMA"] = p.target; q["measuredMA"] = p.measured; q["raw"] = p.raw;
    q["noise"] = p.noise; q["errorMA"] = p.measured - p.target; q["toleranceMA"] = p.tolerance;
    q["uptimeMs"] = p.at; q["calibrationId"] = p.calibrationId;
  }
}

bool currentQcBusy() { return pending >= 0; }
