.SILENT:
all: build run

build:
	echo Building ...
	gcc -g -Iinclude -Llib ./src/*.c -o main.exe -lglfw3 -lglad -lstb -luser32 -lkernel32 -lgdi32 -lwinmm
	echo Done!

run: build
	cls
	./main
