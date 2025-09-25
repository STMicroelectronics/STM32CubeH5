mkdir build
cd build
cmake .. -DBUILD_TESTS=ON  -G "MinGW Makefiles"
cmake --build .
cd ..
.\build\test\qoraal_test
