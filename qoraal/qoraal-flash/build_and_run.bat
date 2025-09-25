mkdir build
cd build
cmake .. -DBUILD_REGTEST=ON -G "MinGW Makefiles" 
cmake --build .
cd ..
.\build\test\flashtest
