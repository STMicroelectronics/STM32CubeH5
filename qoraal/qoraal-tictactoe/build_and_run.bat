mkdir build
cd build
cmake .. -G "MinGW Makefiles" 
cmake --build .
REM we want to be in the same directory as tictactoe.e
cd ..
.\build\src\tictactoe
