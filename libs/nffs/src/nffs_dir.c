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

#include <assert.h>
#include <string.h>
#include "nffs_priv.h"
#include "nffs/nffs.h"

static struct nffs_dir *
nffs_dir_alloc(void)
{
    struct nffs_dir *dir;

    dir = os_memblock_get(&nffs_dir_pool);
    if (dir != NULL) {
        memset(dir, 0, sizeof *dir);
    }

    return dir;
}

static int
nffs_dir_free(struct nffs_dir *dir)
{
    int rc;

    if (dir != NULL) {
        rc = os_memblock_put(&nffs_dir_pool, dir);
        if (rc != 0) {
            return NFFS_EOS;
        }
    }

    return 0;
}

int
nffs_dir_open(const char *path, struct nffs_dir **out_dir)
{
    struct nffs_dir *dir;
    int rc;

    dir = nffs_dir_alloc();
    if (dir == NULL) {
        return NFFS_ENOMEM;
    }

    rc = nffs_path_find_inode_entry(path, &dir->nd_parent_inode_entry);
    if (rc != 0) {
        return rc;
    }

    dir->nd_parent_inode_entry->nie_refcnt++;
    memset(&dir->nd_dirent, 0, sizeof dir->nd_dirent);

    *out_dir = dir;

    return 0;
}

int
nffs_dir_read(struct nffs_dir *dir, struct nffs_dirent **out_dirent)
{
    struct nffs_inode_entry *child;
    int rc;

    if (dir->nd_dirent.nde_inode_entry == NULL) {
        child = SLIST_FIRST(&dir->nd_parent_inode_entry->nie_child_list);
    } else {
        child = SLIST_NEXT(dir->nd_dirent.nde_inode_entry, nie_sibling_next);
        rc = nffs_inode_dec_refcnt(dir->nd_dirent.nde_inode_entry);
        if (rc != 0) {
            /* XXX: Need to clean up anything? */
            return rc;
        }
    }
    dir->nd_dirent.nde_inode_entry = child;

    if (child == NULL) {
        *out_dirent = NULL;
        return NFFS_ENOENT;
    }

    child->nie_refcnt++;
    *out_dirent = &dir->nd_dirent;

    return 0;
}

int
nffs_dir_close(struct nffs_dir *dir)
{
    int rc;

    if (dir == NULL) {
        return 0;
    }

    if (dir->nd_dirent.nde_inode_entry != NULL) {
        rc = nffs_inode_dec_refcnt(dir->nd_dirent.nde_inode_entry);
        if (rc != 0) {
            return rc;
        }
    }

    rc = nffs_inode_dec_refcnt(dir->nd_parent_inode_entry);
    if (rc != 0) {
        return rc;
    }

    rc = nffs_dir_free(dir);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
