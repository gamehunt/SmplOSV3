sudo rm -rf build   || 1
sudo rm -rf include || 1
sudo rm -rf lib     || 1
sudo cmake . -Bbuild && sync
