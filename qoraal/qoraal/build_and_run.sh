mkdir build
cd build
cmake .. -DBUILD_TESTS=ON 
cmake --build .
cd ..
./build/test/qoraal_test

