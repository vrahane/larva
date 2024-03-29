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
#include <string.h>
#include <assert.h>
#include "nffs/nffs.h"
#include "nffs_priv.h"

struct nffs_hash_list *nffs_hash;

uint32_t nffs_hash_next_dir_id;
uint32_t nffs_hash_next_file_id;
uint32_t nffs_hash_next_block_id;

int
nffs_hash_id_is_dir(uint32_t id)
{
    return id >= NFFS_ID_DIR_MIN && id < NFFS_ID_DIR_MAX;
}

int
nffs_hash_id_is_file(uint32_t id)
{
    return id >= NFFS_ID_FILE_MIN && id < NFFS_ID_FILE_MAX;
}

int
nffs_hash_id_is_inode(uint32_t id)
{
    return nffs_hash_id_is_dir(id) || nffs_hash_id_is_file(id);
}

int
nffs_hash_id_is_block(uint32_t id)
{
    return id >= NFFS_ID_BLOCK_MIN && id < NFFS_ID_BLOCK_MAX;
}

static int
nffs_hash_fn(uint32_t id)
{
    return id % NFFS_HASH_SIZE;
}

struct nffs_hash_entry *
nffs_hash_find(uint32_t id)
{
    struct nffs_hash_entry *entry;
    struct nffs_hash_entry *prev;
    struct nffs_hash_list *list;
    int idx;

    idx = nffs_hash_fn(id);
    list = nffs_hash + idx;

    prev = NULL;
    SLIST_FOREACH(entry, list, nhe_next) {
        if (entry->nhe_id == id) {
            /* Put entry at the front of the list. */
            if (prev != NULL) {
                SLIST_NEXT(prev, nhe_next) = SLIST_NEXT(entry, nhe_next);
                SLIST_INSERT_HEAD(list, entry, nhe_next);
            }
            return entry;
        }

        prev = entry;
    }

    return NULL;
}

struct nffs_inode_entry *
nffs_hash_find_inode(uint32_t id)
{
    struct nffs_hash_entry *entry;

    assert(nffs_hash_id_is_inode(id));

    entry = nffs_hash_find(id);
    return (struct nffs_inode_entry *)entry;
}

struct nffs_hash_entry *
nffs_hash_find_block(uint32_t id)
{
    struct nffs_hash_entry *entry;

    assert(nffs_hash_id_is_block(id));

    entry = nffs_hash_find(id);
    return entry;
}

void
nffs_hash_insert(struct nffs_hash_entry *entry)
{
    struct nffs_hash_list *list;
    int idx;

    idx = nffs_hash_fn(entry->nhe_id);
    list = nffs_hash + idx;

    SLIST_INSERT_HEAD(list, entry, nhe_next);
}

void
nffs_hash_remove(struct nffs_hash_entry *entry)
{
    struct nffs_hash_list *list;
    int idx;

    idx = nffs_hash_fn(entry->nhe_id);
    list = nffs_hash + idx;

    SLIST_REMOVE(list, entry, nffs_hash_entry, nhe_next);
}

int
nffs_hash_init(void)
{
    int i;

    free(nffs_hash);

    nffs_hash = malloc(NFFS_HASH_SIZE * sizeof *nffs_hash);
    if (nffs_hash == NULL) {
        return NFFS_ENOMEM;
    }

    for (i = 0; i < NFFS_HASH_SIZE; i++) {
        SLIST_INIT(nffs_hash + i);
    }

    return 0;
}

