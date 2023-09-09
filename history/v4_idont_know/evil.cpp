// Import libraries
//
#define UNICODE
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>

// Set namespaces
//
using namespace std;

// typedefs
//
typedef int (__thiscall* TinternalSend) (uintptr_t*, uint32_t, BYTE*);
typedef int (__thiscall* TinternalSendAfter) (uintptr_t*);

// global variables, classes, pointers
//
uintptr_t* CPythonNetworkStream = reinterpret_cast<uintptr_t*>(0x09C20DE0);
BYTE* pntBufferCrypt = (BYTE*) 0x080042A8;
BYTE* pntSendCounter = (BYTE*) 0x09C20E70;

// allocate a new console window
//
void startConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    cout << "Hack" << endl;
}

// stop the allocatet console window
//
void stopConsole() {
    FreeConsole();
}

// send own data packets to game
//
void Send(uint32_t length, BYTE* ptrSendBuffer) {
    TinternalSend internalSend = (TinternalSend) 0x01057B40;
    internalSend(CPythonNetworkStream, length, ptrSendBuffer);
    TinternalSendAfter internalSendAfter = (TinternalSendAfter) 0x01057BB0;
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

// process main function
//
void Main(HINSTANCE hInst) {
    startConsole();
    //BYTE SendBuffer[] = {0x4F, 0x01, 0x06, 0x00};
    //Send(sizeof(SendBuffer), &SendBuffer[0]);
    trackSend();
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