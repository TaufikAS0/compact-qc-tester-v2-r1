#include "qc_profile_math.h"
#include "aht10_math.h"
namespace {
using namespace QcProfiles;
constexpr bool catalogUnique() {
  for(size_t i=0;i<TESTS;++i) for(size_t j=0;j<i;++j)
    if(std::string_view(catalog[i].id)==catalog[j].id || std::string_view(catalog[i].name)==catalog[j].name) return false;
  return true;
}
static_assert(catalogUnique());
static_assert(defaultMask(0)==ALL && !enabled(defaultMask(1),"Radio configuration"));
static_assert(!enabled(defaultMask(1),"STUSB4500") && enabled(defaultMask(1),"RS485"));
static_assert(!enabled(0,"OLED") && !enabled(ALL,"Unknown test"));
constexpr uint64_t withoutDo=ALL&~(uint64_t(1)<<index("Relay:0"))&~(uint64_t(1)<<index("Relay:1"));
static_assert(!enabled(withoutDo,"Relay:0") && !enabled(withoutDo,"Relay:1") && enabled(withoutDo,"LED:0"));
static_assert(enabled(withoutDo|(uint64_t(1)<<index("Relay:0")),"Relay:0") && !enabled(withoutDo,"Relay:1"));
static_assert(parseId("M0")==0 && parseId("M3")==3 && parseId("M1000")==-1 && parseId("M-1")==-1);
static_assert(parseId("M01")==-1 && parseId("M")==-1 && parseId("<M2>")==-1);
constexpr uint8_t hot[]={0x08,0xff,0xff,0xff,0xff,0xff};
static_assert(Aht10::humidity(hot)>99 && Aht10::temperature(hot)>149);
static_assert(!Aht10::calibrated(0x80) && Aht10::COMMAND_SETTLE_MS>20);
}
