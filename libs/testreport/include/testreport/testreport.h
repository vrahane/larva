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

#ifndef H_TESTREPORT_
#define H_TESTREPORT_

struct nffs_area_desc;

struct tr_config {
    const char *tc_base_path;
    const struct nffs_area_desc *tc_area_descs;
};

extern struct tr_config tr_config;

int tr_init(void);

#endif
