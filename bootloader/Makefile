ARCH = x86_64
TARGET = BOOTX64.EFI

SRCS = $(wildcard src/*.c)

CFLAGS = -pedantic -Wall -Wextra --std=c11 -O2 -Iinclude 
LDFLAGS =

USE_GCC = 1
include uefi/Makefile

install:
	cp BOOTX64.EFI ../build
