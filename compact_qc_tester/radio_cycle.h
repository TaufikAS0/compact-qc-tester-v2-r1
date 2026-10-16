#pragma once
#include <string_view>
namespace RadioCycle {
constexpr const char *CRITERION="GET_SEND_GET_V1";
constexpr const char *verdict(bool getOk,bool sent,bool acknowledged,bool matches){
 return !getOk?"FAIL":!sent?"MENUNGGU SEND":acknowledged&&matches?"PASS":"FAIL";
}
constexpr bool resumable(std::string_view criterion,bool verified,bool attempted,unsigned mask,unsigned acked){
 return criterion==CRITERION&&verified&&attempted&&mask&&(acked&mask)==mask;
}
}
