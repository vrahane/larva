#include <assert.h>
#include <string.h>
#include "ffs_priv.h"
#include "ffs/ffs.h"

int
ffs_format_from_scratch_sector(uint16_t sector_id)
{
    struct ffs_disk_sector disk_sector;
    struct ffs_sector_info *sector;
    int rc;

    assert(sector_id < ffs_num_sectors);
    sector = ffs_sectors + sector_id;

    rc = ffs_flash_read(sector_id, 0, &disk_sector, sizeof disk_sector);
    if (rc != 0) {
        return rc;
    }

    if (!ffs_sector_is_scratch(&disk_sector)) {
        rc = flash_erase_sector(sector->fsi_offset);
        if (rc != 0) {
            return rc;
        }

        ffs_sector_set_magic(&disk_sector);
        rc = ffs_flash_write(sector_id, 0, &disk_sector,
                             sizeof disk_sector.fds_magic);
        if (rc != 0) {
            return rc;
        }
    } else {
        disk_sector.fds_id = sector_id;
        rc = ffs_flash_write(sector_id, FFS_SECTOR_ID_OFFSET,
                             &disk_sector.fds_id, sizeof disk_sector.fds_id);
        if (rc != 0) {
            return rc;
        }
    }

    return 0;
}

int
ffs_format_scratch_sector(uint16_t sector_id)
{
    struct ffs_disk_sector disk_sector;
    struct ffs_sector_info *sector;
    int rc;

    assert(sector_id < ffs_num_sectors);
    sector = ffs_sectors + sector_id;

    rc = flash_erase_sector(sector->fsi_offset);
    if (rc != 0) {
        return rc;
    }
    sector->fsi_cur = 0;

    memset(&disk_sector, 0, sizeof disk_sector);
    ffs_sector_set_magic(&disk_sector);

    rc = ffs_flash_write(sector_id, 0, &disk_sector,
                         sizeof disk_sector.fds_magic);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

int
ffs_format_sector(uint16_t sector_id)
{
    struct ffs_disk_sector disk_sector;
    struct ffs_sector_info *sector;
    int rc;

    assert(sector_id < ffs_num_sectors);
    sector = ffs_sectors + sector_id;

    rc = flash_erase_sector(sector->fsi_offset);
    if (rc != 0) {
        return rc;
    }
    sector->fsi_cur = 0;

    memset(&disk_sector, 0, sizeof disk_sector);
    ffs_sector_set_magic(&disk_sector);

    rc = ffs_flash_write(sector_id, 0, &disk_sector, sizeof disk_sector);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

int
ffs_format_full(void)
{
    uint16_t sector_id;
    int rc;
    int i;

    /* Select largest sector to be the initial scratch sector. */
    ffs_scratch_sector_id = 0;
    for (i = 1; i < ffs_num_sectors; i++) {
        if (ffs_sectors[i].fsi_length >
            ffs_sectors[ffs_scratch_sector_id].fsi_length) {

            ffs_scratch_sector_id = i;
        }
    }

    for (i = 0; i < ffs_num_sectors; i++) {
        if (i == ffs_scratch_sector_id) {
            rc = ffs_format_scratch_sector(i);
        } else {
            rc = ffs_format_sector(i);
        }
        if (rc != 0) {
            return rc;
        }
    }

    ffs_free_all();
    ffs_next_id = 0;
    ffs_root_dir = NULL;

    /* Create root directory. */
    rc = ffs_new_file(&ffs_root_dir, NULL, "", 0, 1);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
