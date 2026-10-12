#pragma once
#include <Arduino.h>

void beginTplWatchdog();
void updateTplWatchdog();
bool tplWatchdogReady();
const char *tplWatchdogError();
uint32_t tplWatchdogPulses();
