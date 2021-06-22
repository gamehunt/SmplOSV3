sudo rm -rf build || 1
cmake . -Bbuild && cd build && sudo make && sync
