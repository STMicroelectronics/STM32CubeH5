mkdir build
cd build
cmake .. -DBUILD_REGTEST=ON 
cmake --build .
cd ..
./build/test/flashtest

