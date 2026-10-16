#include "qc_session.h"
#include "qc_blob.h"
#include "rtc_qc.h"
#include "config.h"
#include "qc_hardware.h"
#include "qc_profiles.h"
#include "qc_recipes.h"
#include "current_calibration.h"
#include "current_qc.h"
#include "ota_manager.h"
#include <math.h>
namespace {
bool autoSave=false,dirty=false;uint32_t dirtyAt=0,lastSave=0;String progressDetail="Belum ada checkpoint; simpan progres sebelum tes panjang";String savedSession;JsonDocument saved;
bool idle(){JsonDocument c;calibrationState(c.to<JsonObject>());return !webOtaBusy()&&!pdNegotiating()&&!currentQcBusy()&&c["pending"].as<int>()<0;}
}
void beginQcProgress(){String why;if(qcLoadBlob("qc-progress","session-v1",saved,why)){savedSession=saved["sessionId"].as<String>();progressDetail="Checkpoint ditemukan; pilih Pulihkan secara eksplisit";}else progressDetail=why;}
void qcProgressDirty(){if(!dirty)dirtyAt=millis();dirty=true;}
void qcProgressState(JsonObject d){d["schema"]="QC-PROGRESS-1";d["available"]=!savedSession.isEmpty();d["sessionId"]=savedSession;d["autoSave"]=autoSave;d["dirty"]=dirty;d["detail"]=progressDetail;d["storedBuildIdentity"]=saved["buildIdentity"];d["storedProfile"]=saved["profile"];}
void qcProgressNewSession(){autoSave=false;dirty=false;}
bool qcSaveProgress(String &why){
 if(!qcProfileSelected()||!idle()){why="Select profile / wait for capture before checkpoint";return false;}
 JsonDocument next;next["schema"]="QC-PROGRESS-1";next["serial"]=unitSerial();next["buildIdentity"]=FW_BUILD_ID;next["calibrationId"]=currentCalibrationId();next["profile"]=qcVariantName();next["profileRevision"]=activeQcProfileRevision();next["profileMask"]=activeQcProfileMask();next["recipeIdentity"]=activeQcRecipeIdentity();
 qcCaptureProgress(next.as<JsonObject>());rtcExportEvidence(next["rtcEvidence"].to<JsonObject>());
 if(next["schema"]!="QC-PROGRESS-1"||next["buildIdentity"]!=FW_BUILD_ID||next["sessionId"].as<String>()!=qcSessionId()||next["profile"].as<String>()!=qcVariantName()||next["calibrationId"].as<uint32_t>()!=currentCalibrationId()){why="Checkpoint header invalid; previous stored progress preserved";progressDetail=why;return false;}
 if(!qcSaveBlob("qc-progress","session-v1",next,why)){progressDetail=why;return false;}
 saved.set(next);savedSession=next["sessionId"].as<String>();autoSave=true;dirty=false;lastSave=millis();progressDetail=why;return true;
}
bool qcResumeProgress(String &why,const String &upgradeFromBuild){
 if(qcProfileSelected()){why="Resume only before selecting a new session; do not overwrite active QC";return false;}
 bool upgrading=!upgradeFromBuild.isEmpty();
 const bool knownUpgrade=upgradeFromBuild=="QC_BUILD=1.17.0|2026-10-16T11:59:37Z" || upgradeFromBuild=="QC_BUILD=1.16.0|2026-10-16T06:31:10Z" || upgradeFromBuild=="QC_BUILD=1.11.0|2026-10-15T08:16:55Z";
 if(upgrading&&(String(FW_VERSION)!="1.17.1"||!knownUpgrade||saved["buildIdentity"]!=upgradeFromBuild||rtcRunActive()||saved["rtcEvidence"]["clockRevision"].as<uint32_t>()!=rtcClockRevision())){why="Only explicit exact verified1.11.0/1.16.0/1.17.0 checkpoint upgrade without active run/clock change is allowed";return false;}
 if(!idle()||savedSession.isEmpty()||saved["schema"]!="QC-PROGRESS-1"||saved["serial"].as<String>()!=unitSerial()||(!upgrading&&saved["buildIdentity"]!=FW_BUILD_ID)||saved["calibrationId"].as<uint32_t>()!=currentCalibrationId()){why="Checkpoint identity/build/calibration differs or unavailable; download saved evidence, do not mix";return false;}
 JsonDocument current;qcProfilesState(current.to<JsonObject>());qcRecipesState(current.as<JsonObject>());String profile=saved["profile"].as<String>();bool found=false;
 for(JsonObject p:current["profiles"].as<JsonArray>())if(p["id"]==profile&&p["revision"]==saved["profileRevision"])found=true;
 bool recipeFound=false;for(JsonObject p:current["recipes"].as<JsonArray>())if(p["id"]==profile && String(p["revision"].as<unsigned long>())+":"+p["crc"].as<String>()==saved["recipeIdentity"].as<String>())recipeFound=true;
 if(!found||!recipeFound||!qcRecipesReady()){why="Saved profile/recipe revision unavailable";return false;}
 // Validate recipe identity before selecting anything. select only after complete structural validation.
 if(!qcValidateProgress(saved.as<JsonObjectConst>(),why))return false;
 if(upgrading){current.clear();String prior=saved["evidence"]["upgradeFromBuild"]|"",previous=saved["evidence"]["previousUpgradeFromBuild"]|"";saved["buildIdentity"]=FW_BUILD_ID;if(prior.length())saved["evidence"]["previousUpgradeFromBuild"]=prior;saved["evidence"]["upgradeFromBuild"]=upgradeFromBuild;
  if(!qcSaveBlob("qc-progress","session-v1",saved,why)){saved["buildIdentity"]=upgradeFromBuild;if(prior.length())saved["evidence"]["upgradeFromBuild"]=prior;else saved["evidence"].remove("upgradeFromBuild");if(previous.length())saved["evidence"]["previousUpgradeFromBuild"]=previous;else saved["evidence"].remove("previousUpgradeFromBuild");return false;}}
 if(!selectQcProfile(profile)||activeQcProfileMask()!=saved["profileMask"].as<String>()||activeQcRecipeIdentity()!=saved["recipeIdentity"].as<String>()){why="Saved profile/mask/recipe differs; new session required";return false;}
 qcRestoreProgress(saved.as<JsonObjectConst>());rtcRestoreEvidence(saved["rtcEvidence"]);autoSave=true;dirty=false;progressDetail="Completed evidence restored; outputs OFF, unfinished tests must be repeated";why=progressDetail;return true;
}
void updateQcProgress(){if(autoSave&&dirty&&idle()&&millis()-dirtyAt>=2000&&millis()-lastSave>=60000){String why;qcSaveProgress(why);lastSave=millis();}}
void qcSavedProgress(JsonObject d){d.set(saved.as<JsonObject>());}
void qcReleaseProgressCache(){saved.clear();}
bool qcImportLegacy(JsonDocument &original,String &why){
 JsonObjectConst old=original.as<JsonObjectConst>();
 // One-time explicit bridge for the real1.9.1 RAM session. CRC is integrity/traceability, not a signature.
 if(qcProfileSelected()){why="Legacy import requires unselected device";return false;}
 if(old["version"]!="1.9.1"){why="Legacy source version missing/different";return false;}
 if(old["hardware"]["serial"].as<String>()!=unitSerial()){why="Legacy source serial missing/different";return false;}
 if(!idle()){why="Legacy import blocked by capture/PD/OTA or unavailable calibration state";return false;}
 if(rtcRunActive()){why="Legacy import blocked by active RTC run";return false;}
 auto q=old["qc"].as<JsonObjectConst>();String profile=q["variant"].as<String>();
 if(!q["profileSelected"].as<bool>()||old["buildIdentity"].as<String>()!=String("QC_BUILD=1.9.1|")+old["buildUTC"].as<String>()){why="Legacy build/profile invalid";return false;}
 String canonical=String("QC1|1.9.1|")+old["buildUTC"].as<String>()+"|"+profile+"|"+unitSerial()+"|"+q["sessionId"].as<String>()+"|"+q["profileRevision"].as<String>()+"|"+q["profileMask"].as<String>()+"|RECIPE:"+q["recipe"]["revision"].as<String>()+":"+q["recipe"]["crc"].as<String>();
 auto field=[&](String v){canonical+="|"+String(v.length())+":"+v;};for(JsonObjectConst t:q["tests"].as<JsonArrayConst>()){field(t["name"].as<String>());field(t["status"].as<String>());field(t["detail"].as<String>());}field(q["overall"].as<String>());
 char crc[9];snprintf(crc,sizeof(crc),"%08lX",static_cast<unsigned long>(qcCrc(canonical.c_str())));String code=String("QC1-")+profile+"-"+unitSerial()+"-"+q["overall"].as<String>()+"-"+q["sessionId"].as<String>()+"-"+crc;
 if(code!=q["qcCode"].as<String>()){why="Legacy QC code CRC mismatch; original evidence must remain unedited";return false;}
 if(!savedSession.isEmpty()){
  bool retry=saved["sessionId"]==q["sessionId"]&&saved["operator"]==q["operator"]&&saved["evidence"]["originQCCode"]==code&&rtcClockRevision()==0&&!saved["rtcEvidence"]["tickPassed"].as<bool>()&&!saved["rtcEvidence"]["tickFailed"].as<bool>()&&saved["rtcEvidence"]["external"].size()==0;
  auto tests=saved["tests"].as<JsonArrayConst>();auto incoming=q["tests"].as<JsonArrayConst>();retry=retry&&tests.size()==incoming.size();
  for(size_t i=0;retry&&i<tests.size();++i)if(tests[i]["name"]!="RTC")retry=tests[i]["name"]==incoming[i]["name"]&&tests[i]["status"]==incoming[i]["status"]&&tests[i]["detail"]==incoming[i]["detail"];
  if(!retry){why="Existing checkpoint contains different/new evidence; legacy replacement blocked";return false;}
 }
 JsonDocument current;qcProfilesState(current.to<JsonObject>());qcRecipesState(current.as<JsonObject>());bool profileOk=false,recipeOk=false;
 for(JsonObject p:current["profiles"].as<JsonArray>())if(p["id"]==profile&&p["revision"]==q["profileRevision"])profileOk=true;
 for(JsonObject p:current["recipes"].as<JsonArray>())if(p["id"]==profile&&p["revision"]==q["recipe"]["revision"]&&p["crc"]==q["recipe"]["crc"])recipeOk=true;
 JsonDocument cal;calibrationState(cal.to<JsonObject>());
 if(!profileOk||!recipeOk){why="Legacy profile/preset revision differs";return false;}
 // The original API serialized doubles to decimal JSON. Never compare the parsed rounded value
 // bit-for-bit with the full-precision NVS coefficient; captured calibration CRC IDs below stay exact.
 if(cal["active"].as<bool>()!=old["hardware"]["calibration"]["active"].as<bool>()||fabs(cal["gain"].as<double>()-old["hardware"]["calibration"]["gain"].as<double>())>1e-9||fabs(cal["offset"].as<double>()-old["hardware"]["calibration"]["offset"].as<double>())>1e-8){why="Legacy calibration coefficients differ beyond JSON rounding";return false;}
 for(JsonObjectConst p:old["hardware"]["currentQc"]["points"].as<JsonArrayConst>())if(p["captured"]==true&&p["calibrationId"].as<uint32_t>()!=currentCalibrationId()){why="Legacy analog calibration identity differs";return false;}
 JsonDocument next;next["schema"]="QC-PROGRESS-1";next["serial"]=unitSerial();next["buildIdentity"]=FW_BUILD_ID;next["calibrationId"]=currentCalibrationId();next["profile"]=profile;next["profileRevision"]=q["profileRevision"];next["profileMask"]=q["profileMask"];next["recipeIdentity"]=q["recipe"]["revision"].as<String>()+":"+q["recipe"]["crc"].as<String>();next["sessionId"]=q["sessionId"];next["operator"]=q["operator"];
 auto tests=next["tests"].to<JsonArray>();for(JsonObjectConst t:q["tests"].as<JsonArrayConst>()){auto p=tests.add<JsonObject>();for(const char *k:{"name","status","detail"})p[k].set(t[k]);}
 next["outputs"].set(q["outputs"]);auto cycles=next["cycles"].to<JsonArray>();for(JsonObjectConst p:q["inputs"].as<JsonArrayConst>())cycles.add(p["cycles"]);
 auto evidence=next["evidence"].to<JsonObject>();evidence["analog"].set(old["hardware"]["currentQc"]);for(const char *k:{"modbus","network","serial","radio"})evidence[k].set(q[k]);evidence["originFirmware"]="1.9.1";evidence["originBuildUTC"]=old["buildUTC"];evidence["originQCCode"]=code;evidence["originOperatorProvided"]=!q["operator"].as<String>().isEmpty();
 if(!qcValidateProgress(next.as<JsonObjectConst>(),why))return false;
 current.clear();cal.clear();original.clear(); // Input and validation trees are no longer needed; keep ESP32 heap for atomic readback.
 if(!qcSaveBlob("qc-progress","session-v1",next,why))return false;saved.set(next);savedSession=next["sessionId"].as<String>();return qcResumeProgress(why);
}
