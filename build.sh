sudo rm -rf build   || 1
sudo rm -rf sysroot || 1
sudo cmake . -Bbuild && sync
