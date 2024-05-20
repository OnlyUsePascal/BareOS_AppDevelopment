cmake_minimum_required(VERSION 3.27)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(asm2)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER aarch64-none-elf-gcc)
set(CMAKE_LINKER aarch64-none-elf-ld)
set(CMAKE_OBJCOPY aarch64-none-elf-objcopy)

set(FLAGS -Wall -O2 -ffreestanding -nostdinc -nostdlib)

set(SRC_DIR kernel)
set(INCLUDE_DIR lib)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/link.ld)
set(OBJ_LIB obj_lib)
set(TARGET kernel.elf)
set(TARGET_DEBUG kernel)
set(TARGET_IMG kernel8.img)

enable_language(ASM)

set(SRC_FILES
        ${SRC_DIR}/boot.S
        ${SRC_DIR}/def.c
        ${SRC_DIR}/font.c
        ${SRC_DIR}/maze.c
        ${SRC_DIR}/framebf.c
        ${SRC_DIR}/image.c
        ${SRC_DIR}/game_be.c
        ${SRC_DIR}/game_fe.c
        ${SRC_DIR}/helper.c
        ${SRC_DIR}/kernel.c
        ${SRC_DIR}/link.ld
        ${SRC_DIR}/mbox.c
        ${SRC_DIR}/uart0.c
        ${SRC_DIR}/util_str.c
        ${SRC_DIR}/utils.c
)

add_executable(${TARGET} ${SRC_FILES})

target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIR} .)
target_compile_options(${TARGET} PRIVATE ${FLAGS})
target_link_options(${TARGET} PRIVATE -nostdlib -T ${LINKER_SCRIPT})

add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET} ${TARGET_IMG}
        COMMENT "Copying into ${TARGET_IMG}"
)

set(RASPI raspi3b)

add_custom_target(
        uart0
        COMMAND qemu-system-aarch64 -M ${RASPI} -kernel ${TARGET_IMG} -serial stdio
        DEPENDS ${TARGET_IMG}
        COMMENT "Run UART0 configuration"
)

