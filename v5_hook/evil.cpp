// https://guidedhacking.com/threads/c-detour-hooking-function-tutorial.7930/
// Import libraries
//
#define UNICODE
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <MinHook.h>


//#if _WIN64 
//#pragma comment(lib, "libMinHook.x64.lib")
//#else
#pragma comment(lib, "libMinHook.x86.lib")
//#endif


// Set namespaces
//
using namespace std;

// typedefs
//
typedef int (__thiscall* TinternalSend) (uintptr_t*, uint32_t, BYTE*);
typedef int (__thiscall* TinternalSendAfter) (uintptr_t*);

// global variables, classes, pointers
//
uintptr_t* CPythonNetworkStream = reinterpret_cast<uintptr_t*>(0x0A287968);
BYTE* pntBufferCrypt = (BYTE*) 0x080042A8;
BYTE* pntSendCounter = (BYTE*) 0x09C20E70;
int hooked = 0;
TinternalSend internalSend = (TinternalSend) 0x012F7B40;
TinternalSendAfter internalSendAfter = (TinternalSendAfter) 0x012F7BB0;

// allocate a new console window
//
void startConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    cout << "Hack started. v1" << endl;
}

// stop the allocatet console window
//
void stopConsole() {
    cout << "Hack stopped." << endl;
    FreeConsole();
}

// send own data packets to game
//
void Send(uint32_t length, BYTE* ptrSendBuffer) {
    internalSend(CPythonNetworkStream, length, ptrSendBuffer);
    internalSendAfter(CPythonNetworkStream);
}

// returns a number+1 at every send that was done
//
int getSendCounter() {
    return (int)pntSendCounter[0];
}

// read the send buffer
//
void trackSend() {
    BYTE buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int sendCounter = getSendCounter();
    int n = 0;
    while(n<10) {
        int bufCounter = getSendCounter();
        if(sendCounter != bufCounter) {
            memcpy (&buf[0], pntBufferCrypt, 21);
            for(int i=0; i<21; i++) {
                cout <<  (((int)buf[i]<16)?"0":"") << uppercase << hex << (int)buf[i];
            }
            cout << endl;
            sendCounter = bufCounter;
            n=n+1;
        }
    }
}

// Detour function which overrides MessageBoxW.
int WINAPI DetourInternalSend(uintptr_t* classpointer, uint32_t length_packet, BYTE* buffer_packet)
{
    return internalSend(classpointer, length_packet, buffer_packet);
}

// process main function
//
void Main(HINSTANCE hInst) {
    startConsole();
    
    // Initialize MinHook.
    if (MH_Initialize() != MH_OK) {
        cout << "error init minhook" << endl;
    }

    // Create a hook for MessageBoxW, in disabled state.
    if (MH_CreateHook(&internalSend, reinterpret_cast<LPVOID*>(&DetourInternalSend), reinterpret_cast<LPVOID*>(&internalSend)) != MH_OK) {
        cout << "error create hook" << endl;
    }

    // Enable the hook for MessageBoxW.
    if (MH_EnableHook(&internalSend) != MH_OK) {
        cout << "error enable hook" << endl;
    }

    while (true) {
        Sleep(1000);
        if (GetAsyncKeyState(VK_ESCAPE) & 1) {
            break;
        }
        //BYTE SendBuffer[] = {0x4F, 0x01, 0x00, 0x00};
        //Send(sizeof(SendBuffer), &SendBuffer[0]);
        cout << hooked << endl;
    }
    
    // Disable the hook for MessageBoxW.
    if (MH_DisableHook(&internalSend) != MH_OK) {
        cout << "error disable hook" << endl;
    }
    // Uninitialize MinHook.
    if (MH_Uninitialize() != MH_OK) {
        cout << "error un init minhook" << endl;
    }

    stopConsole();
    FreeLibraryAndExitThread(hInst, 0);
}

// DLL main function
//
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