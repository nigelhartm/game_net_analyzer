@echo off
cls
echo [101;93m Build evil.dll [0m
g++ -c evil.cpp -m32
g++ -shared -o C://evil.dll evil.o

echo [101;93m "Build main.cpp" [0m
g++ -c main.cpp -m32
g++ -o main.exe main.o

echo [101;93m "Run main.exe" [0m
main.exe
