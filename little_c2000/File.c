/**
 * @file FileSys_Task.c
 * @brief Base on LittleFS   
 * @author      Ziyang.Wang  
 * @version     V0.0.0
 * @attention
 *  This material is the exclusive property of KUKA Robot Group                
 *  Except as expressly permitted by separate agreement, this material may only
 *  be used by members of the development department of KUKA Robot Group for   
 *  internal development purposes of KUKA Robot Group.
 *  Copyright (C) 2021                                                         
 *  KUKA Robot Group. All Rights Reserved.                                     
 * @htmlonly 
 * <span style="font-weight: bold">History</span> 
 * @endhtmlonly 
 * Version|Author|Date|Describe
 * -------|-------|-------|-------- 
 * V0.0.0|Ziyang.Wang  |04/17/2025      |Module Initial                
 */

/******* I N C L U D E - F I L E S *******************************************/
#include "FileSys_Task.h"
#include "HAL_Flash.h"
#include "lfs.h"
#include 

/******* L O C A L - D E F I N E S *******************************************/
#define START_ADRESS_OFF 0xB8000

#define RAM_BLOCK_SIZE 128          // 修改块大小为 64 字节
#define RAM_BLOCK_COUNT 8         // 块总数（根据需求调整）
#define RAM_CACHE_SIZE 128          // 缓存大小（建议与块大小一致或更小）
#define RAM_LOOKAHEAD_SIZE 16       // 查找缓冲区大小（需为 8 的倍数）S
#define RAM_BLOCK_CYCLES 500       // 每块的最大擦除次数

/******* L O C A L - T Y P E S ***********************************************/ 
/******* L O C A L - D A T A *************************************************/
static uint16_t ram_storage[RAM_BLOCK_SIZE * RAM_BLOCK_COUNT * 2];
static uint8_t lookahead_buffer[RAM_LOOKAHEAD_SIZE] __attribute__((aligned(4)));
/******* L O C A L - F U N C T I O N S ***************************************/
int lfs_deski_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
int lfs_deski_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
int lfs_deski_erase(const struct lfs_config *c, lfs_block_t block);
int lfs_deski_sync(const struct lfs_config *c);

int ram_read(const struct lfs_config *c, lfs_block_t block,
             lfs_off_t off, void *buffer, lfs_size_t size) ;
int ram_prog(const struct lfs_config *c, lfs_block_t block,
             lfs_off_t off, const void *buffer, lfs_size_t size);             
int ram_erase(const struct lfs_config *c, lfs_block_t block);
int ram_sync(const struct lfs_config *c);



static struct lfs_config cfg_ram = {
    .read_size = 4,                          // 最小读取单位
    .prog_size = 4,                         // 最小写入单位
    .block_size = RAM_BLOCK_SIZE,             // 块大小
    .block_count = RAM_BLOCK_COUNT,           // 块总数
    .cache_size = RAM_CACHE_SIZE,             // 缓存大小
    .lookahead_size = RAM_LOOKAHEAD_SIZE,     // 查找缓冲区大小
    .block_cycles = RAM_BLOCK_CYCLES,         // 每块的最大擦除次数

    .read = ram_read,                         // 读取接口
    .prog = ram_prog,                         // 写入接口
    .erase = ram_erase,                       // 擦除接口
    .sync = ram_sync                          // 同步接口
};

static struct lfs_config cfg_flash = {
    .read_size = 4,
    .prog_size = 4,
    .block_size = 8192,
    .block_count = 2,
    .cache_size = 128,
    .lookahead_size = 8,
    .block_cycles = 500,

    .read = lfs_deski_read,
    .erase = lfs_deski_erase,
    .prog = lfs_deski_prog,
    .sync = lfs_deski_sync
};
////////////////////FLASH///////////////////////////////////////////////////////
int lfs_deski_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
    //F2838x_Flash_Read((uint8_t *)buffer, c->block_size * block + off, size);
    uint32_t addr = block * c->block_size + off + START_ADRESS_OFF;
    Flash_Read(addr, (uint16_t *)buffer, size);
    return 0;
}

int lfs_deski_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
    //F2838x_Flash_Write_NoCheck((uint8_t *)buffer, c->block_size * block + off, size);
    uint32_t addr = block * c->block_size + off + START_ADRESS_OFF;
    Flash_Write(addr, (uint16_t *)buffer, size);
    return 0;
}

int lfs_deski_erase(const struct lfs_config *c, lfs_block_t block) {
    uint32_t addr = block * c->block_size
    + START_ADRESS_OFF;
    Flash_Erase(addr);
    return 0;
}

int lfs_deski_sync(const struct lfs_config *c) {
    return 0;
}

////////////////////END FLASH///////////////////////////////////////////////////////

////////////////////RAM///////////////////////////////////////////////////////
int ram_read(const struct lfs_config *c, lfs_block_t block,
             lfs_off_t off, void *buffer, lfs_size_t size) {
    uint16_t *block_start = &ram_storage[block * RAM_BLOCK_SIZE + off];
    uint16_t *dest = (uint16_t*)buffer;

    // 按逻辑单元（16-bit）读取
    for (int i = 0; i < size; i++) {
        dest[i] = block_start[i];
    }
    return 0;
}

int ram_prog(const struct lfs_config *c, lfs_block_t block,
             lfs_off_t off, const void *buffer, lfs_size_t size) {
    uint16_t *block_start = &ram_storage[block * RAM_BLOCK_SIZE + off];
    const uint16_t *src = (const uint16_t*)buffer;

    // 按逻辑单元（16-bit）写入
    for (int i = 0; i < size; i++) {
        // 拆分 16-bit 值为两个 uint16_t 元素
        block_start[i]   = src[i];
    }
    return 0;
}

int ram_erase(const struct lfs_config *c, lfs_block_t block) {
    // 计算块的起始地址
    uint16_t *block_start = &ram_storage[block * RAM_BLOCK_SIZE];

    // 每个逻辑单元（16-bit）由两个 uint16_t 元素组成
    for (int i = 0; i < RAM_BLOCK_SIZE; i++) {
        // 高字节和低字节均置为 0xFF
        block_start[i]   = 0xFFFF;
    }
    return 0;
}


int ram_sync(const struct lfs_config *c) {
    // 在 RAM 中无需实际同步操作
    return 0; // 成功
}
////////////////////END RAM///////////////////////////////////////////////////////





void lfs_test_demo(){
    Flash_Init();
    //uint16_t dataBuffer[] = {0x1234, 0x9ABC, 0x9ABC, 0xDEF0,0x1234, 0x5678, 0x9ABC, 0xDEF0}; // 要写锟斤拷锟斤拷锟斤拷锟�
    uint32_t dataBuffer[] = {0x12345678, 0x9ABCDEF0, 0x12345678, 0x9ABCDEF0};
    Flash_Erase(0);
//
    Flash_Write(0Xb8000,dataBuffer,sizeof(dataBuffer));
    uint32_t readBuffer[sizeof(dataBuffer) / sizeof(uint32_t)];
//
    Flash_Read(0Xb8000,readBuffer,sizeof(dataBuffer));
    Flash_Erase(0Xb8000);
    lfs_t lfs;
    lfs_file_t file;

    for (lfs_block_t block = 0; block < cfg.block_count; block++) {
        int err = ram_erase(&cfg, block);
        if (err != LFS_ERR_OK) {
            // 擦除失败处理（如记录错误日志）
            printf("Erase failed on block %d, error: %d\n", block, err);
            return err;
        }
    }

    // 写入测试
    uint16_t test_value = 0x1234;
    ram_prog(&cfg, 0, 0, &test_value, 1);

    // 读取验证
    uint16_t read_value;
    ram_read(&cfg, 0, 0, &read_value, 1);
    if (read_value != 0x1234) {
        printf("Test failed: Expected 0x1234, got 0x%04X\n", read_value);
    }


    //lfs.lookahead.start = (uint32_t)0x0B8000;

//    int err = lfs_format(&lfs, &cfg);
//    lfs_mount(&lfs, &cfg);


    int err = lfs_mount(&lfs, &cfg);


        // reformat if we can't mount the filesystem
        // this should only happen on the first boot
        if (err) {
            lfs_format(&lfs, &cfg);
            lfs_mount(&lfs, &cfg);
        }

        // read current count
        uint32_t boot_count = 0;
        lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
        lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

        // update boot count
        boot_count += 1;
        lfs_file_rewind(&lfs, &file);
        lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

        // remember the storage is not updated until the file is closed successfully
        lfs_file_close(&lfs, &file);

        // release any resources we were using
        lfs_unmount(&lfs);


}

/******* G L O B A L - F U N C T I O N S *************************************/

/******* F U N C T I O N S - D E F I N I T I O N S ***************************/

/******* E N D ***** (FileSys_Task.c) *********************************************/