#include "qc_reset_scope.h"
static_assert(qcResetAffects("Modbus tag:1","RS485"));
static_assert(qcResetAffects("RS485","Modbus tag:2"));
static_assert(!qcResetAffects("RS485","RS232"));
static_assert(qcResetAffects("Radio communication","Radio configuration"));
static_assert(!qcResetAffects("WiFi network","Ethernet network"));
static_assert(!qcResetAffects("Relay:0","Relay:1"));
static_assert(!qcResetAffects("Input:DI1","Input:DI2"));
