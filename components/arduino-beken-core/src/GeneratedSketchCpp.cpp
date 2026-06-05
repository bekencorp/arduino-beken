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

// The make-driven build writes additional sketch C++ sources to build/generated/sketch-extra.cpp.
// Arduino CLI/IDE compiles sibling sketch .cpp files directly as ordinary sketch objects.
#if !defined(ARDUINO_BEKEN_IDE_BUILD) && __has_include("../../../build/generated/sketch-extra.cpp")
#include "../../../build/generated/sketch-extra.cpp"
#endif