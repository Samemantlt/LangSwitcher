@echo off
set CMAKE="C:\Program Files\CMake\bin\cmake.exe"
%CMAKE% -S . -B build -G "MinGW Makefiles"
%CMAKE% --build build
