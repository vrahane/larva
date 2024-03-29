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

#ifndef H_NFFS_PRIV_
#define H_NFFS_PRIV_

#include <inttypes.h>
#include "os/queue.h"
#include "os/os_mempool.h"
#include "nffs/nffs.h"

#define NFFS_HASH_SIZE               256

#define NFFS_ID_DIR_MIN              0
#define NFFS_ID_DIR_MAX              0x10000000
#define NFFS_ID_FILE_MIN             0x10000000
#define NFFS_ID_FILE_MAX             0x80000000
#define NFFS_ID_BLOCK_MIN            0x80000000
#define NFFS_ID_BLOCK_MAX            0xffffffff

#define NFFS_ID_ROOT_DIR             0
#define NFFS_ID_NONE                 0xffffffff

#define NFFS_AREA_MAGIC0             0xb98a31e2
#define NFFS_AREA_MAGIC1             0x7fb0428c
#define NFFS_AREA_MAGIC2             0xace08253
#define NFFS_AREA_MAGIC3             0xb185fc8e
#define NFFS_BLOCK_MAGIC             0x53ba23b9
#define NFFS_INODE_MAGIC             0x925f8bc0

#define NFFS_AREA_ID_NONE            0xff
#define NFFS_AREA_VER                0
#define NFFS_AREA_OFFSET_ID          23

#define NFFS_SHORT_FILENAME_LEN      3

#define NFFS_BLOCK_MAX_DATA_SZ_MAX   2048

/** On-disk representation of an area header. */
struct nffs_disk_area {
    uint32_t nda_magic[4];  /* NFFS_AREA_MAGIC{0,1,2,3} */
    uint32_t nda_length;    /* Total size of area, in bytes. */
    uint8_t nda_ver;        /* Current nffs version: 0 */
    uint8_t nda_gc_seq;     /* Garbage collection count. */
    uint8_t reserved8;
    uint8_t nda_id;         /* 0xff if scratch area. */
};

/** On-disk representation of an inode (file or directory). */
struct nffs_disk_inode {
    uint32_t ndi_magic;         /* NFFS_INODE_MAGIC */
    uint32_t ndi_id;            /* Unique object ID. */
    uint32_t ndi_seq;           /* Sequence number; greater supersedes
                                   lesser. */
    uint32_t ndi_parent_id;     /* Object ID of parent directory inode. */
    uint8_t reserved8;
    uint8_t ndi_filename_len;   /* Length of filename, in bytes. */
    uint16_t ndi_crc16;         /* Covers rest of header and filename. */
    /* Followed by filename. */
};

#define NFFS_DISK_INODE_OFFSET_CRC  18

/** On-disk representation of a data block. */
struct nffs_disk_block {
    uint32_t ndb_magic;     /* NFFS_BLOCK_MAGIC */
    uint32_t ndb_id;        /* Unique object ID. */
    uint32_t ndb_seq;       /* Sequence number; greater supersedes lesser. */
    uint32_t ndb_inode_id;  /* Object ID of owning inode. */
    uint32_t ndb_prev_id;   /* Object ID of previous block in file;
                               NFFS_ID_NONE if this is the first block. */
    uint16_t ndb_data_len;  /* Length of data contents, in bytes. */
    uint16_t ndb_crc16;     /* Covers rest of header and data. */
    /* Followed by 'ndb_data_len' bytes of data. */
};

#define NFFS_DISK_BLOCK_OFFSET_CRC  20

/**
 * What gets stored in the hash table.  Each entry represents a data block or
 * an inode.
 */
struct nffs_hash_entry {
    SLIST_ENTRY(nffs_hash_entry) nhe_next;
    uint32_t nhe_id;        /* 0 - 0x7fffffff if inode; else if block. */
    uint32_t nhe_flash_loc; /* Upper-byte = area idx; rest = area offset. */
};


SLIST_HEAD(nffs_hash_list, nffs_hash_entry);
SLIST_HEAD(nffs_inode_list, nffs_inode_entry);

/** Each inode hash entry is actually one of these. */
struct nffs_inode_entry {
    struct nffs_hash_entry nie_hash_entry;
    SLIST_ENTRY(nffs_inode_entry) nie_sibling_next;
    union {
        struct nffs_inode_list nie_child_list;           /* If directory */
        struct nffs_hash_entry *nie_last_block_entry;    /* If file */
    };
    uint8_t nie_refcnt;
};

/** Full inode representation; not stored permanently RAM. */
struct nffs_inode {
    struct nffs_inode_entry *ni_inode_entry; /* Points to real inode entry. */
    uint32_t ni_seq;                         /* Sequence number; greater
                                                supersedes lesser. */
    struct nffs_inode_entry *ni_parent;      /* Points to parent directory. */
    uint8_t ni_filename_len;                 /* # chars in filename. */
    uint8_t ni_filename[NFFS_SHORT_FILENAME_LEN]; /* First 3 bytes. */
};

/** Full data block representation; not stored permanently RAM. */
struct nffs_block {
    struct nffs_hash_entry *nb_hash_entry;   /* Points to real block entry. */
    uint32_t nb_seq;                         /* Sequence number; greater
                                                supersedes lesser. */
    struct nffs_inode_entry *nb_inode_entry; /* Owning inode. */
    struct nffs_hash_entry *nb_prev;         /* Previous block in file. */
    uint16_t nb_data_len;                    /* # of data bytes in block. */
    uint16_t reserved16;
};

struct nffs_file {
    struct nffs_inode_entry *nf_inode_entry;
    uint32_t nf_offset;
    uint8_t nf_access_flags;
};

struct nffs_area {
    uint32_t na_offset;
    uint32_t na_length;
    uint32_t na_cur;
    uint16_t na_id;
    uint8_t na_gc_seq;
};

struct nffs_disk_object {
    int ndo_type;
    uint8_t ndo_area_idx;
    uint32_t ndo_offset;
    union {
        struct nffs_disk_inode ndo_disk_inode;
        struct nffs_disk_block ndo_disk_block;
    };
};

struct nffs_seek_info {
    struct nffs_block nsi_last_block;
    uint32_t nsi_block_file_off;
    uint32_t nsi_file_len;
};

#define NFFS_OBJECT_TYPE_INODE   1
#define NFFS_OBJECT_TYPE_BLOCK   2

#define NFFS_PATH_TOKEN_NONE     0
#define NFFS_PATH_TOKEN_BRANCH   1
#define NFFS_PATH_TOKEN_LEAF     2

struct nffs_path_parser {
    int npp_token_type;
    const char *npp_path;
    const char *npp_token;
    int npp_token_len;
    int npp_off;
};

/** Represents a single cached data block. */
struct nffs_cache_block {
    TAILQ_ENTRY(nffs_cache_block) ncb_link; /* Next / prev cached block. */
    struct nffs_block ncb_block;            /* Full data block. */
    uint32_t ncb_file_offset;               /* File offset of this block. */
};

TAILQ_HEAD(nffs_cache_block_list, nffs_cache_block);

/** Represents a single cached file inode. */
struct nffs_cache_inode {
    TAILQ_ENTRY(nffs_cache_inode) nci_link;        /* Sorted; LRU at tail. */
    struct nffs_inode nci_inode;                   /* Full inode. */
    struct nffs_cache_block_list nci_block_list;   /* List of cached blocks. */
    uint32_t nci_file_size;                        /* Total file size. */
};

struct nffs_dirent {
    struct nffs_inode_entry *nde_inode_entry;
};

struct nffs_dir {
    struct nffs_inode_entry *nd_parent_inode_entry;
    struct nffs_dirent nd_dirent;
};

extern void *nffs_file_mem;
extern void *nffs_block_entry_mem;
extern void *nffs_inode_mem;
extern void *nffs_cache_inode_mem;
extern void *nffs_cache_block_mem;
extern void *nffs_dir_mem;
extern struct os_mempool nffs_file_pool;
extern struct os_mempool nffs_dir_pool;
extern struct os_mempool nffs_inode_entry_pool;
extern struct os_mempool nffs_block_entry_pool;
extern struct os_mempool nffs_cache_inode_pool;
extern struct os_mempool nffs_cache_block_pool;
extern uint32_t nffs_hash_next_file_id;
extern uint32_t nffs_hash_next_dir_id;
extern uint32_t nffs_hash_next_block_id;
extern struct nffs_area *nffs_areas;
extern uint8_t nffs_num_areas;
extern uint8_t nffs_scratch_area_idx;
extern uint16_t nffs_block_max_data_sz;

#define NFFS_FLASH_BUF_SZ        256
extern uint8_t nffs_flash_buf[NFFS_FLASH_BUF_SZ];

extern struct nffs_hash_list *nffs_hash;
extern struct nffs_inode_entry *nffs_root_dir;
extern struct nffs_inode_entry *nffs_lost_found_dir;

/* @area */
int nffs_area_magic_is_set(const struct nffs_disk_area *disk_area);
int nffs_area_is_scratch(const struct nffs_disk_area *disk_area);
void nffs_area_to_disk(const struct nffs_area *area,
                       struct nffs_disk_area *out_disk_area);
uint32_t nffs_area_free_space(const struct nffs_area *area);
int nffs_area_find_corrupt_scratch(uint16_t *out_good_idx,
                                   uint16_t *out_bad_idx);

/* @block */
struct nffs_hash_entry *nffs_block_entry_alloc(void);
void nffs_block_entry_free(struct nffs_hash_entry *entry);
int nffs_block_read_disk(uint8_t area_idx, uint32_t area_offset,
                         struct nffs_disk_block *out_disk_block);
int nffs_block_write_disk(const struct nffs_disk_block *disk_block,
                          const void *data,
                          uint8_t *out_area_idx, uint32_t *out_area_offset);
int nffs_block_delete_from_ram(struct nffs_hash_entry *entry);
void nffs_block_delete_list_from_ram(struct nffs_block *first,
                                     struct nffs_block *last);
void nffs_block_delete_list_from_disk(const struct nffs_block *first,
                                      const struct nffs_block *last);
void nffs_block_to_disk(const struct nffs_block *block,
                        struct nffs_disk_block *out_disk_block);
int nffs_block_from_hash_entry_no_ptrs(struct nffs_block *out_block,
                                       struct nffs_hash_entry *entry);
int nffs_block_from_hash_entry(struct nffs_block *out_block,
                               struct nffs_hash_entry *entry);
int nffs_block_read_data(const struct nffs_block *block, uint16_t offset,
                         uint16_t length, void *dst);

/* @cache */
void nffs_cache_inode_delete(const struct nffs_inode_entry *inode_entry);
int nffs_cache_inode_ensure(struct nffs_cache_inode **out_entry,
                            struct nffs_inode_entry *inode_entry);
void nffs_cache_inode_range(const struct nffs_cache_inode *cache_inode,
                            uint32_t *out_start, uint32_t *out_end);
int nffs_cache_seek(struct nffs_cache_inode *cache_inode, uint32_t to,
                    struct nffs_cache_block **out_cache_block);
void nffs_cache_clear(void);

/* @crc */
int nffs_crc_flash(uint16_t initial_crc, uint8_t area_idx,
                   uint32_t area_offset, uint32_t len, uint16_t *out_crc);
uint16_t nffs_crc_disk_block_hdr(const struct nffs_disk_block *disk_block);
int nffs_crc_disk_block_validate(const struct nffs_disk_block *disk_block,
                                uint8_t area_idx, uint32_t area_offset);
void nffs_crc_disk_block_fill(struct nffs_disk_block *disk_block,
                              const void *data);
int nffs_crc_disk_inode_validate(const struct nffs_disk_inode *disk_inode,
                                 uint8_t area_idx, uint32_t area_offset);
void nffs_crc_disk_inode_fill(struct nffs_disk_inode *disk_inode,
                              const char *filename);

/* @config */
void nffs_config_init(void);

/* @dir */
int nffs_dir_open(const char *path, struct nffs_dir **out_dir);
int nffs_dir_read(struct nffs_dir *dir, struct nffs_dirent **out_dirent);
int nffs_dir_close(struct nffs_dir *dir);

/* @file */
int nffs_file_open(struct nffs_file **out_file, const char *filename,
                   uint8_t access_flags);
int nffs_file_seek(struct nffs_file *file, uint32_t offset);
int nffs_file_read(struct nffs_file *file, uint32_t len, void *out_data,
                   uint32_t *out_len);
int nffs_file_close(struct nffs_file *file);
int nffs_file_new(struct nffs_inode_entry *parent, const char *filename,
                  uint8_t filename_len, int is_dir,
                  struct nffs_inode_entry **out_inode_entry);

/* @format */
int nffs_format_area(uint8_t area_idx, int is_scratch);
int nffs_format_from_scratch_area(uint8_t area_idx, uint8_t area_id);
int nffs_format_full(const struct nffs_area_desc *area_descs);

/* @gc */
int nffs_gc(uint8_t *out_area_idx);
int nffs_gc_until(uint32_t space, uint8_t *out_area_idx);

/* @flash */
struct nffs_area *nffs_flash_find_area(uint16_t logical_id);
int nffs_flash_read(uint8_t area_idx, uint32_t offset,
                    void *data, uint32_t len);
int nffs_flash_write(uint8_t area_idx, uint32_t offset,
                     const void *data, uint32_t len);
int nffs_flash_copy(uint8_t area_id_from, uint32_t offset_from,
                    uint8_t area_id_to, uint32_t offset_to,
                    uint32_t len);
uint32_t nffs_flash_loc(uint8_t area_idx, uint32_t offset);
void nffs_flash_loc_expand(uint32_t flash_loc, uint8_t *out_area_idx,
                           uint32_t *out_area_offset);

/* @hash */
int nffs_hash_id_is_dir(uint32_t id);
int nffs_hash_id_is_file(uint32_t id);
int nffs_hash_id_is_inode(uint32_t id);
int nffs_hash_id_is_block(uint32_t id);
struct nffs_hash_entry *nffs_hash_find(uint32_t id);
struct nffs_inode_entry *nffs_hash_find_inode(uint32_t id);
struct nffs_hash_entry *nffs_hash_find_block(uint32_t id);
void nffs_hash_insert(struct nffs_hash_entry *entry);
void nffs_hash_remove(struct nffs_hash_entry *entry);
int nffs_hash_init(void);

/* @inode */
struct nffs_inode_entry *nffs_inode_entry_alloc(void);
void nffs_inode_entry_free(struct nffs_inode_entry *inode_entry);
int nffs_inode_calc_data_length(struct nffs_inode_entry *inode_entry,
                                uint32_t *out_len);
int nffs_inode_data_len(struct nffs_inode_entry *inode_entry,
                        uint32_t *out_len);
uint32_t nffs_inode_parent_id(const struct nffs_inode *inode);
int nffs_inode_delete_from_disk(struct nffs_inode *inode);
int nffs_inode_entry_from_disk(struct nffs_inode_entry *out_inode,
                               const struct nffs_disk_inode *disk_inode,
                               uint8_t area_idx, uint32_t offset);
int nffs_inode_rename(struct nffs_inode_entry *inode_entry,
                      struct nffs_inode_entry *new_parent,
                      const char *new_filename);
void nffs_inode_insert_block(struct nffs_inode *inode,
                             struct nffs_block *block);
int nffs_inode_read_disk(uint8_t area_idx, uint32_t offset,
                         struct nffs_disk_inode *out_disk_inode);
int nffs_inode_write_disk(const struct nffs_disk_inode *disk_inode,
                          const char *filename, uint8_t area_idx,
                          uint32_t offset);
int nffs_inode_dec_refcnt(struct nffs_inode_entry *inode_entry);
int nffs_inode_add_child(struct nffs_inode_entry *parent,
                         struct nffs_inode_entry *child);
void nffs_inode_remove_child(struct nffs_inode *child);
int nffs_inode_is_root(const struct nffs_disk_inode *disk_inode);
int nffs_inode_read_filename(struct nffs_inode_entry *inode_entry,
                             size_t max_len, char *out_name,
                             uint8_t *out_full_len);
int nffs_inode_filename_cmp_ram(const struct nffs_inode *inode,
                                const char *name, int name_len,
                                int *result);
int nffs_inode_filename_cmp_flash(const struct nffs_inode *inode1,
                                  const struct nffs_inode *inode2,
                                  int *result);
int nffs_inode_read(struct nffs_inode_entry *inode_entry, uint32_t offset,
                    uint32_t len, void *data, uint32_t *out_len);
int nffs_inode_seek(struct nffs_inode_entry *inode_entry, uint32_t offset,
                    uint32_t length, struct nffs_seek_info *out_seek_info);
int nffs_inode_from_entry(struct nffs_inode *out_inode,
                          struct nffs_inode_entry *entry);
int nffs_inode_unlink_from_ram(struct nffs_inode *inode,
                               struct nffs_hash_entry **out_next);
int nffs_inode_unlink(struct nffs_inode *inode);

/* @misc */
int nffs_misc_reserve_space(uint16_t space,
                            uint8_t *out_area_idx, uint32_t *out_area_offset);
int nffs_misc_set_num_areas(uint8_t num_areas);
int nffs_misc_validate_root_dir(void);
int nffs_misc_validate_scratch(void);
int nffs_misc_create_lost_found_dir(void);
int nffs_misc_set_max_block_data_len(uint16_t min_data_len);
int nffs_misc_reset(void);

/* @path */
int nffs_path_parse_next(struct nffs_path_parser *parser);
void nffs_path_parser_new(struct nffs_path_parser *parser, const char *path);
int nffs_path_find(struct nffs_path_parser *parser,
                   struct nffs_inode_entry **out_inode_entry,
                   struct nffs_inode_entry **out_parent);
int nffs_path_find_inode_entry(const char *filename,
                               struct nffs_inode_entry **out_inode_entry);
int nffs_path_unlink(const char *filename);
int nffs_path_rename(const char *from, const char *to);
int nffs_path_new_dir(const char *path,
                      struct nffs_inode_entry **out_inode_entry);

/* @restore */
int nffs_restore_full(const struct nffs_area_desc *area_descs);

/* @write */
int nffs_write_to_file(struct nffs_file *file, const void *data, int len);


#define NFFS_HASH_FOREACH(entry, i)                                      \
    for ((i) = 0; (i) < NFFS_HASH_SIZE; (i)++)                 \
        SLIST_FOREACH((entry), &nffs_hash[i], nhe_next)

#define NFFS_FLASH_LOC_NONE  nffs_flash_loc(NFFS_AREA_ID_NONE, 0)

#endif
