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
#include <stdint.h>
#include "mcu/nrf52_hal.h"

static const struct nrf52_uart_cfg uart_cfg = {
    .suc_pin_tx = 6,
    .suc_pin_rx = 8,
    .suc_pin_rts = 5,
    .suc_pin_cts = 7
};

const struct nrf52_uart_cfg *bsp_uart_config(void)
{
    return &uart_cfg;
}
