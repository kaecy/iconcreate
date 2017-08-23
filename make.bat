@echo off

rc iconcreate.rc
cl iconcreate.cpp iconcreate.res

mkdir out 2>nul
move *.exe out > nul
erase *.obj > nul
erase *.res > nul
