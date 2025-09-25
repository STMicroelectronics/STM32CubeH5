mkdir build
cd build
cmake .. -DBUILD_HTTPTEST=ON -G "MinGW Makefiles" 
cmake --build .
cd ..
.\build\test\httptest
