#pragma once
#include <WebServer.h>

void beginWebOta(WebServer &server, void (*stopOutputs)());
void updateWebOta();
bool webOtaBusy();
