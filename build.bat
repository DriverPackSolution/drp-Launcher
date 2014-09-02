echo off
cls
set path=C:\mingw\bin;%path%
windres res.rc -O coff -o res.res
gcc -Wall main.c res.res -o main.exe -fomit-frame-pointer -funswitch-loops -O3 -Os -mwindows
strip main.exe
