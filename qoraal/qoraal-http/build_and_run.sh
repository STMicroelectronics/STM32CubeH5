mkdir build
cd build
cmake .. -DBUILD_HTTPTEST=ON 
cmake --build .
cd ..
./build/test/httptest
