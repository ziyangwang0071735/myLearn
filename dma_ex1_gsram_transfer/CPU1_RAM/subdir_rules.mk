################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu64 --tmu_support=tmu1 --vcu_support=vcrc -Ooff --include_path="C:/Users/wzy00/workspace_v12/dma_ex1_gsram_transfer" --include_path="C:/ti/C2000Ware_5_02_00_00" --include_path="C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=CPU1 --define=RAM --define=generic_ram_lnk --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/wzy00/workspace_v12/dma_ex1_gsram_transfer/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-2041147617: ../dma_ex1_gsram_transfer.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1271/ccs/utils/sysconfig_1.20.0/sysconfig_cli.bat" --script "C:/Users/wzy00/workspace_v12/dma_ex1_gsram_transfer/dma_ex1_gsram_transfer.syscfg" -o "syscfg" -s "C:/ti/C2000Ware_5_02_00_00/.metadata/sdk.json" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-2041147617 ../dma_ex1_gsram_transfer.syscfg
syscfg/board.h: build-2041147617
syscfg/board.cmd.genlibs: build-2041147617
syscfg/board.opt: build-2041147617
syscfg/board.json: build-2041147617
syscfg/pinmux.csv: build-2041147617
syscfg/device.c: build-2041147617
syscfg/device.h: build-2041147617
syscfg/device_cmd.cmd: build-2041147617
syscfg/device_cmd.c: build-2041147617
syscfg/device_cmd.h: build-2041147617
syscfg/device_cmd.opt: build-2041147617
syscfg/device_cmd.cmd.genlibs: build-2041147617
syscfg/c2000ware_libraries.cmd.genlibs: build-2041147617
syscfg/c2000ware_libraries.opt: build-2041147617
syscfg/c2000ware_libraries.c: build-2041147617
syscfg/c2000ware_libraries.h: build-2041147617
syscfg/clocktree.h: build-2041147617
syscfg: build-2041147617

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu64 --tmu_support=tmu1 --vcu_support=vcrc -Ooff --include_path="C:/Users/wzy00/workspace_v12/dma_ex1_gsram_transfer" --include_path="C:/ti/C2000Ware_5_02_00_00" --include_path="C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=CPU1 --define=RAM --define=generic_ram_lnk --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/wzy00/workspace_v12/dma_ex1_gsram_transfer/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

f28p65x_codestartbranch.obj: C:/ti/C2000Ware_5_02_00_00/device_support/f28p65x/common/source/f28p65x_codestartbranch.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu64 --tmu_support=tmu1 --vcu_support=vcrc -Ooff --include_path="C:/Users/wzy00/workspace_v12/dma_ex1_gsram_transfer" --include_path="C:/ti/C2000Ware_5_02_00_00" --include_path="C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=CPU1 --define=RAM --define=generic_ram_lnk --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/wzy00/workspace_v12/dma_ex1_gsram_transfer/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


