/**
 * Copyright (c) 2015 Runtime Inc.
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

#include "mcu/stm32f407xx.h"
#include "mcu/stm32f4xx_hal_def.h"
#include "hal/hal_system.h"

void
system_reset(void)
{
    NVIC_SystemReset();
}

uint32_t
HAL_GetTick(void)
{
    return 0;
}

HAL_StatusTypeDef
HAL_InitTick (uint32_t TickPriority)
{
    return HAL_OK;
}
