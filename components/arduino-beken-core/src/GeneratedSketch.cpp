// Copyright 2025-2026 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Arduino.h"

#define ARDUINO_BEKEN_STRINGIFY_IMPL(x) #x
#define ARDUINO_BEKEN_STRINGIFY(x) ARDUINO_BEKEN_STRINGIFY_IMPL(x)

// The make-driven build writes the prepared sketch to build/generated/sketch.cpp.
// Include it from a stable translation unit so sketch changes do not rely on
// CMake reconfiguration to alter the component source list.
#if !defined(ARDUINO_BEKEN_IDE_BUILD) && __has_include("../../../build/generated/sketch.cpp")
#include "../../../build/generated/sketch.cpp"
#elif defined(ARDUINO_BEKEN_CLI_PROJECT_SOURCE)
#include ARDUINO_BEKEN_STRINGIFY(ARDUINO_BEKEN_CLI_PROJECT_SOURCE)
#endif