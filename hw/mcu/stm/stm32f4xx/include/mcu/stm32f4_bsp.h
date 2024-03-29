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

#ifndef __MCU_STM32F4_BSP_H_
#define __MCU_STM32F4_BSP_H_

/**
 * BSP specific UART settings.
 */
struct stm32f4_uart_cfg {
    USART_TypeDef *suc_uart;			/* UART dev registers */
    volatile uint32_t *suc_rcc_reg;		/* RCC register to modify */
    uint32_t suc_rcc_dev;			/* RCC device ID */
    int8_t suc_pin_tx;				/* pins for IO */
    int8_t suc_pin_rx;
    int8_t suc_pin_rts;
    int8_t suc_pin_cts;
    uint8_t suc_pin_af;				/* AF selection for this */
    IRQn_Type suc_irqn;				/* NVIC IRQn */
};

const struct stm32f4_uart_cfg *bsp_uart_config(int port);

/*
 * Internal API for stm32f4xx mcu specific code.
 */
int hal_gpio_init_af(int pin, uint8_t af_type, enum gpio_pull pull);

#endif /* __MCU_STM32F4_BSP_H_ */
