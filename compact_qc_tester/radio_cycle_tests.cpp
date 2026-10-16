#include "radio_cycle.h"
using namespace RadioCycle;
static_assert(std::string_view(verdict(true,false,false,true))=="MENUNGGU SEND");
static_assert(std::string_view(verdict(true,true,true,true))=="PASS");
static_assert(std::string_view(verdict(false,true,true,true))=="FAIL");
static_assert(std::string_view(verdict(true,true,false,true))=="FAIL");
static_assert(std::string_view(verdict(true,true,true,false))=="FAIL");
static_assert(!resumable("",true,true,1,1));
static_assert(!resumable(CRITERION,true,false,1,1));
static_assert(!resumable(CRITERION,true,true,3,1));
static_assert(resumable(CRITERION,true,true,1,1));
