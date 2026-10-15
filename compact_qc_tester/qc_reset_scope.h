#pragma once
#include <string_view>
// Shared evidence: an individual Modbus tag cannot leave RS485 PASS behind.
constexpr bool qcResetAffects(std::string_view target,std::string_view item){
  const bool modbus=target=="RS485"||target=="Modbus tag:1"||target=="Modbus tag:2";
  const bool radio=target=="Radio communication"||target=="Radio configuration";
  return modbus?(item=="RS485"||item=="Modbus tag:1"||item=="Modbus tag:2"):
    radio?(item=="Radio communication"||item=="Radio configuration"):target==item;
}
