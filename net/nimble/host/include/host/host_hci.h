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

#ifndef H_HOST_HCI_
#define H_HOST_HCI_

#include "nimble/hci_common.h"

int host_hci_init(void);
void host_hci_task(void *arg);
int host_hci_cmd_le_set_scan_rsp_data(uint8_t *data, uint8_t len);
int host_hci_cmd_le_set_adv_data(uint8_t *data, uint8_t len);
int host_hci_cmd_le_set_adv_params(struct hci_adv_params *adv);
int host_hci_cmd_le_set_rand_addr(uint8_t *addr);
int host_hci_cmd_le_set_event_mask(uint64_t event_mask);
int host_hci_cmd_le_set_adv_enable(uint8_t enable);
int host_hci_cmd_le_set_scan_enable(uint8_t enable, uint8_t filter_dups);
int host_hci_cmd_le_set_scan_params(uint8_t scan_type, uint16_t scan_itvl, 
                                    uint16_t scan_window, uint8_t own_addr_type,
                                    uint8_t filter_policy);

#endif /* H_HOST_HCI_ */
