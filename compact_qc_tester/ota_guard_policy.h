#pragma once
#include <string_view>
#include <stdint.h>
namespace ota_guard {
constexpr bool targetMatches(std::string_view product,std::string_view serial,std::string_view expectedSerial,std::string_view hardware,std::string_view partition,std::string_view archive){
 return product=="TMC"&&!expectedSerial.empty()&&serial==expectedSerial&&hardware=="TMC_T_V2_R1+TMC_B_V3_R0"&&partition=="tmc-legacy-16mb"&&archive=="1";
}
constexpr bool sha256Valid(std::string_view text){if(text.size()!=64)return false;for(char c:text)if(!((c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F')))return false;return true;}
constexpr bool capacityFits(size_t size,size_t slot){return size>=256&&slot>0&&size<=slot;}
constexpr bool appHeaderValid(const uint8_t *p,size_t n){return n>=24&&p[0]==0xe9&&p[12]==0&&p[13]==0&&(p[3]>>4)==4;}
constexpr bool activateAllowed(bool started,size_t received,size_t expected,bool hashMatches,bool targetFound,bool buildFound){return started&&received==expected&&hashMatches&&targetFound&&buildFound;}
}
