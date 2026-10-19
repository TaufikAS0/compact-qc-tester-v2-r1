#include "eeprom_transaction.h"
namespace {
constexpr bool test(int fault) {
  uint8_t before[4]={1,2,3,4},pattern[4]={9,8,7,6},memory[4]={1,2,3,4};
  int writes=0,reads=0;
  auto write=[&](const uint8_t *p) {
    ++writes;
    for(int i=0;i<4;++i)memory[i]=p[i];
    if((fault==1&&writes==1)||(fault==3&&writes==2)){memory[3]=0;return false;}
    return true;
  };
  auto read=[&](uint8_t *p) {
    ++reads;for(int i=0;i<4;++i)p[i]=memory[i];
    if(fault==2&&reads==1)p[0]^=1;
    if(fault==4&&reads==2)p[0]^=1;
    return true;
  };
  auto r=QcEeprom::challenge<4>(before,pattern,write,read);
  if(writes!=2)return false;
  if(fault==0)return r.matched&&r.restored;
  if(fault==1||fault==2)return !r.matched&&r.restored&&memory[3]==before[3];
  return r.matched&&!r.restored;
}
static_assert(test(0),"EEPROM exact write/read/restore must pass");
static_assert(test(1),"Partial challenge write still restores original bytes");
static_assert(test(2),"Corrupt challenge read cannot pass; restoration still required");
static_assert(test(3),"Failed restore cannot pass");
static_assert(test(4),"Restore readback mismatch cannot pass");
}
