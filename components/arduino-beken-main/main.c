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

#include <os/os.h>

#include "bk_private/bk_init.h"

extern void user_app_main(void);
extern void rtos_set_user_app_entry(beken_thread_function_t entry);

int main(void) {
#if (CONFIG_SYS_CPU0)
    rtos_set_user_app_entry((beken_thread_function_t)user_app_main);
#endif

    bk_init();
    return 0;
}
