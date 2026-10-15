#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
void beginQcRecipes();
bool qcRecipesReady();
void qcRecipesState(JsonObject out);
String activeQcRecipeIdentity();
bool saveQcRecipe(const String &id, uint32_t expected, const String &json, String &detail);
