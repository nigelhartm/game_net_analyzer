// https://guidedhacking.com/threads/c-detour-hooking-function-tutorial.7930/
// Import libraries
//
#define UNICODE
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <vector>
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

int hooklength = 6;
uintptr_t* hookAddress = reinterpret_cast<uintptr_t*>(0x008A7B43); // after subfunction
uintptr_t* jmpBackAddy = reinterpret_cast<uintptr_t*>(reinterpret_cast<uint32_t> (hookAddress) + hooklength);
BYTE ownBuffer[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
BYTE* bufferAddress = (BYTE*) 0x00000000;
uint32_t bufferLength = 0;

TinternalSend internalSend = (TinternalSend) 0x012F7B40;
TinternalSendAfter internalSendAfter = (TinternalSendAfter) 0x012F7BB0;

// allocate a new console window
//
void startConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    cout << "Hack started." << endl;
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

void otherFunc() {
    uintptr_t* ptr_bufferAddress = reinterpret_cast<uintptr_t*>(bufferAddress);
    memcpy (&ownBuffer[0], ptr_bufferAddress, bufferLength);
    if(bufferLength != 1) {
        cout << endl << endl;
        cout << "SEND PACKAGE (LENGTH = " << bufferLength << " + 1)" << endl;
    }
    for(int i = 0; i < bufferLength; i++) {
        cout << ( ( (int) ownBuffer[i] < 16) ? "0" : "") << uppercase << hex << (int)ownBuffer[i];
    }
}

void __declspec(naked) hooked_send() {
    // Push Address (return), EAX, ECX, EBX, EDX, ESI, EDI, EBP
    asm("push %eax;");
    asm("movl %0, %%eax;"::"r" (jmpBackAddy):);
    asm("push %eax;");
    asm("push %ecx;");
    asm("movl 0x08(%esp), %ecx;");
    asm("movl 0x04(%esp), %eax;");
    asm("movl %eax, 0x08(%esp);");
    asm("movl %ecx, 0x04(%esp);");
    asm("push %ebx;");
    asm("push %edx;");
    asm("push %esi;");
    asm("push %edi;");
    asm("push %ebp;");

    // mein code
    asm("movl 0x2C(%%esp), %0": "=r" (bufferAddress)::);
    asm("movl 0x28(%%esp), %0": "=r" (bufferLength)::);
    otherFunc();

    // Pop EBP, EDI, ESI, EDX, EXB, ECX, EAX
    asm("pop %ebp;");
    asm("pop %edi;");
    asm("pop %esi;");
    asm("pop %edx;");
    asm("pop %ebx;");
    asm("pop %ecx;");
    asm("pop %eax;");

    // Run deleted instructions
    asm("movl 0x54(%esi), %eax;");
    asm("movl 0x50(%esi), %ecx;");

    // return
    asm("ret");
}

// FInd pattern?
// easy implementation... faster ones available under https://guidedhacking.com/threads/external-internal-pattern-scanning-guide.14112/
//
DWORD GetAddressFromSignature(std::vector<int> signature, DWORD startaddress=0, DWORD endaddress=0) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (startaddress == 0) {
        startaddress = (DWORD)(si.lpMinimumApplicationAddress);
    }
    if (endaddress == 0) {
        endaddress = (DWORD)(si.lpMaximumApplicationAddress);
    }
 
    MEMORY_BASIC_INFORMATION mbi{ 0 };
    DWORD protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);
 
    for (DWORD i = startaddress; i < endaddress -signature.size(); i++) {
        //std::cout << "scanning: " << std::hex << i << std::endl;
        if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
            if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT)) {
                //std::cout << "Bad Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
                i += mbi.RegionSize;
                continue; // if bad adress then dont read from it
            }
            //std::cout << "Good Region! Region Base Address: " << mbi.BaseAddress << " | Region end address: " << std::hex << (int)((DWORD)mbi.BaseAddress + mbi.RegionSize) << std::endl;
            for (DWORD k = (DWORD)mbi.BaseAddress; k < (DWORD)mbi.BaseAddress + mbi.RegionSize - signature.size(); k++) {
                for (DWORD j = 0; j < signature.size(); j++) {
                    if (signature.at(j) != -1 && signature.at(j) != *(BYTE*)(k + j))
                        break;
                    if (j + 1 == signature.size())
                        return k;
                }
            }
            i = (DWORD)mbi.BaseAddress + mbi.RegionSize;
        }
    }
    return NULL;
}

// process main function
//
void Main(HINSTANCE hInst) {
    startConsole();

    // init hook
    std::vector<int> signature_send = {0x56, 0x8B, 0xF1, 0x8B, 0x46, 0x54, 0x8B, 0x4E, 0x50, 0x57, 0x8B, 0x7C, 0x24, 0x0C, 0x2B, 0xC8, 0x8D, 0x57, 0x01, 0x3B, 0xD1};
    cout << "Search for send function." << endl;
    DWORD add_send = GetAddressFromSignature(signature_send,0x00000000,0x00000000);
    cout << "Found Send function at "  << uppercase << hex << add_send << endl;
    hookAddress = reinterpret_cast<uintptr_t*>(add_send + 0x03);
    jmpBackAddy = reinterpret_cast<uintptr_t*>(reinterpret_cast<uint32_t> (hookAddress) + hooklength);
    detour_send(hookAddress, reinterpret_cast<uintptr_t*>(&hooked_send), hooklength);
    cout << "Hooked function at " << uppercase << hex << hookAddress << endl;

    // print status
    while (true) {
        Sleep(500);
        if (GetAsyncKeyState(VK_ESCAPE) & 1) {
            break;
        }
        //BYTE SendBuffer[] = {0x4F, 0x01, 0x00, 0x00};
        //Send(sizeof(SendBuffer), &SendBuffer[0]);
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
