/*!
 * \file lfs_c2000_fram.c
 */

// #include "autoconf.h"
#include "lfs.h"
// #include <ti/sysbios/gates/GateMutexPri.h>
// #include <xdc/runtime/Error.h>
// #include <xdc/runtime/Assert.h>
// #if CONFIG_USE_SPI_FM25W256
// #include "fram/fm25w256/fm25w256.h"
// #endif /* CONFIG_USE_SPI_FM25W256 */

#define RAM_BLOCK_SIZE 128          // 淇敼鍧楀ぇ灏忎负 64 瀛楄妭
#define RAM_BLOCK_COUNT 8         // 鍧楁�绘暟锛堟牴鎹渶姹傝皟鏁达級
#define RAM_CACHE_SIZE 128          // 缂撳瓨澶у皬锛堝缓璁笌鍧楀ぇ灏忎竴鑷存垨鏇村皬锛�
#define RAM_LOOKAHEAD_SIZE 16       // 鏌ユ壘缂撳啿鍖哄ぇ灏忥紙闇�涓� 8 鐨勫�嶆暟锛塖
#define RAM_BLOCK_CYCLES 500       // 姣忓潡鐨勬渶澶ф摝闄ゆ鏁�

/******* L O C A L - T Y P E S ***********************************************/
/******* L O C A L - D A T A *************************************************/
static uint16_t ram_storage[RAM_BLOCK_SIZE * RAM_BLOCK_COUNT * 2];
static uint8_t lookahead_buffer[RAM_LOOKAHEAD_SIZE] __attribute__((aligned(4)));







static struct lfs_config cfg = {0};
static lfs_file_t file = {0};
static lfs_t lfs;
static int bInit = 0;
static int bMount = 0;
static int bFileIsOpen = 0;
// static GateMutexPri_Handle lfs_mutex;
// static IArg mutex_key;
static const char * tag = "lfs";

#if CONFIG_USE_SPI_FM25W256
#pragma DATA_SECTION(fm25w256Buffer, "DMARAM")
FM25W256_DECLARE_BUFFER(fm25w256Buffer, CONFIG_LFS_BLOCK_SZ);
#endif /* CONFIG_USE_SPI_FM25W256 */

static int fram_read(const struct lfs_config *c,
                    lfs_block_t block,
                    lfs_off_t off,
                    void *buffer,
                    lfs_size_t size)
{
    int ret = LFS_ERR_OK;
    uint16_t *block_start = &ram_storage[block * RAM_BLOCK_SIZE + off];
    uint16_t *dest = (uint16_t*)buffer;

    // 鎸夐�昏緫鍗曞厓锛�16-bit锛夎鍙�
    for (int i = 0; i < size; i++) {
        dest[i] = block_start[i];
    }
    return (ret);
}


static int fram_prog(const struct lfs_config *c,
                    lfs_block_t block,
                    lfs_off_t off,
                    const void *buffer,
                    lfs_size_t size)
{
    int ret = LFS_ERR_OK;
    uint16_t *block_start = &ram_storage[block * RAM_BLOCK_SIZE + off];
    const uint16_t *src = (const uint16_t*)buffer;

    // 鎸夐�昏緫鍗曞厓锛�16-bit锛夊啓鍏�
    for (int i = 0; i < size; i++) {
        // 鎷嗗垎 16-bit 鍊间负涓や釜 uint16_t 鍏冪礌
        block_start[i]   = src[i];
    }
    return (ret);
}


static int fram_erase(const struct lfs_config *c, lfs_block_t block)
{
    int ret = LFS_ERR_OK;
    uint16_t *block_start = &ram_storage[block * RAM_BLOCK_SIZE];

    // 姣忎釜閫昏緫鍗曞厓锛�16-bit锛夌敱涓や釜 uint16_t 鍏冪礌缁勬垚
    for (int i = 0; i < RAM_BLOCK_SIZE; i++) {
        // 楂樺瓧鑺傚拰浣庡瓧鑺傚潎缃负 0xFF
        block_start[i]   = 0xFFFF;
    }
    return (ret);
}


static int fram_sync(const struct lfs_config *c)
{
    return 0;
}

#ifdef LFS_THREADSAFE
static int lock(const struct lfs_config *c)
{
    mutex_key = GateMutexPri_enter(lfs_mutex);
    return 0;
}


static int unlock(const struct lfs_config *c)
{
    GateMutexPri_leave(lfs_mutex, mutex_key);
    return 0;
}
#endif


#define CONFIG_FRAM_LFS_CACHE_SIZE      RAM_CACHE_SIZE//(64)
#define CONFIG_FRAM_LFS_BLOCK_COUNT     RAM_BLOCK_COUNT//(CONFIG_SPI_FM25W256_SIZE / CONFIG_LFS_BLOCK_SZ)
#define CONFIG_FRAM_LFS_LOOKAHEAD_SIZE  (CONFIG_FRAM_LFS_BLOCK_COUNT / 8)

static uint8_t lfs_readBuffer[CONFIG_FRAM_LFS_CACHE_SIZE];
static uint8_t lfs_progBuffer[CONFIG_FRAM_LFS_CACHE_SIZE];
static uint8_t lfs_lookAheadBuffer[CONFIG_FRAM_LFS_LOOKAHEAD_SIZE];

static void lfs_c2000_init()
{
    if(bInit == 0) {
#ifdef LFS_THREADSAFE
        Error_Block eb;
        GateMutexPri_Params mutexParam;
        Error_init(&eb);
        GateMutexPri_Params_init(&mutexParam);
        mutexParam.instance->name = tag;
        lfs_mutex = GateMutexPri_create(&mutexParam, &eb);
        Assert_isTrue((Error_check(&eb) == FALSE) && (lfs_mutex != NULL), NULL);
#endif

        memset(&cfg, 0, sizeof(cfg));
        cfg.context = NULL;
        cfg.read = fram_read;
        cfg.prog = fram_prog;
        cfg.erase = fram_erase;
        cfg.sync = fram_sync;
#ifdef LFS_THREADSAFE
        cfg.lock = lock;
        cfg.unlock = unlock;
#endif
        cfg.read_size = 4;//CONFIG_LFS_READ_SZ;
        cfg.prog_size = 4;//CONFIG_LFS_PROG_SZ;
        cfg.block_size = RAM_BLOCK_SIZE;//CONFIG_LFS_BLOCK_SZ;
        cfg.block_count = CONFIG_FRAM_LFS_BLOCK_COUNT;
        cfg.block_cycles = 500;
        cfg.cache_size = CONFIG_FRAM_LFS_CACHE_SIZE;
        cfg.lookahead_size = CONFIG_FRAM_LFS_LOOKAHEAD_SIZE;

        cfg.read_buffer = lfs_readBuffer;
        cfg.prog_buffer = lfs_progBuffer;
        cfg.lookahead_buffer = lfs_lookAheadBuffer;
        bInit = 1;
    }
}

//////////////////////SPARE///////////////////////////
// int lfs_c2000_format()
// {
//     if(bInit == 0) {
//         lfs_c2000_init();
//     }
//     return (lfs_format(&lfs, &cfg));
// }


// lfs_t * lfs_c2000_mount()
// {
//     int lfsRet = LFS_ERR_OK;
//     lfs_t * ret = NULL;

//     if(bInit == 0) {
//         lfs_c2000_init();
//     }

//     lfsRet = lfs_mount(&lfs, &cfg);
//     if(lfsRet == LFS_ERR_OK) {
//         bMount = 1;
//         ret = &lfs;
//     }

//     return (ret);
// }


// int lfs_c2000_umount()
// {
//     int ret = LFS_ERR_OK;

//     if(bInit == 0) {
//         return -1;
//     }

//     if(bMount) {
//         ret = lfs_unmount(&lfs);
//         bMount = 0;
//     }

//     return (ret);
// }


// int lfs_c2000_ls(const char * path, char * outBuffer, size_t bufferLen)
// {
//     char tempStr[24];
//     if((bMount == 0) || (path == NULL) || (outBuffer == NULL)) {
//         return -1;
//     }

//     lfs_dir_t dir;
//     struct lfs_info info;
//     int err = lfs_dir_open(&lfs, &dir, path);
//     if(err) {
//         return err;
//     }

//     while(1) {
//         int res = lfs_dir_read(&lfs, &dir, &info);
//         if (res < 0) {
//             return res;
//         }

//         if(res == 0) {
//             break;
//         }

//         switch (info.type) {
//             case LFS_TYPE_REG: {
//                 outBuffer = strncat(outBuffer, "    ", bufferLen);
//                 break;
//             }
//             case LFS_TYPE_DIR: {
//                 outBuffer = strncat(outBuffer, "  d ", bufferLen);
//                 break;
//             }
//             default: {
//                 outBuffer = strncat(outBuffer, "  ? ", bufferLen);
//                 break;
//             }
//         }

//         System_snprintf(tempStr, sizeof(tempStr), "%10ld ", info.size);
//         strncat(outBuffer, tempStr, bufferLen);
//         System_snprintf(tempStr, sizeof(tempStr), "%s\r\n", info.name);
//         strncat(outBuffer, tempStr, bufferLen);
//     }

//     strncat(outBuffer, "\r\n", bufferLen);
//     err = lfs_dir_close(&lfs,  &dir);
//     if(err) {
//         return err;
//     }
//     return 0;
// }


// int lfs_c2000_mkdir(const char * path)
// {
//     if((bMount == 0) || (path == NULL)) {
//         return -1;
//     }

//     return(lfs_mkdir(&lfs, path));
// }


// lfs_file_t * lfs_c2000_fopen(const char * pathName)
// {
//     int lfsRet = 0;
//     lfs_file_t * ret = NULL;

//     if((bMount == 0) || (pathName == NULL) || (bFileIsOpen != 0)) {
//         return NULL;
//     }

//     lfsRet = lfs_file_open(&lfs, &file, pathName, LFS_O_CREAT | LFS_O_RDWR);
//     if(lfsRet == LFS_ERR_OK) {
//         bFileIsOpen = 1;
//         ret = &file;
//     } else {
//         bFileIsOpen = 0;
//     }

//     return(ret);
// }


// int lfs_c2000_fwrite(const char * strData, size_t len)
// {
//     if((bMount == 0) || (strData == NULL) || (bFileIsOpen != 1)) {
//         return -1;
//     }
//     return(lfs_file_write(&lfs, &file, strData, len));
// }


// int lfs_c2000_fread(char * outBuffer, size_t bufLen)
// {
//     if((bMount == 0) || (outBuffer == NULL) || (bFileIsOpen != 1)) {
//         return -1;
//     }
//     return(lfs_file_read(&lfs, &file, outBuffer, bufLen));
// }


// int lfs_c2000_mv(const char * source, const char * target)
// {
//     if((bMount == 0) || (source == NULL) || (target == NULL)) {
//         return -1;
//     }

//     return(lfs_rename(&lfs, source, target));
// }


// int lfs_c2000_rm(const char * path)
// {
//     if((bMount == 0) || (path == NULL)) {
//         return -1;
//     }

//     return(lfs_remove(&lfs, path));
// }


// int lfs_c2000_fclose()
// {
//     int ret = 0;
//     if((bMount == 0) || (bFileIsOpen != 1)) {
//         return -1;
//     }

//     ret = lfs_file_close(&lfs, &file);
//     memset(&file, 0, sizeof(file));
//     bFileIsOpen = 0;
//     return ret;
// }
//////////////////////SPARE///////////////////////////

// Software CRC implementation with small lookup table
uint32_t lfs_crc(uint32_t crc, const void *buffer, size_t size) {
    static const uint32_t rtable[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c,
    };
    const uint8_t *data = buffer;

    for (size_t i = 0; i < size; i++) {
        crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 0)) & 0xf];
        crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 4)) & 0xf];
    }
    return crc;
}
