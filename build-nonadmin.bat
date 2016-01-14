echo off
cls
set path=C:\mingw\bin;%path%
windres res.rc -O coff -o res.res
gcc -Wall main.c res.res -o main.exe -imacros default.h -fomit-frame-pointer -funswitch-loops -O3 -Os -mwindows -DUNICODE -lShlwapi -lShell32
strip main.exe
