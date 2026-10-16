#pragma once
#include <WebServer.h>
void beginGuardedOta(WebServer &web, bool (*ready)(String &));
bool guardedOtaBusy();
void updateGuardedOta();
