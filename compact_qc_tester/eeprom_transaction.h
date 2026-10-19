#pragma once
#include <cstddef>
#include <cstdint>
namespace QcEeprom {
struct Result { bool matched, restored; };
// Restoration is attempted even after a partial challenge write or failed read.
template<std::size_t N, class Write, class Read>
constexpr Result challenge(const uint8_t *before,const uint8_t *pattern,Write write,Read read) {
  uint8_t check[N]={};
  bool matched=write(pattern)&&read(check);
  for(std::size_t i=0;i<N;++i)matched=matched&&check[i]==pattern[i];
  bool restored=write(before)&&read(check);
  for(std::size_t i=0;i<N;++i)restored=restored&&check[i]==before[i];
  return {matched,restored};
}
}
