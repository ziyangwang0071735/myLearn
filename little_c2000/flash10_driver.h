// flash10_driver.h
#ifndef _FLASH10_DRIVER_H_
#define _FLASH10_DRIVER_H_

#include <stdint.h>

void flash10_init(void);
int flash10_read(uint32_t addr, void *buffer, uint32_t size);
int flash10_prog(uint32_t addr, const void *data, uint32_t size);
int flash10_erase(uint32_t addr);

#endif
