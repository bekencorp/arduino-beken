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

void *__dso_handle = 0;
// newlib's __libc_init_array() still expects these legacy hooks to exist.
void _init(void) {}
void _fini(void) {}

#ifdef ARDUINO_BEKEN_IDE_BUILD
volatile const char build_version[] = __DATE__ " " __TIME__;
#endif
