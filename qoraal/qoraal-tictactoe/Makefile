# Detect OS and set platform-specific variables
ifeq ($(OS),Windows_NT)
    MKDIR = if not exist build mkdir build
    EXECUTABLE = build\src\tictactoe.exe
    RM = rmdir /S /Q
else
    MKDIR = mkdir -p build
    EXECUTABLE = ./build/src/tictactoe
    RM = rm -rf
endif

.PHONY: all build run clean

all: build run

build:
	$(MKDIR)
	cd build && cmake .. && cmake --build .

run:
	cd $(CURDIR) && $(EXECUTABLE)

clean:
	$(RM) build
