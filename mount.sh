sudo losetup /dev/loop0 disk.img
sudo losetup /dev/loop1 disk.img -o 1048576

sudo mount /dev/loop1 /mnt/smplos
