@echo off
cls

echo [101;93m "Build debug.cpp" [0m
g++ debug.cpp -o debug

echo [101;93m "Run debug.exe" [0m
debug.exe