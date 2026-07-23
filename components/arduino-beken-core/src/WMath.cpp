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
#include <stdlib.h>

// map() is provided by api/Common.cpp.
// random()/randomSeed() are Arduino WMath helpers used by sketches.

long random(long howbig)
{
    if (howbig == 0) {
        return 0;
    }
    return (long)((unsigned long)rand() % (unsigned long)howbig);
}

long random(long howsmall, long howbig)
{
    if (howsmall >= howbig) {
        return howsmall;
    }
    long diff = howbig - howsmall;
    return random(diff) + howsmall;
}

void randomSeed(unsigned long seed)
{
    if (seed != 0) {
        srand((unsigned int)seed);
    }
}
