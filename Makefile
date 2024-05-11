#--------------------------------------Makefile-------------------------------------

GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib
DIR_BUI = build
DIR_KER = kernel
DIR_ROOT = $(shell pwd)
CFILES = $(wildcard $(DIR_KER)/*.c)
OFILES = $(CFILES:$(DIR_KER)/%.c=$(DIR_BUI)/%.o)

all: clean $(DIR_BUI)/kernel8.img run

$(DIR_BUI)/boot.o: $(DIR_KER)/boot.S
	aarch64-none-elf-gcc -I$(DIR_ROOT) $(GCCFLAGS) -c $< -o $@
	
$(DIR_BUI)/%.o: $(DIR_KER)/%.c
	aarch64-none-elf-gcc -I$(DIR_ROOT) $(GCCFLAGS) -c $< -o $@

$(DIR_BUI)/kernel8.img: $(DIR_BUI)/boot.o $(OFILES)
	aarch64-none-elf-ld -nostdlib $^ -T $(DIR_KER)/link.ld -o $(DIR_BUI)/kernel8.elf
	aarch64-none-elf-objcopy -O binary $(DIR_BUI)/kernel8.elf $@

clean:
	del  .\$(DIR_BUI)\*

# Run emulation with QEMU
run: 
	qemu-system-aarch64 -M raspi3 -kernel $(DIR_BUI)/kernel8.img -serial stdio
