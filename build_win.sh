/mingw64/bin/gcc `pkg-config --cflags gtk+-3.0` -o debug/win64/JustEditor.exe sources/JustEditor.c `pkg-config --libs gtk+-3.0`
debug/win64/JustEditor.exe $1
