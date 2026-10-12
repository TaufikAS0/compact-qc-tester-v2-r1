#pragma once
#include <ArduinoJson.h>
#include <Arduino.h>
void beginCurrentCalibration();
void updateCurrentCalibration();
bool captureCurrentPoint(int index, float reference, String &detail);
bool saveCurrentCalibration(float maxResidual, String &detail);
void resetCurrentPoints();
bool calibratedCurrent(float raw, float &ma);
void calibrationState(JsonObject out);
