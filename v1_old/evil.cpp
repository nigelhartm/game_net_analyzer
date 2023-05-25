#define UNICODE
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>

using namespace std;

/* Find functions static
    SendItemUsePacket -> Code dissect -> Refferenced Strings -> SendItemUsePacket Error -> select function -> Adresse ist oben und klasse in ecx
    pntBufferCrypt -> breakpoint on ws32_send dann der pointer an stelle 3 aufm stack
    pntSendCounter -> breakpoint on ws32_send dann der pointer an stelle ""7"" aufm stack irgendwo darunter ist der counter
*/

/* SendItemUsePacket
    INVENTORY
    [1] MAIN INVENTORY
        SLOTS:
            [0 - 44]  I
            [45 - 89] II
    [2] EQUIP
        SLOTS:
            [0] ARMOR
            [1] HELMET
            [2] SHOES
            [3] BRACELET
            [4] WEAPON
            [5] NECKLACE
            [6] EARRINGS
            [8] SHIELD
    [7] EQUIP EXTRAS
        SLOTS:
            [0] LEFT WINDSHOES
            [1] RIGHT WINDSHOES
    MISSING: ARROWS, ETC. not tested no ninja
    IMPORTANT: The skill and pot leiste is no extra inventory just like a short cut and has same command like real position
                IF one item blocks more than on slot just the top one is counting, the other one is empty/ not useable
*/
void SendItemUsePacket(uint32_t slot, uint32_t inventory) {
    uint32_t arg1 = (slot<<8) + inventory;

    uintptr_t SendChatPacketAddress = 0x002E29C0;
    typedef int ( __thiscall* tSendChatPacket )( uintptr_t* This, uint32_t);
    tSendChatPacket SendChatPacket = ( tSendChatPacket ) SendChatPacketAddress;
    SendChatPacket (reinterpret_cast<uintptr_t*>(0x08CBEBB8), arg1);
}

int getSendCounter() {
    unsigned char* pntSendCounter = (unsigned char*) 0x0A5416C8;  
    //cout << uppercase << hex << (int)pntSendCounter[0] << endl;
    return (int)pntSendCounter[0];
}

void ReadSendBuffer() {
    int sendCounter = getSendCounter();
    int n = 0;
    while(n<10) {
        int bufCounter = getSendCounter();
        int bufferLength = 21;
        if(sendCounter != bufCounter) {
            sendCounter = bufCounter;
            // print buffer
            unsigned char* pntBufferCrypt = (unsigned char*) 0x074E42A8;
            for(int i=0; i<bufferLength; i++) {
                cout << (((int)pntBufferCrypt[i]<16)?"0":"") << uppercase << hex << (int)pntBufferCrypt[i] << " ";
            }
            cout << endl;
            n=n+1;
        }
        Sleep(1);
    }
}

void startConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    cout << "Hack started" << endl;
}

void stopConsole() {
    FreeConsole();
}

// the real one sending byte by byte
// missing how to hook it to get all the send stuff
//
void SendBytesFunction() {
    struct ItemUsePacket{
        BYTE header;
        BYTE unknown;
        BYTE slot;
        BYTE unknown2;
    };
    uintptr_t SendUseItemPacketAddress = 0x01197B40;
    typedef int ( __thiscall* tSendUseItem )( uintptr_t* This, uint32_t, ItemUsePacket* pointerToStruct);
    tSendUseItem SendUseItem = ( tSendUseItem ) SendUseItemPacketAddress;

    ItemUsePacket iup;
    iup.header = 0x4F;
    iup.unknown = 0x01;
    iup.slot = 0x05;
    iup.unknown2 = 0x00;

    uint32_t length = 4;
    SendUseItem(reinterpret_cast<uintptr_t*>(0x0A805970), length, &iup);

    cout << "Done1." << endl;

    // unknown additional function ... maybe autmatic do 5th byte thing
    // works wir in caller function nach dem senden gecalled
    uintptr_t unkfncaddress = 0x01197BB0;
    typedef int ( __thiscall* tunkfnc )( uintptr_t* This);
    tunkfnc unkfnc = ( tunkfnc ) unkfncaddress;

    unkfnc(reinterpret_cast<uintptr_t*>(0x0A805970));
    
    cout << "Done2." << endl;
}

void Main(HINSTANCE hInst) {
    // Init
    startConsole();

    // Main
    //SendItemUsePacket(0, 1);
    //ReadSendBuffer();
    SendBytesFunction();

    // Exit
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

// #########################################################################
// TRASH OR NOT USED:
// #########################################################################
    /* COMPILE:
        g++ -c evil.cpp
        g++ -shared -o evil.dll evil.o
    */
// #########################################################################
    /* PRINT MEMORY BYTE ARRAY
        while(1==1) {
            unsigned char* pntBufferDecrypt = (unsigned char*) 0x096786B0;  
            for(int i=0; i<21; i++) {
                cout << hex << (int)pntBufferDecrypt[i] << " ";
            }
            cout << endl;
        }
    */
// #########################################################################
    /* NOT WORKING FUNCTION CALL - FIND OUT WHY?
        void SendItemUsePacket() {
            typedef int(__thiscall* TSendItemUsePacket)(int, const uint32_t);
            TSendItemUsePacket FSendItemUsePacket = reinterpret_cast< TSendItemUsePacket >(0x012429C0);
            FSendItemUsePacket(*reinterpret_cast<uintptr_t*>(0x09CB8968), 1);
        }
    */
// #########################################################################
    /* MESSAGE BOX
        MessageBox (0, L"From DLL\n", L"Process Detach", MB_ICONINFORMATION);
    */