rm -rf build || 1
cmake . -Bbuild && cd build && make && sudo make install && sync
