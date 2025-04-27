################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../2837xD_RAM_CLA_lnk_cpu1.cmd 

SYSCFG_SRCS += \
../lab_f2837xd_launchpad.syscfg 

CLA_SRCS += \
../lab_cla_tasks.cla 

LIB_SRCS += \
C:/ti/C2000Ware_5_02_00_00/driverlib/f2837xd/driverlib/ccs/Debug/driverlib.lib 

C_SRCS += \
../Hal_Eep.c \
../i2cLib_FIFO_polling.c \
../i2c_ex4_eeprom_polling.c \
./syscfg/board.c \
./syscfg/c2000ware_libraries.c 

GEN_FILES += \
./syscfg/board.c \
./syscfg/board.opt \
./syscfg/c2000ware_libraries.opt \
./syscfg/c2000ware_libraries.c 

CLA_DEPS += \
./lab_cla_tasks.d 

GEN_MISC_DIRS += \
./syscfg 

C_DEPS += \
./Hal_Eep.d \
./i2cLib_FIFO_polling.d \
./i2c_ex4_eeprom_polling.d \
./syscfg/board.d \
./syscfg/c2000ware_libraries.d 

GEN_OPTS += \
./syscfg/board.opt \
./syscfg/c2000ware_libraries.opt 

OBJS += \
./Hal_Eep.obj \
./i2cLib_FIFO_polling.obj \
./i2c_ex4_eeprom_polling.obj \
./lab_cla_tasks.obj \
./syscfg/board.obj \
./syscfg/c2000ware_libraries.obj 

GEN_MISC_FILES += \
./syscfg/board.h \
./syscfg/board.cmd.genlibs \
./syscfg/board.json \
./syscfg/pinmux.csv \
./syscfg/c2000ware_libraries.cmd.genlibs \
./syscfg/c2000ware_libraries.h \
./syscfg/clocktree.h 

GEN_MISC_DIRS__QUOTED += \
"syscfg" 

OBJS__QUOTED += \
"Hal_Eep.obj" \
"i2cLib_FIFO_polling.obj" \
"i2c_ex4_eeprom_polling.obj" \
"lab_cla_tasks.obj" \
"syscfg\board.obj" \
"syscfg\c2000ware_libraries.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\board.h" \
"syscfg\board.cmd.genlibs" \
"syscfg\board.json" \
"syscfg\pinmux.csv" \
"syscfg\c2000ware_libraries.cmd.genlibs" \
"syscfg\c2000ware_libraries.h" \
"syscfg\clocktree.h" 

C_DEPS__QUOTED += \
"Hal_Eep.d" \
"i2cLib_FIFO_polling.d" \
"i2c_ex4_eeprom_polling.d" \
"syscfg\board.d" \
"syscfg\c2000ware_libraries.d" 

GEN_FILES__QUOTED += \
"syscfg\board.c" \
"syscfg\board.opt" \
"syscfg\c2000ware_libraries.opt" \
"syscfg\c2000ware_libraries.c" 

CLA_DEPS__QUOTED += \
"lab_cla_tasks.d" 

C_SRCS__QUOTED += \
"../Hal_Eep.c" \
"../i2cLib_FIFO_polling.c" \
"../i2c_ex4_eeprom_polling.c" \
"./syscfg/board.c" \
"./syscfg/c2000ware_libraries.c" 

SYSCFG_SRCS__QUOTED += \
"../lab_f2837xd_launchpad.syscfg" 


