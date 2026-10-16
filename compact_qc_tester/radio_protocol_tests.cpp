#include "radio_protocol.h"
static_assert(RadioProtocol::parse("WMCFG:4",1)==4);
static_assert(RadioProtocol::parse(" +WMCFG = 4 \r",1)==4);
static_assert(RadioProtocol::parse("WMCFG\xef\xbc\x9a 4",1)==4);
static_assert(RadioProtocol::parse("Channel:10",0)==10);
// Actual E70(915NW14S)_V1.0 UART response captured on this unit.
static_assert(RadioProtocol::parse("Mode:4",1)==4);
static_assert(RadioProtocol::parse("Mode:4",0)==-1);
static_assert(RadioProtocol::parse("Mode:5",1)==-1);
static_assert(RadioProtocol::parse("AT+WMCFG=?",1)==-1);
static_assert(RadioProtocol::parse("+OK",1)==-1);
static_assert(RadioProtocol::parse("WMCFG:5",1)==-1);
static_assert(RadioProtocol::parse("WMCFG:4junk",1)==-1);
static_assert(RadioProtocol::parse("TMCFG:4",1)==-1);
static_assert(RadioProtocol::writeAck("+OK\r")&&!RadioProtocol::writeAck("NOT+OK")&&!RadioProtocol::writeAck("AT+CNCFG=+OK"));
