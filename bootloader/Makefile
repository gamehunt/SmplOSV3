ARCH = x86_64
TARGET = BOOTX64.EFI
SRCS = $(wildcard src/*.c)

CFLAGS = -pedantic -Wall -Wextra -Werror --std=c11 -O2 -Iinclude -I../shared -D_SMPLOS_BOOTLOADER
LDFLAGS =

USE_GCC = 1
include uefi/Makefile

install:
	cp $(TARGET) ../build
	cp -r include ../build/include/boot
