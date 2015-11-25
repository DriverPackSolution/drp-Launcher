echo off
cls
set path=C:\mingw\bin;%path%
windres res.rc -O coff -o res.res
gcc -Wall main-configurator.c res.res -o main-configurator.exe -fomit-frame-pointer -funswitch-loops -O3 -Os -mwindows -DUNICODE -lShlwapi -lShell32
strip main-configurator.exe
