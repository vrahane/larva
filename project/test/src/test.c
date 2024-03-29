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

#include <stddef.h>
#include "os/os_test.h"
#include "nffs/nffs_test.h"
#include "bootutil/bootutil_test.h"
#include "testutil/testutil.h"

int
main(void)
{
    tu_config.tc_print_results = 1;
    tu_init();

    os_test_all();
    nffs_test_all();
    boot_test_all();

    return 0;
}
