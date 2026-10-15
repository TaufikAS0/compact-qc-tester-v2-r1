#include "rtc_math.h"
constexpr bool roundtrip(int64_t e){uint8_t b[7]={};int64_t out=0;return RtcMath::encode(e,b)&&RtcMath::decode(b,out)&&out==e;}
static_assert(roundtrip(946684800LL)&&roundtrip(4102444799LL));
static_assert(roundtrip(RtcMath::epoch(2024,2,29,23,59,59)));
static_assert(roundtrip(RtcMath::epoch(2026,10,15,8,0,0)));
static_assert(!RtcMath::date(2025,4,31,0,0,0)&&!RtcMath::date(2024,1,0,0,0,0));
static_assert(!RtcMath::date(2100,1,1,0,0,0)&&!RtcMath::date(2025,1,1,24,0,0));
constexpr bool badBcd(){uint8_t b[]={0x6a,0,0,1,1,1,0x26};int64_t out=0;return !RtcMath::decode(b,out);}
static_assert(badBcd());
constexpr bool twelve(){uint8_t b[]={0,0,0x72,1,1,1,0x26};int64_t out=0;return RtcMath::decode(b,out)&&out==RtcMath::epoch(2026,1,1,12,0,0);}
static_assert(twelve());
static_assert(RtcMath::readback(1792047120,1792047120)&&RtcMath::readback(1792047120,1792047121));
static_assert(!RtcMath::readback(1792047120,1792047119)&&!RtcMath::readback(1792047120,1792047122));
static_assert(!RtcMath::readback(0,0)&&!RtcMath::readback(4102444799LL,4102444800LL));
static_assert(RtcMath::writeChallenge(946684800LL,946684800LL)==946684920LL);
static_assert(RtcMath::writeChallenge(4102444799LL,4102444799LL)==4102444679LL);
static_assert(RtcMath::writeChallenge(1792047120,1792047240)==1792047000);
static_assert(RtcMath::evidence(86400,.2,1.2,3,true,false,false)==1);
static_assert(RtcMath::evidence(86399,0,1,3,true,false,false)==0);
static_assert(RtcMath::evidence(86400,0,1,.1728,true,false,false)==0);
static_assert(RtcMath::evidence(86400,2.5,1,3,true,false,false)==0);
static_assert(RtcMath::evidence(86400,-5,1,3,true,false,false)==-1);
static_assert(RtcMath::evidence(300,0,1,3,true,true,false)==0);
static_assert(RtcMath::evidence(300,0,1,3,true,true,true)==1);
static_assert(RtcMath::evidence(86400,0,1,3,false,false,false)==0);
