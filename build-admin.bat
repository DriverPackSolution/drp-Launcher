echo off
cls
set path=C:\mingw\bin;%path%
windres resa.rc -O coff -o resa.res
gcc -Wall main.c resa.res -o main-admin.exe -imacros default.h -fomit-frame-pointer -funswitch-loops -O3 -Os -mwindows -DUNICODE -lShlwapi -lShell32
strip main-admin.exe
