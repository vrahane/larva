#include <assert.h>
#include "ffs/ffs.h"
#include "ffs_priv.h"

int
ffs_flash_read(uint16_t sector_id, uint32_t offset, void *data, uint32_t len)
{
    const struct ffs_sector_info *sector;
    int rc;

    assert(sector_id < ffs_num_sectors);

    sector = ffs_sectors + sector_id;

    if (offset + len > sector->fsi_length) {
        return FFS_ERANGE;
    }

    rc = flash_read(data, sector->fsi_offset + offset, len);
    return rc;
}

int
ffs_flash_write(uint16_t sector_id, uint32_t offset, const void *data,
                uint32_t len)
{
    struct ffs_sector_info *sector;
    int rc;

    assert(sector_id < ffs_num_sectors);

    sector = ffs_sectors + sector_id;
    assert(offset >= sector->fsi_cur);
    if (offset + len > sector->fsi_length) {
        return FFS_ERANGE;
    }

    rc = flash_write(data, sector->fsi_offset + offset, len);
    if (rc != 0) {
        return FFS_EFLASH_ERROR;
    }

    sector->fsi_cur = offset + len;

    return 0;
}

/** Not thread safe. */
int
ffs_flash_copy(uint16_t sector_id_from, uint32_t offset_from,
               uint16_t sector_id_to, uint32_t offset_to,
               uint32_t len)
{
    static uint8_t buf[256];
    uint32_t chunk_len;
    int rc;

    while (len > 0) {
        if (len > sizeof buf) {
            chunk_len = sizeof buf;
        } else {
            chunk_len = len;
        }

        rc = ffs_flash_read(sector_id_from, offset_from, buf, chunk_len);
        if (rc != 0) {
            return rc;
        }

        rc = ffs_flash_write(sector_id_to, offset_to, buf, chunk_len);
        if (rc != 0) {
            return rc;
        }

        offset_from += chunk_len;
        offset_to += chunk_len;
        len -= chunk_len;
    }

    return 0;
}
