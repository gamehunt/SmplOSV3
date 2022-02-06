ARCH = x86_64
TARGET = BOOTX64.EFI

C_SRCS = $(wildcard src/*.c)
ASM_SRCS = $(wildcard src/*.S)

SRCS = $(C_SRCS) $(ASM_SRCS)

CLEAN_OBJS = $(SRCS:.c=.o) $(ASM_SRCS:.S=.o)

CFLAGS = -pedantic -Wall -Wextra -Werror --std=c11 -O2 -Iinclude -I../shared -D_SMPLOS_BOOTLOADER
LDFLAGS =

.PHONY: clean

USE_GCC = 1
include uefi/Makefile

clean:
	$(MAKE) -f uefi/Makefile $@
	rm $(CLEAN_OBJS) || :

install:
	cp $(TARGET) ../build
	cp -r include ../build/include/boot
