mkdir build
cd build
cmake .. -DCFG_ENGINE_REGISTRY_ENABLE=ON -DBUILD_TOASTER=ON -G "MinGW Makefiles" 
cmake --build .
cd ..
.\build\test\toaster .\test\toaster.e
