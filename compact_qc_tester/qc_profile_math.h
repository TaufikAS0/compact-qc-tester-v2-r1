#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string_view>
namespace QcProfiles {
struct Test { const char *id; const char *name; };
constexpr Test catalog[] = {
  {"oled","OLED"},{"i2c","I2C scan"},{"rtc","RTC"},{"eeprom","EEPROM"},
  {"aht","AHT10"},{"analog","ADS / 4-20mA"},{"pd","STUSB4500"},{"eth_spi","Ethernet"},{"sd","SD"},
  {"di1","Input:DI1"},{"di2","Input:DI2"},{"di3","Input:DI3"},{"di4","Input:DI4"},
  {"dmm","Input:DMM mode"},{"normal","Input:Normal mode"},{"webserver","Input:Webserver mode"},
  {"led0","LED:0"},{"led1","LED:1"},{"led2","LED:2"},{"led3","LED:3"},{"led4","LED:4"},
  {"led5","LED:5"},{"led6","LED:6"},{"led7","LED:7"},{"led8","LED:8"},{"led9","LED:9"},{"led10","LED:10"},
  {"do1","Relay:0"},{"do2","Relay:1"},{"rs485","RS485"},{"wifi","WiFi network"},
  {"eth_net","Ethernet network"},{"mqtt","MQTT"},{"rs232","RS232"},
  {"radio_comm","Radio communication"},{"radio_config","Radio configuration"}
};
constexpr size_t TESTS = sizeof(catalog)/sizeof(catalog[0]);
constexpr uint64_t ALL = (uint64_t(1)<<TESTS)-1;
constexpr int index(std::string_view value, bool byId=false) {
  if (!byId && (value=="Modbus tag:1" || value=="Modbus tag:2")) value="RS485";
  for (size_t i=0;i<TESTS;++i) if (value==(byId?catalog[i].id:catalog[i].name)) return i;
  return -1;
}
constexpr bool enabled(uint64_t mask, std::string_view name) {
  int i=index(name); return i>=0 && (mask & (uint64_t(1)<<i));
}
constexpr uint64_t defaultMask(unsigned id) {
  return id==1 ? ALL & ~(uint64_t(1)<<index("STUSB4500")) & ~(uint64_t(1)<<index("EEPROM")) &
    ~(uint64_t(1)<<index("RS232")) & ~(uint64_t(1)<<index("Radio configuration")) : ALL;
}
constexpr int parseId(std::string_view s) {
  if(s.size()<2 || s.size()>4 || s[0]!='M' || (s.size()>2 && s[1]=='0')) return -1;
  unsigned id=0;
  for(size_t i=1;i<s.size();++i) { if(s[i]<'0'||s[i]>'9') return -1; id=id*10+s[i]-'0'; }
  return id<=999 ? int(id) : -1;
}
static_assert(TESTS<64 && !enabled(defaultMask(1),"RS232") && !enabled(defaultMask(1),"EEPROM"));
static_assert(enabled(defaultMask(1),"Radio communication") && enabled(defaultMask(1),"Modbus tag:1"));
static_assert(parseId("M2")==2 && parseId("M999")==999 && parseId("M01")==-1);
}
