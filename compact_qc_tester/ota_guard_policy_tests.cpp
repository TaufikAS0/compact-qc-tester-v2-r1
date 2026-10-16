#include "ota_guard_policy.h"
using namespace ota_guard;
static_assert(targetMatches("TMC","TMC123","TMC123","TMC_T_V2_R1+TMC_B_V3_R0","tmc-legacy-16mb","1"));
static_assert(!targetMatches("TTB","TMC123","TMC123","TMC_T_V2_R1+TMC_B_V3_R0","tmc-legacy-16mb","1"));
static_assert(!targetMatches("TMC","TMC999","TMC123","TMC_T_V2_R1+TMC_B_V3_R0","tmc-legacy-16mb","1"));
static_assert(!targetMatches("TMC","TMC123","TMC123","UNKNOWN","tmc-legacy-16mb","1"));
static_assert(!targetMatches("TMC","TMC123","TMC123","TMC_T_V2_R1+TMC_B_V3_R0","default","1"));
static_assert(!targetMatches("TMC","TMC123","TMC123","TMC_T_V2_R1+TMC_B_V3_R0","tmc-legacy-16mb","0"));
static_assert(sha256Valid("0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"));
static_assert(!sha256Valid("0123"));
static_assert(!sha256Valid("x123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"));
static_assert(capacityFits(0x200000,0x200000)&&!capacityFits(0x200001,0x200000)&&!capacityFits(255,0x200000));
static_assert(activateAllowed(true,1000,1000,true,true,true));
static_assert(!activateAllowed(false,1000,1000,true,true,true)&&!activateAllowed(true,999,1000,true,true,true));
static_assert(!activateAllowed(true,1000,1000,false,true,true)&&!activateAllowed(true,1000,1000,true,false,true)&&!activateAllowed(true,1000,1000,true,true,false));
constexpr uint8_t classic[24]={0xe9,0,0,0x40};
constexpr uint8_t wrongChip[24]={0xe9,0,0,0x40,0,0,0,0,0,0,0,0,9};
constexpr uint8_t merged[24]={0xff};
static_assert(appHeaderValid(classic,24)&&!appHeaderValid(classic,23)&&!appHeaderValid(wrongChip,24)&&!appHeaderValid(merged,24));
