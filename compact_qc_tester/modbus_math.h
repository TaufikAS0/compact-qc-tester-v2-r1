#pragma once
#include <stdint.h>
#include <stddef.h>
namespace QcModbus {
constexpr uint16_t crc(const uint8_t *p, size_t n) {
  uint16_t c = 0xffff;
  for (size_t i=0;i<n;++i) { c ^= p[i]; for(int b=0;b<8;++b) c=(c>>1)^((c&1)?0xa001:0); }
  return c;
}
constexpr bool valid(const uint8_t *p, size_t n) {
  return n>=5 && crc(p,n-2)==uint16_t(p[n-2]|uint16_t(p[n-1])<<8);
}
constexpr int bytes(uint8_t fn, uint16_t count) { return fn<=2 ? (count+7)/8 : count*2; }
enum class Response { Valid, Short, Crc, Slave, Exception, Function, Length };
constexpr Response validate(const uint8_t *p,size_t n,uint8_t slave,uint8_t function,uint16_t count) {
  if(n<5)return Response::Short;
  if(!valid(p,n))return Response::Crc;
  if(p[0]!=slave)return Response::Slave;
  if(p[1]==(function|0x80))return n==5?Response::Exception:Response::Length;
  if(p[1]!=function)return Response::Function;
  if(p[2]!=bytes(function,count)||n!=size_t(5+p[2]))return Response::Length;
  return Response::Valid;
}
}
