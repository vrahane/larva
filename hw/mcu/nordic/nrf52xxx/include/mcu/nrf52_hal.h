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

#ifndef H_NRF52_HAL_
#define H_NRF52_HAL_

#ifdef __cplusplus
 extern "C" {
#endif

/* Helper functions to enable/disable interrupts. */
#define __HAL_DISABLE_INTERRUPTS(x)                     \
    do {                                                \
        x = __get_PRIMASK();                            \
        __disable_irq();                                \
    } while(0);

#define __HAL_ENABLE_INTERRUPTS(x)                      \
    do {                                                \
        if (!x) {                                       \
            __enable_irq();                             \
        }                                               \
    } while(0);

struct nrf52_uart_cfg {
    int8_t suc_pin_tx;                          /* pins for IO */
    int8_t suc_pin_rx;
    int8_t suc_pin_rts;
    int8_t suc_pin_cts;
};
const struct nrf52_uart_cfg *bsp_uart_config(void);

#ifdef __cplusplus
}
#endif

#endif  /* H_NRF52_HAL_ */

