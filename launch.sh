./build.sh && sync && qemu-system-x86_64 -monitor stdio -drive format=raw,file=disk.img -m 1G -d cpu_reset
