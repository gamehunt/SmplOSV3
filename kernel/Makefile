ARCH = x86_64
TARGET = smplos.elf
SRCS = $(wildcard src/*.c src/**/*.c)
ASM_SRCS = $(wildcard src/*.asm)
OBJS = $(SRCS:.c=.o) $(ASM_SRCS:.asm=.o)

CFLAGS = -ffreestanding -pedantic -Wall -Wextra -Werror --std=c11 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Iinclude -I../shared -I../build/include -D_SMPLOS_KERNEL
LINK_OPTS = -ffreestanding -T link.ld  -L../build -nostdlib -lgcc -lk -z max-page-size=0x1000

CC = $(ARCH)-elf-gcc
LD = $(ARCH)-elf-ld
ASM = nasm

.PHONY: clean

%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o : %.asm
	$(ASM) -felf64 $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LINK_OPTS) -o $@ 

all: $(TARGET)

clean:
	rm $(OBJS) 2>/dev/null || :
	rm $(TARGET) 2>/dev/null || :

install:
	cp $(TARGET) ../build