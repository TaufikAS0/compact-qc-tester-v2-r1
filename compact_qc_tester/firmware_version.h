#pragma once
constexpr char FW_VERSION[] = "1.6.1";
#include "build_metadata.h"
#include <string_view>
static_assert(std::string_view(FW_VERSION) == FW_BUILD_VERSION,
              "Regenerate metadata using Build-Firmware.ps1");
