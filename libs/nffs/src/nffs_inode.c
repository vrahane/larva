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
#include "testutil/testutil.h"
#include "os/os_mempool.h"
#include "nffs/nffs.h"
#include "nffs_priv.h"
#include "crc16.h"

/* Partition the flash buffer into two equal halves; used for filename
 * comparisons.
 */
#define NFFS_INODE_FILENAME_BUF_SZ   (sizeof nffs_flash_buf / 2)
static uint8_t *nffs_inode_filename_buf0 = nffs_flash_buf;
static uint8_t *nffs_inode_filename_buf1 =
    nffs_flash_buf + NFFS_INODE_FILENAME_BUF_SZ;

/** A list of directory inodes with pending unlink operations. */
static struct nffs_hash_list nffs_inode_unlink_list;

struct nffs_inode_entry *
nffs_inode_entry_alloc(void)
{
    struct nffs_inode_entry *inode_entry;

    inode_entry = os_memblock_get(&nffs_inode_entry_pool);
    if (inode_entry != NULL) {
        memset(inode_entry, 0, sizeof *inode_entry);
    }

    return inode_entry;
}

void
nffs_inode_entry_free(struct nffs_inode_entry *inode_entry)
{
    if (inode_entry != NULL) {
        assert(nffs_hash_id_is_inode(inode_entry->nie_hash_entry.nhe_id));
        os_memblock_put(&nffs_inode_entry_pool, inode_entry);
    }
}

uint32_t
nffs_inode_disk_size(const struct nffs_inode *inode)
{
    return sizeof (struct nffs_disk_inode) + inode->ni_filename_len;
}

int
nffs_inode_read_disk(uint8_t area_idx, uint32_t offset,
                    struct nffs_disk_inode *out_disk_inode)
{
    int rc;

    rc = nffs_flash_read(area_idx, offset, out_disk_inode,
                         sizeof *out_disk_inode);
    if (rc != 0) {
        return rc;
    }
    if (out_disk_inode->ndi_magic != NFFS_INODE_MAGIC) {
        return NFFS_EUNEXP;
    }

    return 0;
}

int
nffs_inode_write_disk(const struct nffs_disk_inode *disk_inode,
                      const char *filename, uint8_t area_idx,
                      uint32_t area_offset)
{
    int rc;

    rc = nffs_flash_write(area_idx, area_offset, disk_inode,
                          sizeof *disk_inode);
    if (rc != 0) {
        return rc;
    }

    if (disk_inode->ndi_filename_len != 0) {
        rc = nffs_flash_write(area_idx, area_offset + sizeof *disk_inode,
                              filename, disk_inode->ndi_filename_len);
        if (rc != 0) {
            return rc;
        }
    }

    ASSERT_IF_TEST(nffs_crc_disk_inode_validate(disk_inode, area_idx,
                                                area_offset) == 0);

    return 0;
}

int
nffs_inode_calc_data_length(struct nffs_inode_entry *inode_entry,
                            uint32_t *out_len)
{
    struct nffs_hash_entry *cur;
    struct nffs_block block;
    int rc;

    assert(nffs_hash_id_is_file(inode_entry->nie_hash_entry.nhe_id));

    *out_len = 0;

    cur = inode_entry->nie_last_block_entry;
    while (cur != NULL) {
        rc = nffs_block_from_hash_entry(&block, cur);
        if (rc != 0) {
            return rc;
        }

        *out_len += block.nb_data_len;

        cur = block.nb_prev;
    }

    return 0;
}

int
nffs_inode_data_len(struct nffs_inode_entry *inode_entry, uint32_t *out_len)
{
    struct nffs_cache_inode *cache_inode;
    int rc;

    rc = nffs_cache_inode_ensure(&cache_inode, inode_entry);
    if (rc != 0) {
        return rc;
    }

    *out_len = cache_inode->nci_file_size;

    return 0;
}

int
nffs_inode_from_entry(struct nffs_inode *out_inode,
                      struct nffs_inode_entry *entry)
{
    struct nffs_disk_inode disk_inode;
    uint32_t area_offset;
    uint8_t area_idx;
    int cached_name_len;
    int rc;

    nffs_flash_loc_expand(entry->nie_hash_entry.nhe_flash_loc,
                          &area_idx, &area_offset);

    rc = nffs_inode_read_disk(area_idx, area_offset, &disk_inode);
    if (rc != 0) {
        return rc;
    }

    out_inode->ni_inode_entry = entry;
    out_inode->ni_seq = disk_inode.ndi_seq;
    if (disk_inode.ndi_parent_id == NFFS_ID_NONE) {
        out_inode->ni_parent = NULL;
    } else {
        out_inode->ni_parent = nffs_hash_find_inode(disk_inode.ndi_parent_id);
    }
    out_inode->ni_filename_len = disk_inode.ndi_filename_len;

    if (out_inode->ni_filename_len > NFFS_SHORT_FILENAME_LEN) {
        cached_name_len = NFFS_SHORT_FILENAME_LEN;
    } else {
        cached_name_len = out_inode->ni_filename_len;
    }
    if (cached_name_len != 0) {
        rc = nffs_flash_read(area_idx, area_offset + sizeof disk_inode,
                             out_inode->ni_filename, cached_name_len);
        if (rc != 0) {
            return rc;
        }
    }

    return 0;
}

uint32_t
nffs_inode_parent_id(const struct nffs_inode *inode)
{
    if (inode->ni_parent == NULL) {
        return NFFS_ID_NONE;
    } else {
        return inode->ni_parent->nie_hash_entry.nhe_id;
    }
}

static int
nffs_inode_delete_blocks_from_ram(struct nffs_inode_entry *inode_entry)
{
    int rc;

    assert(nffs_hash_id_is_file(inode_entry->nie_hash_entry.nhe_id));

    while (inode_entry->nie_last_block_entry != NULL) {
        rc = nffs_block_delete_from_ram(inode_entry->nie_last_block_entry);
        if (rc != 0) {
            return rc;
        }
    }

    return 0;
}

static int
nffs_inode_delete_from_ram(struct nffs_inode_entry *inode_entry)
{
    int rc;

    if (nffs_hash_id_is_file(inode_entry->nie_hash_entry.nhe_id)) {
        rc = nffs_inode_delete_blocks_from_ram(inode_entry);
        if (rc != 0) {
            return rc;
        }
    }

    nffs_cache_inode_delete(inode_entry);
    nffs_hash_remove(&inode_entry->nie_hash_entry);
    nffs_inode_entry_free(inode_entry);

    return 0;
}

/**
 * Inserts the specified inode entry into the unlink list.  Because a hash
 * entry only has a single 'next' pointer, this function removes the entry from
 * the hash table prior to inserting it into the unlink list.
 *
 * @param inode_entry           The inode entry to insert.
 */
static void
nffs_inode_insert_unlink_list(struct nffs_inode_entry *inode_entry)
{
    nffs_hash_remove(&inode_entry->nie_hash_entry);
    SLIST_INSERT_HEAD(&nffs_inode_unlink_list, &inode_entry->nie_hash_entry,
                      nhe_next);
}

/**
 * Decrements the reference count of the specified inode entry.
 *
 * @param inode_entry               The inode entry whose reference count
 *                                      should be decremented.
 */
int
nffs_inode_dec_refcnt(struct nffs_inode_entry *inode_entry)
{
    int rc;

    assert(inode_entry->nie_refcnt > 0);

    inode_entry->nie_refcnt--;
    if (inode_entry->nie_refcnt == 0) {
        if (nffs_hash_id_is_file(inode_entry->nie_hash_entry.nhe_id)) {
            rc = nffs_inode_delete_from_ram(inode_entry);
            if (rc != 0) {
                return rc;
            }
        } else {
            nffs_inode_insert_unlink_list(inode_entry);
        }
    }

    return 0;
}

/**
 * Unlinks every directory inode entry present in the unlink list.  After this
 * function completes:
 *     o Each descendant directory is deleted from RAM.
 *     o Each descendant file has its reference count decremented (and deleted
 *       from RAM if its reference count reaches zero).
 *
 * @param inout_next            This parameter is only necessary if you are
 *                                  calling this function during an iteration
 *                                  of the entire hash table; pass null
 *                                  otherwise.
 *                              On input, this points to the next hash entry
 *                                  you plan on processing.
 *                              On output, this points to the next hash entry
 *                                  that should be processed.
 */
static int
nffs_inode_process_unlink_list(struct nffs_hash_entry **inout_next)
{
    struct nffs_inode_entry *inode_entry;
    struct nffs_inode_entry *child_next;
    struct nffs_inode_entry *child;
    struct nffs_hash_entry *hash_entry;
    int rc;

    while ((hash_entry = SLIST_FIRST(&nffs_inode_unlink_list)) != NULL) {
        assert(nffs_hash_id_is_dir(hash_entry->nhe_id));

        SLIST_REMOVE_HEAD(&nffs_inode_unlink_list, nhe_next);

        inode_entry = (struct nffs_inode_entry *)hash_entry;

        /* Recursively unlink each child. */
        child = SLIST_FIRST(&inode_entry->nie_child_list);
        while (child != NULL) {
            child_next = SLIST_NEXT(child, nie_sibling_next);

            if (inout_next != NULL && *inout_next == &child->nie_hash_entry) {
                *inout_next = &child_next->nie_hash_entry;
            }

            rc = nffs_inode_dec_refcnt(child);
            if (rc != 0) {
                return rc;
            }

            child = child_next;
        }

        /* The directory is already removed from the hash table; just free its
         * memory.
         */
        nffs_inode_entry_free(inode_entry);
    }

    return 0;
}

int
nffs_inode_delete_from_disk(struct nffs_inode *inode)
{
    struct nffs_disk_inode disk_inode;
    uint32_t offset;
    uint8_t area_idx;
    int rc;

    /* Make sure it isn't already deleted. */
    assert(inode->ni_parent != NULL);

    rc = nffs_misc_reserve_space(sizeof disk_inode, &area_idx, &offset);
    if (rc != 0) {
        return rc;
    }

    inode->ni_seq++;

    disk_inode.ndi_magic = NFFS_INODE_MAGIC;
    disk_inode.ndi_id = inode->ni_inode_entry->nie_hash_entry.nhe_id;
    disk_inode.ndi_seq = inode->ni_seq;
    disk_inode.ndi_parent_id = NFFS_ID_NONE;
    disk_inode.ndi_filename_len = 0;
    nffs_crc_disk_inode_fill(&disk_inode, "");

    rc = nffs_inode_write_disk(&disk_inode, "", area_idx, offset);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

int
nffs_inode_rename(struct nffs_inode_entry *inode_entry,
                 struct nffs_inode_entry *new_parent,
                 const char *new_filename)
{
    struct nffs_disk_inode disk_inode;
    struct nffs_inode inode;
    uint32_t area_offset;
    uint8_t area_idx;
    int filename_len;
    int rc;

    rc = nffs_inode_from_entry(&inode, inode_entry);
    if (rc != 0) {
        return rc;
    }

    if (inode.ni_parent != new_parent) {
        if (inode.ni_parent != NULL) {
            nffs_inode_remove_child(&inode);
        }
        if (new_parent != NULL) {
            rc = nffs_inode_add_child(new_parent, inode.ni_inode_entry);
            if (rc != 0) {
                return rc;
            }
        }
        inode.ni_parent = new_parent;
    }

    if (new_filename != NULL) {
        filename_len = strlen(new_filename);
    } else {
        filename_len = inode.ni_filename_len;
        nffs_flash_loc_expand(inode_entry->nie_hash_entry.nhe_flash_loc,
                              &area_idx, &area_offset);
        rc = nffs_flash_read(area_idx,
                             area_offset + sizeof (struct nffs_disk_inode),
                             nffs_flash_buf, filename_len);
        if (rc != 0) {
            return rc;
        }

        new_filename = (char *)nffs_flash_buf;
    }

    rc = nffs_misc_reserve_space(sizeof disk_inode + filename_len,
                                 &area_idx, &area_offset);
    if (rc != 0) {
        return rc;
    }

    disk_inode.ndi_magic = NFFS_INODE_MAGIC;
    disk_inode.ndi_id = inode_entry->nie_hash_entry.nhe_id;
    disk_inode.ndi_seq = inode.ni_seq + 1;
    disk_inode.ndi_parent_id = nffs_inode_parent_id(&inode);
    disk_inode.ndi_filename_len = filename_len;
    nffs_crc_disk_inode_fill(&disk_inode, new_filename);

    rc = nffs_inode_write_disk(&disk_inode, new_filename, area_idx,
                               area_offset);
    if (rc != 0) {
        return rc;
    }

    inode_entry->nie_hash_entry.nhe_flash_loc =
        nffs_flash_loc(area_idx, area_offset);

    return 0;
}

static int
nffs_inode_read_filename_chunk(const struct nffs_inode *inode,
                               uint8_t filename_offset, void *buf, int len)
{
    uint32_t area_offset;
    uint8_t area_idx;
    int rc;

    assert(filename_offset + len <= inode->ni_filename_len);

    nffs_flash_loc_expand(inode->ni_inode_entry->nie_hash_entry.nhe_flash_loc,
                          &area_idx, &area_offset);
    area_offset += sizeof (struct nffs_disk_inode) + filename_offset;

    rc = nffs_flash_read(area_idx, area_offset, buf, len);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

/**
 * Retrieves the filename of the specified inode.  The retrieved
 * filename is always null-terminated.  To ensure enough space to hold the full
 * filename plus a null-termintor, a destination buffer of size
 * (NFFS_FILENAME_MAX_LEN + 1) should be used.
 *
 * @param inode_entry           The inode entry to query.
 * @param max_len               The size of the "out_name" character buffer.
 * @param out_name              On success, the entry's filename is written
 *                                  here; always null-terminated.
 * @param out_name_len          On success, contains the actual length of the
 *                                  filename, NOT including the
 *                                  null-terminator.
 *
 * @return                      0 on success; nonzero on failure.
 */
int
nffs_inode_read_filename(struct nffs_inode_entry *inode_entry, size_t max_len,
                         char *out_name, uint8_t *out_full_len)
{
    struct nffs_inode inode;
    int read_len;
    int rc;

    rc = nffs_inode_from_entry(&inode, inode_entry);
    if (rc != 0) {
        return rc;
    }

    if (max_len > inode.ni_filename_len) {
        read_len = inode.ni_filename_len;
    } else {
        read_len = max_len - 1;
    }

    rc = nffs_inode_read_filename_chunk(&inode, 0, out_name, read_len);
    if (rc != 0) {
        return rc;
    }

    out_name[read_len] = '\0';

    return 0;
}

int
nffs_inode_add_child(struct nffs_inode_entry *parent,
                     struct nffs_inode_entry *child)
{
    struct nffs_inode_entry *prev;
    struct nffs_inode_entry *cur;
    struct nffs_inode child_inode;
    struct nffs_inode cur_inode;
    int cmp;
    int rc;

    assert(nffs_hash_id_is_dir(parent->nie_hash_entry.nhe_id));

    rc = nffs_inode_from_entry(&child_inode, child);
    if (rc != 0) {
        return rc;
    }

    prev = NULL;
    SLIST_FOREACH(cur, &parent->nie_child_list, nie_sibling_next) {
        assert(cur != child);
        rc = nffs_inode_from_entry(&cur_inode, cur);
        if (rc != 0) {
            return rc;
        }

        rc = nffs_inode_filename_cmp_flash(&child_inode, &cur_inode, &cmp);
        if (rc != 0) {
            return rc;
        }

        if (cmp < 0) {
            break;
        }

        prev = cur;
    }

    if (prev == NULL) {
        SLIST_INSERT_HEAD(&parent->nie_child_list, child, nie_sibling_next);
    } else {
        SLIST_INSERT_AFTER(prev, child, nie_sibling_next);
    }

    return 0;
}

void
nffs_inode_remove_child(struct nffs_inode *child)
{
    struct nffs_inode_entry *parent;

    parent = child->ni_parent;
    assert(parent != NULL);
    assert(nffs_hash_id_is_dir(parent->nie_hash_entry.nhe_id));
    SLIST_REMOVE(&parent->nie_child_list, child->ni_inode_entry,
                 nffs_inode_entry, nie_sibling_next);
    SLIST_NEXT(child->ni_inode_entry, nie_sibling_next) = NULL;
}

int
nffs_inode_filename_cmp_ram(const struct nffs_inode *inode,
                            const char *name, int name_len,
                            int *result)
{
    int short_len;
    int chunk_len;
    int rem_len;
    int off;
    int rc;

    if (name_len < inode->ni_filename_len) {
        short_len = name_len;
    } else {
        short_len = inode->ni_filename_len;
    }

    if (short_len <= NFFS_SHORT_FILENAME_LEN) {
        chunk_len = short_len;
    } else {
        chunk_len = NFFS_SHORT_FILENAME_LEN;
    }
    *result = strncmp((char *)inode->ni_filename, name, chunk_len);

    off = chunk_len;
    while (*result == 0 && off < short_len) {
        rem_len = short_len - off;
        if (rem_len > NFFS_INODE_FILENAME_BUF_SZ) {
            chunk_len = NFFS_INODE_FILENAME_BUF_SZ;
        } else {
            chunk_len = rem_len;
        }

        rc = nffs_inode_read_filename_chunk(inode, off,
                                            nffs_inode_filename_buf0,
                                            chunk_len);
        if (rc != 0) {
            return rc;
        }

        *result = strncmp((char *)nffs_inode_filename_buf0, name + off,
                          chunk_len);
        off += chunk_len;
    }

    if (*result == 0) {
        *result = inode->ni_filename_len - name_len;
    }

    return 0;
}

int
nffs_inode_filename_cmp_flash(const struct nffs_inode *inode1,
                              const struct nffs_inode *inode2,
                              int *result)
{
    int short_len;
    int chunk_len;
    int rem_len;
    int off;
    int rc;

    if (inode1->ni_filename_len < inode2->ni_filename_len) {
        short_len = inode1->ni_filename_len;
    } else {
        short_len = inode2->ni_filename_len;
    }

    if (short_len <= NFFS_SHORT_FILENAME_LEN) {
        chunk_len = short_len;
    } else {
        chunk_len = NFFS_SHORT_FILENAME_LEN;
    }
    *result = strncmp((char *)inode1->ni_filename,
                      (char *)inode2->ni_filename,
                      chunk_len);

    off = chunk_len;
    while (*result == 0 && off < short_len) {
        rem_len = short_len - off;
        if (rem_len > NFFS_INODE_FILENAME_BUF_SZ) {
            chunk_len = NFFS_INODE_FILENAME_BUF_SZ;
        } else {
            chunk_len = rem_len;
        }

        rc = nffs_inode_read_filename_chunk(inode1, off,
                                            nffs_inode_filename_buf0,
                                            chunk_len);
        if (rc != 0) {
            return rc;
        }

        rc = nffs_inode_read_filename_chunk(inode2, off,
                                            nffs_inode_filename_buf1,
                                            chunk_len);
        if (rc != 0) {
            return rc;
        }

        *result = strncmp((char *)nffs_inode_filename_buf0,
                          (char *)nffs_inode_filename_buf1,
                          chunk_len);
        off += chunk_len;
    }

    if (*result == 0) {
        *result = inode1->ni_filename_len - inode2->ni_filename_len;
    }

    return 0;
}

/**
 * Finds the set of blocks composing the specified address range within the
 * supplied file inode.  This information is returned in the form of a
 * seek_info object.
 *
 * @param inode_entry           The file inode to seek within.
 * @param offset                The start address of the region to seek to.
 * @param length                The length of the region to seek to.
 * @param out_seek_info         On success, this gets populated with the result
 *                                  of the seek operation.
 *
 * @return                      0 on success; nonzero on failure.
 */
int
nffs_inode_seek(struct nffs_inode_entry *inode_entry, uint32_t offset,
                uint32_t length, struct nffs_seek_info *out_seek_info)
{
    struct nffs_cache_inode *cache_inode;
    struct nffs_hash_entry *cur_entry;
    struct nffs_block block;
    uint32_t block_start;
    uint32_t cur_offset;
    uint32_t seek_end;
    int rc;

    assert(length > 0);
    assert(nffs_hash_id_is_file(inode_entry->nie_hash_entry.nhe_id));

    rc = nffs_cache_inode_ensure(&cache_inode, inode_entry);
    if (rc != 0) {
        return rc;
    }

    if (offset > cache_inode->nci_file_size) {
        return NFFS_ERANGE;
    }
    if (offset == cache_inode->nci_file_size) {
        memset(&out_seek_info->nsi_last_block, 0,
               sizeof out_seek_info->nsi_last_block);
        out_seek_info->nsi_last_block.nb_hash_entry = NULL;
        out_seek_info->nsi_block_file_off = 0;
        out_seek_info->nsi_file_len = cache_inode->nci_file_size;
        return 0;
    }

    seek_end = offset + length;

    cur_entry = inode_entry->nie_last_block_entry;
    cur_offset = cache_inode->nci_file_size;

    while (1) {
        rc = nffs_block_from_hash_entry(&block, cur_entry);
        if (rc != 0) {
            return rc;
        }

        block_start = cur_offset - block.nb_data_len;
        if (seek_end > block_start) {
            out_seek_info->nsi_last_block = block;
            out_seek_info->nsi_block_file_off = block_start;
            out_seek_info->nsi_file_len = cache_inode->nci_file_size;
            return 0;
        }

        cur_offset = block_start;
        cur_entry = block.nb_prev;
    }
}

/**
 * Reads data from the specified file inode.
 *
 * @param inode_entry           The inode to read from.
 * @param offset                The offset within the file to start the read
 *                                  at.
 * @param len                   The number of bytes to attempt to read.
 * @param out_data              On success, the read data gets written here.
 * @param out_len               On success, the number of bytes actually read
 *                                  gets written here.
 *
 * @return                      0 on success; nonzero on failure.
 */
int
nffs_inode_read(struct nffs_inode_entry *inode_entry, uint32_t offset,
                uint32_t len, void *out_data, uint32_t *out_len)
{
    struct nffs_cache_inode *cache_inode;
    struct nffs_cache_block *cache_block;
    uint32_t block_end;
    uint32_t dst_off;
    uint32_t src_off;
    uint32_t src_end;
    uint16_t block_off;
    uint16_t chunk_sz;
    uint8_t *dptr;
    int rc;

    if (len == 0) {
        if (out_len != NULL) {
            *out_len = 0;
        }
        return 0;
    }

    rc = nffs_cache_inode_ensure(&cache_inode, inode_entry);
    if (rc != 0) {
        return rc;
    }

    src_end = offset + len;
    if (src_end > cache_inode->nci_file_size) {
        src_end = cache_inode->nci_file_size;
    }

    /* Initialize variables for the first iteration. */
    dst_off = src_end - offset;
    src_off = src_end;
    dptr = out_data;
    cache_block = NULL;

    /* Read each relevant block into the destination buffer, iterating in
     * reverse.
     */
    while (dst_off > 0) {
        if (cache_block == NULL) {
            rc = nffs_cache_seek(cache_inode, src_off - 1, &cache_block);
            if (rc != 0) {
                return rc;
            }
        }

        if (cache_block->ncb_file_offset < offset) {
            block_off = offset - cache_block->ncb_file_offset;
        } else {
            block_off = 0;
        }

        block_end = cache_block->ncb_file_offset +
                    cache_block->ncb_block.nb_data_len;
        chunk_sz = cache_block->ncb_block.nb_data_len - block_off;
        if (block_end > src_end) {
            chunk_sz -= block_end - src_end;
        }

        dst_off -= chunk_sz;
        src_off -= chunk_sz;

        rc = nffs_block_read_data(&cache_block->ncb_block, block_off, chunk_sz,
                                  dptr + dst_off);
        if (rc != 0) {
            return rc;
        }

        cache_block = TAILQ_PREV(cache_block, nffs_cache_block_list, ncb_link);
    }

    if (out_len != NULL) {
        *out_len = src_end - offset;
    }

    return 0;
}

int
nffs_inode_unlink_from_ram(struct nffs_inode *inode,
                           struct nffs_hash_entry **out_next)
{
    int rc;

    if (inode->ni_parent != NULL) {
        nffs_inode_remove_child(inode);
    }

    if (nffs_hash_id_is_dir(inode->ni_inode_entry->nie_hash_entry.nhe_id)) {
        nffs_inode_insert_unlink_list(inode->ni_inode_entry);
        rc = nffs_inode_process_unlink_list(out_next);
    } else {
        rc = nffs_inode_dec_refcnt(inode->ni_inode_entry);
    }
    if (rc != 0) {
        return rc;
    }

    return 0;
}

/**
 * Unlinks the file or directory represented by the specified inode.  If the
 * inode represents a directory, all the directory's descendants are
 * recursively unlinked.  Any open file handles refering to an unlinked file
 * remain valid, and can be read from and written to.
 *
 * When an inode is unlinked, the following events occur:
 *     o inode deletion record is written to disk.
 *     o inode is removed from parent's child list.
 *     o inode's reference count is decreased (if this brings it to zero, the
 *       inode is fully deleted from RAM).
 *
 * @param inode             The inode to unlink.
 *
 * @return                  0 on success; nonzero on failure.
 */
int
nffs_inode_unlink(struct nffs_inode *inode)
{
    int rc;

    rc = nffs_inode_delete_from_disk(inode);
    if (rc != 0) {
        return rc;
    }

    rc = nffs_inode_unlink_from_ram(inode, NULL);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
