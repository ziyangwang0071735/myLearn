//文件管理核心接口（注释原FLASh调用）//////////begin
void FileMan_InitDirectory(void)
{
    memset(g_directory, 0, sizeof(g_directory));

    // 原始擦除Flash目录区：
    // Flash_Erase(FILEMAN_DIRECTORY_START_ADDRESS, 2);

    // 替换为：
    Flash_Erase(FILEMAN_DIRECTORY_START_ADDRESS, 2);  // 1KB
}

int FileMan_FindEmptySlot(void)
{
    for (int i = 0; i < MAX_FILE_NUM; i++)
        if (g_directory[i].used == 0)
            return i;
    return -1;
}

uint32_t DummyCRC32(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0;
    for (uint32_t i = 0; i < len; i++)
        crc += data[i];
    return crc;
}

int FileMan_WriteFile(const char *name, const uint8_t *data, uint32_t len)
{
    int index = FileMan_FindEmptySlot();
    if (index < 0) return 0;

    uint32_t startAddr = FILEMAN_FILE_START_ADDRESS;
    for (int i = 0; i < index; i++)
        if (g_directory[i].used)
            startAddr += g_directory[i].file_size;

    if ((startAddr + len) > FILEMAN_OCCUPY_END_ADDRESS)
        return 0;

    // 原始写入Flash：
    // Flash_WritePage(startAddr, (uint8_t*)data, len);

    // 替换为：
    if (!Flash_PageWrite(startAddr, (uint8_t*)data, len))
        return 0;

    strcpy(g_directory[index].file_name, name);
    g_directory[index].file_size = len;
    g_directory[index].file_start_addr = startAddr;
    g_directory[index].file_crc32 = DummyCRC32(data, len);
    g_directory[index].used = 1;
    return 1;
}

int FileMan_ReadFile(const char *name, uint8_t *buffer, uint32_t *out_len)
{
    for (int i = 0; i < MAX_FILE_NUM; i++)
    {
        if (g_directory[i].used && strcmp(g_directory[i].file_name, name) == 0)
        {
            // 原始从Flash读取数据：
            // Flash_Read(g_directory[i].file_start_addr, buffer, g_directory[i].file_size);

            // 替换为：
            Flash_ReadData(g_directory[i].file_start_addr,
                           g_directory[i].file_size, buffer);
            *out_len = g_directory[i].file_size;
            return 1;
        }
    }
    return 0;
}
//文件管理核心接口（注释原FLASh调用）//////////end

//精简目录结构和数据结构///////////////////begin
typedef struct {
    uint32_t file_start_addr;
    uint32_t file_size;
    uint32_t file_crc32;
    char     file_name[16];
    uint8_t  used;
} FILEMAN_FILEINFO_t;

static FILEMAN_FILEINFO_t g_directory[MAX_FILE_NUM];
////////////////////////////////

//模拟Flash参数与替换代码///////////
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// === 模拟 Flash 参数 ===
#define FILEMAN_FLASH_SECTOR_SIZE        512
#define FILEMAN_SECTOR_TOTAL             16
#define SIMULATED_FLASH_SIZE             (FILEMAN_FLASH_SECTOR_SIZE * FILEMAN_SECTOR_TOTAL) // 8192 bytes

#define FILEMAN_DIRECTORY_START_ADDRESS  0
#define FILEMAN_DIRECTORY_END_ADDRESS    1023
#define FILEMAN_FILE_START_ADDRESS       1024
#define FILEMAN_OCCUPY_END_ADDRESS       (SIMULATED_FLASH_SIZE - 1)

#define MAX_FILE_NUM                     16

// === 模拟 Flash 区域 ===
static uint8_t FakeFlash[SIMULATED_FLASH_SIZE];

void Flash_ReadData(uint32_t address, uint16_t size, uint8_t *buffer)
{
    // 原始代码：
    // 实际从芯片 Flash 中读取数据
    // Flash_Read(address, buffer, size);

    // 替换为：
    memcpy(buffer, &FakeFlash[address], size);
}

int Flash_PageWrite(uint32_t address, uint8_t *data, uint16_t size)
{
    // 原始代码：
    // return Flash_WritePage(address, data, size);

    // 替换为：
    if ((address + size) > SIMULATED_FLASH_SIZE) return 0;
    for (uint16_t i = 0; i < size; i++)
        FakeFlash[address + i] &= data[i];  // 模拟 Flash 写只能1变0
    return 1;
}

void Flash_Erase(uint32_t address, uint16_t numPages)
{
    // 原始代码：
    // Flash_ErasePages(address, numPages);

    // 替换为：
    uint32_t totalSize = numPages * FILEMAN_FLASH_SECTOR_SIZE;
    if ((address + totalSize) > SIMULATED_FLASH_SIZE) return;
    memset(&FakeFlash[address], 0xFF, totalSize);
}
////////////////end///////////////////////////////////
