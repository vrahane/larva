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
#include "hal/hal_gpio.h"
#include "mcu/stm32f407xx.h"
#include "mcu/stm32f4xx_hal_gpio_ex.h"
#include "mcu/stm32f4_bsp.h"
#include "bsp/bsp.h"
#include <assert.h>

static const struct stm32f4_uart_cfg uart_cfg[UART_CNT] = {
    [0] = {
        .suc_uart = USART6,
        .suc_rcc_reg = &RCC->APB2ENR,
        .suc_rcc_dev = RCC_APB2ENR_USART6EN,
        .suc_pin_tx = 38,
        .suc_pin_rx = 39,
        .suc_pin_rts = -1,
        .suc_pin_cts = -1,
        .suc_pin_af = GPIO_AF8_USART6,
        .suc_irqn = USART6_IRQn
    }
};

const struct stm32f4_uart_cfg *bsp_uart_config(int port)
{
    assert(port < UART_CNT);
    return &uart_cfg[port];
}
