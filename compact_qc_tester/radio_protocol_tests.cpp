#include "radio_protocol.h"
static_assert(RadioProtocol::parse("WMCFG:4",1)==4);
static_assert(RadioProtocol::parse(" +WMCFG = 4 \r",1)==4);
static_assert(RadioProtocol::parse("WMCFG\xef\xbc\x9a 4",1)==4);
static_assert(RadioProtocol::parse("Channel:10",0)==10);
static_assert(RadioProtocol::parse("AT+WMCFG=?",1)==-1);
static_assert(RadioProtocol::parse("+OK",1)==-1);
static_assert(RadioProtocol::parse("WMCFG:5",1)==-1);
static_assert(RadioProtocol::parse("WMCFG:4junk",1)==-1);
static_assert(RadioProtocol::parse("TMCFG:4",1)==-1);
static_assert(RadioProtocol::writeAck("+OK\r")&&!RadioProtocol::writeAck("NOT+OK")&&!RadioProtocol::writeAck("AT+CNCFG=+OK"));
