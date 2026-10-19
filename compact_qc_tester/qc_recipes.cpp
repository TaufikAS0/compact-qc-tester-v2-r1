#include "qc_recipes.h"
#include "qc_profiles.h"
#include "qc_profile_math.h"
#include "qc_report_math.h"
#include <Preferences.h>
#include <math.h>
namespace {
JsonDocument recipes;
bool ready=false;
String recipeDetail;
String crc(const String &s){char b[9];snprintf(b,sizeof(b),"%08lX",static_cast<unsigned long>(qcCrc(s.c_str())));return b;}
bool integer(JsonVariantConst v,long low,long high,long &out){if(!v.is<long>()&&!v.is<unsigned long>())return false;out=v.as<long>();return out>=low&&out<=high;}
bool normalized(JsonObjectConst in,JsonObject out){
  long n;
  if(!in["toleranceMA"].is<float>()&&!in["toleranceMA"].is<int>())return false;
  double tolerance=in["toleranceMA"].as<double>();if(!isfinite(tolerance)||tolerance<.01||tolerance>1)return false;out["toleranceMA"]=tolerance;
  const char* fields[]={"serialBaud","serialPackets","serialTimeoutMs","stepSeconds"};
  const long lows[]={1200,10,500,1},highs[]={115200,1000,10000,10};
  for(unsigned i=0;i<4;++i){if(!integer(in[fields[i]],lows[i],highs[i],n))return false;out[fields[i]]=n;}
  const long bauds[]={1200,2400,4800,9600,19200,38400,57600,115200};
  auto baudOk=[&](long b){for(long v:bauds)if(v==b)return true;return false;};
  if(!baudOk(out["serialBaud"].as<long>())||out["serialTimeoutMs"].as<long>() < 3120000L/out["serialBaud"].as<long>()+300)return false;
  auto refs=in["referencesMA"].as<JsonArrayConst>();if(refs.size()!=5)return false;auto a=out["referencesMA"].to<JsonArray>();
  double last=-1;for(JsonVariantConst v:refs){if(!v.is<float>()&&!v.is<int>())return false;double x=v.as<double>();if(!isfinite(x)||x<4||x>20||x<=last)return false;a.add(x);last=x;}
  auto tags=in["tags"].as<JsonArrayConst>();if(tags.size()!=2)return false;auto dst=out["tags"].to<JsonArray>();
  for(JsonObjectConst t:tags){auto p=dst.add<JsonObject>();String name=t["name"].as<String>();if(!name.length()||name.length()>10)return false;p["name"]=name;
    const char* keys[]={"baud","slave","function","address","count","timeout"};const long lo[]={1200,1,1,0,1,100},hi[]={115200,247,4,65535,2,10000};
    for(unsigned i=0;i<6;++i){if(!integer(t[keys[i]],lo[i],hi[i],n))return false;p[keys[i]]=n;}
    if(!baudOk(p["baud"].as<long>()))return false;
    String parity=t["parity"].as<String>(),type=t["type"].as<String>(),order=t["order"].as<String>();
    if(parity!="N"&&parity!="E"&&parity!="O")return false;if(type!="Unsigned"&&type!="Signed"&&type!="Float")return false;if(order!="ABCD"&&order!="CDAB")return false;
    if(type=="Float"&&(p["count"].as<int>()!=2||p["function"].as<int>()<3))return false;
    if(p["address"].as<long>()+p["count"].as<long>()>65536)return false;
    p["parity"]=parity;p["type"]=type;p["order"]=order;
  }
  return !out.isNull();
}
void defaults(JsonObject p){p["toleranceMA"]=.1;p["serialBaud"]=9600;p["serialPackets"]=100;p["serialTimeoutMs"]=2000;p["stepSeconds"]=5;
 auto a=p["referencesMA"].to<JsonArray>();for(int v:{4,8,12,16,20})a.add(v);
 auto tags=p["tags"].to<JsonArray>();for(int i=0;i<2;++i){auto t=tags.add<JsonObject>();t["name"]=String("Tag")+String(i+1);t["baud"]=9600;t["parity"]="N";t["slave"]=1;t["function"]=3;t["address"]=i;t["count"]=1;t["type"]=i==1?"Signed":"Unsigned";t["order"]="ABCD";t["timeout"]=2000;}}
void entry(const String &id,JsonObject out){out["id"]=id;out["revision"]=0;defaults(out["settings"].to<JsonObject>());for(JsonObject p:recipes["entries"].as<JsonArray>())if(p["id"].as<String>()==id){out["revision"]=p["revision"];out["settings"].set(p["settings"]);break;}String s;serializeJson(out["settings"],s);out["crc"]=crc(s);}
}
void beginQcRecipes(){recipes.clear();recipes["entries"].to<JsonArray>();Preferences p;ready=p.begin("qc-recipes",false);recipeDetail=ready?"Preset default; simpan per profil bila diperlukan":"Preset NVS tidak tersedia";if(!ready)return;String raw=p.getString("recipes-v1",""),sum=p.getString("crc-v1","");p.end();if(raw.isEmpty())return;
 JsonDocument loaded;bool ok=crc(raw)==sum&&!deserializeJson(loaded,raw)&&loaded["schema"].as<int>()==1&&loaded["entries"].is<JsonArray>()&&loaded["entries"].size()<=16;
 if(ok){String ids=",";for(JsonObject v:loaded["entries"].as<JsonArray>()){String id=v["id"].as<String>();JsonDocument n;long rev;ok=QcProfiles::parseId(id.c_str())>=0&&ids.indexOf(","+id+",")<0&&integer(v["revision"],1,2147483647,rev)&&normalized(v["settings"],n.to<JsonObject>());if(!ok)break;ids+=id+",";}}
 if(ok){recipes.set(loaded);recipeDetail="Preset dimuat dari NVS";}else{ready=false;recipeDetail="Preset storage/CRC tidak valid; selection/save diblokir, jangan erase kalibrasi";}}
bool qcRecipesReady(){return ready;}
void qcRecipesState(JsonObject out){out["recipeStorageReady"]=ready;out["recipeStorageDetail"]=recipeDetail;JsonDocument profiles;qcProfilesState(profiles.to<JsonObject>());auto list=out["recipes"].to<JsonArray>();for(JsonObject p:profiles["profiles"].as<JsonArray>())entry(p["id"].as<String>(),list.add<JsonObject>());if(qcProfileIsSelected())entry(activeQcProfileName(),out["recipe"].to<JsonObject>());}
String activeQcRecipeIdentity(){if(!qcProfileIsSelected())return "";JsonDocument p;entry(activeQcProfileName(),p.to<JsonObject>());return String(p["revision"].as<unsigned long>())+":"+p["crc"].as<String>();}
bool saveQcRecipe(const String &id,uint32_t expected,const String &json,String &why){if(!ready){why=recipeDetail;return false;}JsonDocument profiles;qcProfilesState(profiles.to<JsonObject>());bool exists=false;for(JsonObject p:profiles["profiles"].as<JsonArray>())if(p["id"].as<String>()==id)exists=true;if(!exists||json.length()>2048){why="Profil/payload preset tidak valid";return false;}
 JsonDocument old;entry(id,old.to<JsonObject>());if(old["revision"].as<uint32_t>()!=expected||expected>=2147483647){why="Revisi preset berubah; muat ulang";return false;}
 JsonDocument input,settings;if(deserializeJson(input,json)||!normalized(input.as<JsonObjectConst>(),settings.to<JsonObject>())){why="Parameter preset di luar batas/format";return false;}
 JsonDocument next;next.set(recipes);next["schema"]=1;auto list=next["entries"].as<JsonArray>();JsonObject target;for(JsonObject p:list)if(p["id"].as<String>()==id)target=p;if(target.isNull())target=list.add<JsonObject>();target["id"]=id;target["revision"]=expected+1;target["settings"].set(settings.as<JsonObject>());
 String raw;serializeJson(next,raw);Preferences p;if(!p.begin("qc-recipes",false)){why="Tidak dapat membuka preset NVS";return false;}String sum=crc(raw);bool ok=p.putString("recipes-v1",raw)==raw.length()&&p.putString("crc-v1",sum)==sum.length()&&p.getString("recipes-v1","")==raw&&p.getString("crc-v1","")==sum;p.end();if(!ok){ready=false;why=recipeDetail="Preset write/readback gagal; muat ulang/periksa NVS";return false;}recipes.set(next);why=recipeDetail="Preset tersimpan/readback; perubahan profil aktif memulai sesi baru";return true;}
