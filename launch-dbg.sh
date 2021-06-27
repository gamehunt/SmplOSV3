./build.sh && sync && qemu-system-x86_64 -d int -monitor stdio -drive format=raw,file=disk.img -m 1G
