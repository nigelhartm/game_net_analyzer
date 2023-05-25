#define UNICODE
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>

using namespace std;

typedef int (__thiscall* TinternalSend) (uintptr_t*, uint32_t, BYTE*);
typedef int (__thiscall* TinternalSendAfter) (uintptr_t*);

uintptr_t* CPythonNetworkStream = reinterpret_cast<uintptr_t*>(0x0A805970);

void startConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    cout << "Hack" << endl;
}

void stopConsole() {
    FreeConsole();
}

void Send(uint32_t length, BYTE* ptrSendBuffer) {
    TinternalSend internalSend = (TinternalSend) 0x01197B40;
    internalSend(CPythonNetworkStream, length, ptrSendBuffer);

    TinternalSendAfter internalSendAfter = (TinternalSendAfter) 0x01197BB0;
    internalSendAfter(CPythonNetworkStream);
}

void Main(HINSTANCE hInst) {
    startConsole();
    BYTE SendBuffer[] = {0x4F, 0x01, 0x06, 0x00};
    Send(sizeof(SendBuffer), &SendBuffer[0]);
    stopConsole();
    FreeLibraryAndExitThread(hInst, 0);
}

BOOL APIENTRY DllMain (HINSTANCE hInst, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Main, hInst, NULL, NULL);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}