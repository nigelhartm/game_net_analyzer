// https://guidedhacking.com/threads/c-detour-hooking-function-tutorial.7930/
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
uintptr_t* CPythonNetworkStream = reinterpret_cast<uintptr_t*>(0x0A287968);
BYTE* pntBufferCrypt = (BYTE*) 0x080042A8;
BYTE* pntSendCounter = (BYTE*) 0x09C20E70;

int hooked = 0;
int hooklength = 6;
uintptr_t* hookAddress = reinterpret_cast<uintptr_t*>(0x00C17B43); // die ersten 2 mov
uintptr_t* jmpBackAddy = reinterpret_cast<uintptr_t*>(reinterpret_cast<uint32_t> (hookAddress) + hooklength);

TinternalSend internalSend = (TinternalSend) 0x012F7B40;
TinternalSendAfter internalSendAfter = (TinternalSendAfter) 0x012F7BB0;

// allocate a new console window
//
void startConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    cout << "Hack started. v2" << endl;
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

// https://guidedhacking.com/threads/how-to-hook-functions-code-detouring-guide.14185/
bool detour_send(uintptr_t* src, uintptr_t* dst, int len) {
    if (len < 5) return false;
    DWORD curProtection;
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);
    memset(src, 0x90, len);
    uintptr_t relativeAddress = ((uintptr_t)dst - (uintptr_t)src) - 5;
    *(BYTE*)src = 0xE9;
    *(uintptr_t*)((uintptr_t)src + 1) = relativeAddress;
    DWORD temp;
    VirtualProtect(src, len, curProtection, &temp);
    return true;
}

void __declspec(naked) hooked_send() {
    //hooked++;
    //asm(".intel_syntax noprefix;"
    //    "mov eax,[esi+54];"
    //    "mov ecx,[esi+50];"
    //    ".att_syntax;");
    asm("movl 0x54(%esi), %eax;");
    asm("movl 0x50(%esi), %ecx;");
    asm("movl %0, %%eax;" : : "r" (jmpBackAddy));
    asm("push %eax;");
    asm("movl 0x4(%esp), %eax;");
    asm("ret;");
    //asm volatile ("jmp *%0;" : : "r" (jmpBackAddy));
//    asm("pushad;"); // pushfd

//    hooked++;

    //https://www.felixcloutier.com/x86/pusha:pushad.html
    // DWORD regEDI = 0x00000000;
    // asm("pop dword [regEDI]");
    // asm("push dword [regEDI]");


    //asm("popad;"
    //    "mov eax,[esi+0x54];"
    //    "mov ecx,[esi+0x50];"
    //    "jmp dword [jmpBackAddy];");
    //uintptr_t* jmpBackAddy = reinterpret_cast<uintptr_t*>(hookAddress + hooklength);
    //void (*foo)(void) = (void (*)())jmpBackAddy;
    //foo();
}

// process main function
//
void Main(HINSTANCE hInst) {
    startConsole();

    // init hook
    detour_send(hookAddress, reinterpret_cast<uintptr_t*>(&hooked_send), hooklength);

    // print status
    while (true) {
        Sleep(1000);
        if (GetAsyncKeyState(VK_ESCAPE) & 1) {
            break;
        }
        //BYTE SendBuffer[] = {0x4F, 0x01, 0x00, 0x00};
        //Send(sizeof(SendBuffer), &SendBuffer[0]);
        cout << hooked << endl;
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
