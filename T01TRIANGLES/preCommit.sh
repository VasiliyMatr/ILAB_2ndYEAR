
./format.sh

cd build
cmake ..
make clean
make -j12
ctest -j12
