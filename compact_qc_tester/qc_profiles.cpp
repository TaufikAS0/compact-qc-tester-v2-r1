#include "qc_profiles.h"
#include "qc_profile_math.h"
#include "qc_report_math.h"
#include <Preferences.h>
namespace {
constexpr unsigned LIMIT=16;
struct Entry { uint16_t id, reserved; uint32_t revision; uint64_t mask; };
struct Store { uint32_t magic, schema, count; Entry entries[LIMIT]; uint32_t checksum; };
Store store={};
Entry active={};
bool selected=false, writable=false;
char activeName[5]="";
String storeDetail;
uint32_t checksum(const Store &s) {
  uint32_t crc=0xffffffff;
  auto bytes=reinterpret_cast<const uint8_t*>(&s);
  for(size_t i=0;i<offsetof(Store,checksum);++i) crc=qcCrcByte(crc,bytes[i]);
  return crc^0xffffffff;
}
bool valid(const Store &s) {
  if(s.magic!=0x51504631 || s.schema!=1 || s.count<2 || s.count>LIMIT || s.checksum!=checksum(s)) return false;
  bool m0=false,m1=false;
  for(unsigned i=0;i<s.count;++i) {
    const auto &p=s.entries[i];
    if(p.id>999 || !p.revision || (p.mask & ~QcProfiles::ALL) || p.reserved) return false;
    for(unsigned j=0;j<i;++j) if(s.entries[j].id==p.id) return false;
    m0|=p.id==0; m1|=p.id==1;
  }
  return m0&&m1;
}
int find(unsigned id) {
  for(unsigned i=0;i<store.count;++i) if(store.entries[i].id==id) return i;
  return -1;
}
String hexMask(uint64_t mask) { char b[17]; snprintf(b,sizeof(b),"%016llX",(unsigned long long)mask); return b; }
}
void beginQcProfiles() {
  store={}; store.magic=0x51504631; store.schema=1; store.count=2;
  store.entries[0]={0,0,1,QcProfiles::defaultMask(0)};
  store.entries[1]={1,0,1,QcProfiles::defaultMask(1)};
  Preferences prefs;
  writable=prefs.begin("qc-profiles",false);
  storeDetail="Default M0/M1 loaded; select profile before QC";
  if(writable) {
    size_t n=prefs.getBytesLength("profiles-v1");
    if(n) {
      Store loaded={};
      if(n==sizeof(Store) && prefs.getBytes("profiles-v1",&loaded,sizeof(loaded))==sizeof(loaded) && valid(loaded)) {
        store=loaded; storeDetail="QC profiles loaded from NVS";
      } else { writable=false; storeDetail="Invalid profile storage: explicit maintenance required; defaults shown, QC selection blocked"; }
    }
    prefs.end();
  } else storeDetail="Profile NVS unavailable: selection/save blocked";
}
bool selectQcProfile(const String &id) {
  int n=QcProfiles::parseId(id.c_str()); int i=n<0?-1:find(n);
  if(i<0 || !writable) return false;
  active=store.entries[i]; selected=true; snprintf(activeName,sizeof(activeName),"M%u",active.id); return true;
}
bool qcProfileIsSelected() { return selected; }
const char *activeQcProfileName() { return selected?activeName:"UNSELECTED"; }
uint32_t activeQcProfileRevision() { return selected?active.revision:0; }
String activeQcProfileMask() { return hexMask(selected?active.mask:0); }
bool qcTestEnabled(const String &name) {
  // Unselected boot can show sensor diagnostics, but API/session never accepts QC without selection.
  return !selected || QcProfiles::enabled(active.mask,name.c_str());
}
void qcProfilesState(JsonObject out) {
  out["profileRevision"]=activeQcProfileRevision(); out["profileMask"]=activeQcProfileMask();
  out["profileStorageDetail"]=storeDetail; out["profileStorageReady"]=writable;
  out["profileLimit"]=LIMIT;
  auto list=out["profiles"].to<JsonArray>();
  for(unsigned i=0;i<store.count;++i) {
    const auto &p=store.entries[i]; auto e=list.add<JsonObject>();
    e["id"]=String("M")+p.id; e["revision"]=p.revision;
    auto ids=e["enabled"].to<JsonArray>();
    for(size_t j=0;j<QcProfiles::TESTS;++j) if(p.mask&(uint64_t(1)<<j)) ids.add(QcProfiles::catalog[j].id);
  }
  auto catalog=out["testCatalog"].to<JsonArray>();
  for(auto &test:QcProfiles::catalog) { auto t=catalog.add<JsonObject>(); t["id"]=test.id; t["name"]=test.name; }
}
bool saveQcProfile(const String &id,const String &ids,uint32_t expected,String &detail) {
  int n=QcProfiles::parseId(id.c_str()); int i=n<0?-1:find(n);
  if(n<0 || ids.length()>512) { detail="Profile M0..M999 / test list invalid"; return false; }
  if(!writable) { detail=storeDetail; return false; }
  if((i>=0 && store.entries[i].revision!=expected) || (i<0 && expected!=0)) {
    detail="Profile revision changed; reload configuration"; return false;
  }
  if(i<0 && store.count>=LIMIT) { detail="Maximum16 profiles reached"; return false; }
  uint64_t mask=0;
  if(ids.length()) {
    int start=0;
    while(start<int(ids.length())) {
      int end=ids.indexOf(',',start); if(end<0) end=ids.length();
      String key=ids.substring(start,end); int bit=QcProfiles::index(key.c_str(),true);
      if(bit<0 || mask&(uint64_t(1)<<bit)) { detail="Unknown/duplicate test ID"; return false; }
      mask|=uint64_t(1)<<bit; start=end+1;
      if(end==int(ids.length())-1) { detail="Trailing empty test ID"; return false; }
    }
  }
  Store next=store;
  if(i<0) i=next.count++;
  uint32_t revision=expected+1;
  if(!revision) { detail="Revision limit reached"; return false; }
  next.entries[i]={uint16_t(n),0,revision,mask}; next.checksum=checksum(next);
  Preferences prefs;
  if(!prefs.begin("qc-profiles",false)) { detail="Cannot open profile NVS"; return false; }
  bool ok=prefs.putBytes("profiles-v1",&next,sizeof(next))==sizeof(next);
  Store readback={};
  ok=ok && prefs.getBytes("profiles-v1",&readback,sizeof(readback))==sizeof(readback) && valid(readback) &&
    memcmp(&next,&readback,sizeof(next))==0;
  prefs.end();
  if(!ok) { writable=false; detail=storeDetail="Profile save/readback failed; selection/save blocked until reboot/maintenance"; return false; }
  store=next; storeDetail="Profiles saved and read back; active profile changes start a new session";
  detail=storeDetail; return true;
}
