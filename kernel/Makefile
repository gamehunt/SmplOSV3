ARCH = x86_64
TARGET = smplos.elf
SRCS = $(wildcard src/*.c)
ASM_SRCS = $(wildcard src/*.asm)
OBJS = $(SRCS:.c=.o) $(ASM_SRCS:.asm=.o)

CFLAGS = -ffreestanding -pedantic -Wall -Wextra -Werror --std=c11 -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Iinclude -I../shared -I../build/include -D_SMPLOS_KERNEL
LINK_OPTS = -ffreestanding -T link.ld -nostdlib -lgcc -z max-page-size=0x1000

CC = $(ARCH)-elf-gcc
LD = $(ARCH)-elf-ld
ASM = nasm

.PHONY: clean

%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o : %.asm
	$(ASM) -felf64 $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LINK_OPTS) $(OBJS) -o $@ 

all: $(TARGET)

clean:
	rm $(OBJS)
	rm $(TARGET)

install:
	cp $(TARGET) ../build