SHELL = /bin/bash

LOOP_DEVICE := /dev/loop1
MOUNTPOINT := /mnt/smplos

.PHONY: all build bootloader kernel clean

all: build install launch

build: clean prepare bootloader kernel

prepare: clean
	mkdir build
	mkdir build/include
	test -r /usr/share/ovmf/OVMF.fd && cp /usr/share/ovmf/OVMF.fd build/
	sudo mkdir /mnt/smplos || :
	cd bootloader && (test -h uefi || ln -s ../posix-uefi/uefi)

bootloader: prepare
	make -C bootloader
	make -C bootloader install

bootloader-clean:
	make -C bootloader clean || :

kernel: bootloader
	make -C kernel
	make -C kernel install

kernel-clean:
	make -C kernel clean || :

clean: bootloader-clean kernel-clean
	rm -rf bootloader/uefi
	rm -rf build

create-disk:
	dd if=/dev/zero of=disk.img bs=1048576 count=128
	fdisk disk.img <<< $$'g\nn p\n1\n2048\n+32M\nt 1\nw\n'
	sudo losetup $(LOOP_DEVICE) disk.img
	sudo mkfs.vfat -F 16 -n "EFI System" $(LOOP_DEVICE)
	sudo losetup -d $(LOOP_DEVICE)

disk-check-fail:
	$(error Create disk first!)

ovmf-check-fail:
	$(error You need to install ovmf for launching OS)

check-ovmf:
	test -r build/OVMF.fd || make ovmf-check-fail

check-disk:
	test -r disk.img || make disk-check-fail 

mount-disk: check-disk umount-disk
	sudo losetup $(LOOP_DEVICE) disk.img
	sudo mount $(LOOP_DEVICE) /mnt/smplos 

umount-disk:
	sudo umount /mnt/smplos || :
	sudo losetup -d $(LOOP_DEVICE) || :

remove-disk: umount-disk
	rm -rf disk.img

launch: check-ovmf install
	qemu-system-x86_64 -bios build/OVMF.fd -drive file=disk.img,format=raw -serial stdio

install: mount-disk
	sudo mkdir -p /mnt/smplos/EFI/BOOT/
	sudo mkdir -p /mnt/smplos/smplos/
	sudo cp build/BOOTX64.EFI /mnt/smplos/EFI/BOOT/
	sudo cp build/smplos.elf  /mnt/smplos/smplos/smplos.elf
	sudo cp boot.cfg /mnt/smplos/smplos/
	sync