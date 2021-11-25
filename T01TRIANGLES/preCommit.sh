
./format.sh

cd build
cmake ..
make clean
make -j8
ctest -j8
