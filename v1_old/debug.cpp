#define UNICODE
#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>

using namespace std;

int main() {
    // not working cause console application
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    cout << "This works" << endl;
    FreeConsole();
    uint32_t slot = 16;
    cout << hex << slot << endl;
    uint32_t inventory = 0x00000001;
    cout << hex << inventory << endl;
    uint32_t arg1 = (slot<<8) + inventory;
    cout << hex << arg1 << endl;


    return 0;
}