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

#ifndef H_OS_TEST_PRIV_
#define H_OS_TEST_PRIV_

void os_test_restart(void);

int os_mempool_test_suite(void);
int os_mbuf_test_suite(void);
int os_mutex_test_suite(void);
int os_sem_test_suite(void);

#endif
