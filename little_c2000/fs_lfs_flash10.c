// fs_lfs_flash10.c
#include "lfs.h"
#include "flash10_driver.h"

static lfs_t lfs;
static struct lfs_config cfg;

#define FLASH10_START_ADDR 0x086000
#define BLOCK_SIZE 2048
#define BLOCK_COUNT 4  // 4*2KB = 8KB

static int lfs_flash_read(const struct lfs_config *c, lfs_block_t block,
                          lfs_off_t off, void *buffer, lfs_size_t size) {
    return flash10_read(FLASH10_START_ADDR + block * BLOCK_SIZE + off, buffer, size);
}

static int lfs_flash_prog(const struct lfs_config *c, lfs_block_t block,
                          lfs_off_t off, const void *buffer, lfs_size_t size) {
    return flash10_prog(FLASH10_START_ADDR + block * BLOCK_SIZE + off, buffer, size);
}

static int lfs_flash_erase(const struct lfs_config *c, lfs_block_t block) {
    return flash10_erase(FLASH10_START_ADDR + block * BLOCK_SIZE);
}

static int lfs_flash_sync(const struct lfs_config *c) {
    return 0;  // no cache
}

void lfs_flash10_config_init(void) {
    cfg.read = lfs_flash_read;
    cfg.prog = lfs_flash_prog;
    cfg.erase = lfs_flash_erase;
    cfg.sync = lfs_flash_sync;

    cfg.read_size = 64;
    cfg.prog_size = 128;
    cfg.block_size = BLOCK_SIZE;
    cfg.block_count = BLOCK_COUNT;
    cfg.cache_size = 128;
    cfg.lookahead_size = 16;
    cfg.block_cycles = 100;
}

int lfs_flash10_mount(void) {
    lfs_flash10_config_init();
    flash10_init();

    int err = lfs_mount(&lfs, &cfg);
    if (err) {
        lfs_format(&lfs, &cfg);
        err = lfs_mount(&lfs, &cfg);
    }
    return err;
}

int lfs_flash10_file_write(const char *filename, const void *data, size_t len) {
    lfs_file_t file;
    if (lfs_file_open(&lfs, &file, filename, LFS_O_RDWR | LFS_O_CREAT) < 0)
        return -1;
    int res = lfs_file_write(&lfs, &file, data, len);
    lfs_file_close(&lfs, &file);
    return res;
}
