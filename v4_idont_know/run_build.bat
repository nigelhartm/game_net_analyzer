@echo off
cls
echo [101;93m Build evil.dll [0m
g++ -c evil.cpp
g++ -shared -o C://evil.dll evil.o

echo [101;93m "Build main.cpp" [0m
g++ main.cpp -o main

echo [101;93m "Run main.exe" [0m
main.exe