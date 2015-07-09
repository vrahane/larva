/**
 * Copyright (c) 2015 Stack Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "os/os.h"
#include "os/queue.h"

static os_time_t g_os_time = 0;

os_time_t  
os_time_get(void)
{
    return (g_os_time);
}

void
os_time_tick(void)
{
    os_sr_t sr;

    OS_ENTER_CRITICAL(sr);
    ++g_os_time;
    OS_EXIT_CRITICAL(sr);
}