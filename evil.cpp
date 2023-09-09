#define UNICODE

// include all libraries
//
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <vector>
#include <winuser.h>
#include <cstdlib>
#include <sstream>
#include<string>
#include<Tchar.h>

// define constants and variables for GUI
//
#define MYMENU_EXIT         (WM_APP + 101)
#define MYMENU_MESSAGEBOX   (WM_APP + 102) 
#define MYBUTTON_SEND   (WM_APP + 103)
#define MYCHECKBOX_RECV (WM_APP +104)
#define MYCHECKBOX_SEND (WM_APP +105)
HINSTANCE  inj_hModule;
HWND       prnt_hWnd;
HWND TextBox, SendBox, SendButton, CheckBoxRecv, CheckBoxSend;
bool checkbox_recv = false;
bool checkbox_send = false;
LRESULT CALLBACK DLLWindowProc (HWND, UINT, WPARAM, LPARAM);

// set namespace
//
using namespace std;

// create function definitions
typedef int (__thiscall* TinternalSend) (uintptr_t*, uint32_t, BYTE*);
typedef int (__thiscall* TinternalSendAfter) (uintptr_t*);

// define variables for hooking processes
//
uintptr_t* CPythonNetworkStream = reinterpret_cast<uintptr_t*>(0x00000000); // find by hooking send and ecx register
bool initSend = FALSE;
BYTE* bufferptr_CPythonNetworkStream = (BYTE*) 0x00000000;
int hooklength = 6;
int hooklengthRecv = 5;
uintptr_t* hookAddress = reinterpret_cast<uintptr_t*>(0x008A7B43); // after subfunction
uintptr_t* jmpBackAddy = reinterpret_cast<uintptr_t*>(reinterpret_cast<uint32_t> (hookAddress) + hooklength);
uintptr_t* hookAddressRecv = reinterpret_cast<uintptr_t*>(0x008A7B43); // after subfunction
uintptr_t* jmpBackAddyRecv = reinterpret_cast<uintptr_t*>(reinterpret_cast<uint32_t> (hookAddressRecv) + hooklengthRecv);
BYTE ownBuffer[9000];
BYTE ownBufferRecv[9000];
BYTE mySendBuffer[9000];
BYTE* bufferAddress = (BYTE*) 0x00000000;
uint32_t bufferLength = 0;
BYTE* bufferAddressRecv = (BYTE*) 0x00000000;
uint32_t bufferLengthRecv = 0;
TinternalSend internalSend = (TinternalSend) 0x00000000;
TinternalSendAfter internalSendAfter = (TinternalSendAfter) 0x00000000;

// allocate a new console window for the process (mainly for debugging)
//
void startConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    cout << "GAME NETWORK TRACKER started." << endl;
}

// close console window (important because of our injection)
//
void stopConsole() {
    cout << "GAME NETWORK TRACKER stopped." << endl;
    FreeConsole();
}

// Send our own code to the server
//
void Send(uint32_t length, BYTE* ptrSendBuffer) {
    internalSend(CPythonNetworkStream, length, ptrSendBuffer);
    internalSendAfter(CPythonNetworkStream);
}

// detour the send function
//
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
    //Filter Header
    //if(ownBuffer[0] != 0x0B) {
        std::stringstream stream;
        for(int i = 0; i < bufferLength; i++) {
            stream << ( ( (int) ownBuffer[i] < 16) ? "0" : "") << uppercase << hex << (int)ownBuffer[i];
        }
        if(bufferLength > 1) {
            string prefix = "SEND: ";
            string buffer = prefix.append(stream.str());//.append("\r\n"));
            if(checkbox_send) {
                int index = GetWindowTextLength (TextBox);
                SetFocus (TextBox);
                SendMessageA(TextBox, EM_SETSEL, (WPARAM)index, (LPARAM)index);
                SendMessageA(TextBox, EM_REPLACESEL, 0, (LPARAM)buffer.c_str());
            }
        }
        else {
            //cout << "Skipped one byte at the end: " << ( ( (int) ownBuffer[0] < 16) ? "0" : "") << uppercase << hex << (int)ownBuffer[0] << endl;
            string prefix = "";
            string buffer = prefix.append(stream.str().append("\r\n"));
            if(checkbox_send) {
                int index = GetWindowTextLength (TextBox);
                SetFocus (TextBox);
                SendMessageA(TextBox, EM_SETSEL, (WPARAM)index, (LPARAM)index);
                SendMessageA(TextBox, EM_REPLACESEL, 0, (LPARAM)buffer.c_str());
            }
        }
    //}
    // fins first different network so start injection after being ingame
    if(initSend == FALSE){
        CPythonNetworkStream = reinterpret_cast<uintptr_t*>(bufferptr_CPythonNetworkStream);
        cout << "CPYTHON found inside Send at " << (void*)bufferptr_CPythonNetworkStream << endl;
        initSend = TRUE;
    }
}

void otherFuncRecv() {
    uintptr_t* ptr_bufferAddress = reinterpret_cast<uintptr_t*>(bufferAddressRecv);
    memcpy (&ownBufferRecv[0], ptr_bufferAddress, bufferLengthRecv);
    //Filter Header
    //if(ownBufferRecv[0] != 0x0B) {
        std::stringstream stream;
        for(int i = 0; i < bufferLengthRecv; i++) {
            stream << ( ( (int) ownBufferRecv[i] < 16) ? "0" : "") << uppercase << hex << (int)ownBufferRecv[i];
        }
        string prefix = "RECV: ";
        string buffer = prefix.append(stream.str().append("\r\n"));
        if(checkbox_recv) {
            int index = GetWindowTextLength (TextBox);
            SetFocus (TextBox);
            SendMessageA(TextBox, EM_SETSEL, (WPARAM)index, (LPARAM)index);
            SendMessageA(TextBox, EM_REPLACESEL, 0, (LPARAM)buffer.c_str());
        }
    //}
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
    asm("movl 0x2C(%%esp), %0;": "=r" (bufferAddress)::);
    asm("movl 0x28(%%esp), %0;": "=r" (bufferLength)::);
    asm("movl 0x08(%%esp), %0;": "=r" (bufferptr_CPythonNetworkStream)::);
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
    asm("ret;");
}

void __declspec(naked) hooked_recv() {
    // Push Address (return), EAX, ECX, EBX, EDX, ESI, EDI, EBP
    asm("push %eax;");
    asm("movl %0, %%eax;"::"r" (jmpBackAddyRecv):);
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
    asm("movl 0x48(%%esp), %0": "=r" (bufferAddressRecv)::);
    asm("movl 0x4C(%%esp), %0": "=r" (bufferLengthRecv)::);
    //asm("movl %%ecx, %0": "=r" (bufferptr_CPythonNetworkStream)::); // wurde nach esi verschoben
    otherFuncRecv();
    // Pop EBP, EDI, ESI, EDX, EXB, ECX, EAX
    asm("pop %ebp;");
    asm("pop %edi;");
    asm("pop %esi;");
    asm("pop %edx;");
    asm("pop %ebx;");
    asm("pop %ecx;");
    asm("pop %eax;");
    // Run deleted instructions
    asm("movl (%edx), %edi;");
    asm("movl 0x30(%ecx), %edx;");
    // to complicated:
    //asm("movl 0x20(%esp), %ebx;");
    //asm("push %ebp;");
    //asm("push %esi;");
    asm("ret;");
}


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
        if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
            if (mbi.Protect & protectflags || !(mbi.State & MEM_COMMIT)) {
                i += mbi.RegionSize;
                continue;
            }
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

BOOL RegisterDLLWindowClass(wchar_t szClassName[]){
    WNDCLASSEX wc;
    wc.hInstance =  inj_hModule;
	wc.lpszClassName = (LPCWSTR)L"InjectedDLLWindowClass";
    wc.lpszClassName = (LPCWSTR)szClassName;
    wc.lpfnWndProc = DLLWindowProc;
    wc.style = CS_DBLCLKS;
    wc.cbSize = sizeof (WNDCLASSEX);
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    if (!RegisterClassEx (&wc))
		return 0;
}

HMENU CreateDLLWindowMenu(){
	HMENU hMenu;
	hMenu = CreateMenu();
	HMENU hMenuPopup;
    if(hMenu==NULL)
        return FALSE;
    hMenuPopup = CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING, MYMENU_EXIT, TEXT("Exit"));
    AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("File")); 
	hMenuPopup = CreatePopupMenu();
    AppendMenu (hMenuPopup, MF_STRING,MYMENU_MESSAGEBOX, TEXT("Clear Textbox")); 
    AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("Options")); 
	return hMenu;
}

DWORD WINAPI ThreadProc( LPVOID lpParam ){
    MSG messages;
	wchar_t *pString = reinterpret_cast<wchar_t * > (lpParam);
	HMENU hMenu = CreateDLLWindowMenu();
	RegisterDLLWindowClass(L"InjectedDLLWindowClass");
	prnt_hWnd = FindWindow(L"Window Injected Into ClassName", L"Window Injected Into Caption");
	HWND hwnd = CreateWindowEx (0, L"InjectedDLLWindowClass", pString, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1225, 750, prnt_hWnd, hMenu,inj_hModule, NULL );
	ShowWindow (hwnd, SW_SHOWNORMAL);
    startConsole();
    std::vector<int> signature_send = {0x56, 0x8B, 0xF1, 0x8B, 0x46, 0x54, 0x8B, 0x4E, 0x50, 0x57, 0x8B, 0x7C, 0x24, 0x0C, 0x2B, 0xC8, 0x8D, 0x57, 0x01, 0x3B, 0xD1};
    cout << "Search for send function." << endl;
    DWORD add_send = GetAddressFromSignature(signature_send,0x00000000,0x00000000);
    cout << "Found Send function at "  << uppercase << hex << add_send << endl;
    internalSend = (TinternalSend) (add_send);
    internalSendAfter = (TinternalSendAfter) (add_send+0x70);
    hookAddress = reinterpret_cast<uintptr_t*>(add_send + 0x03);
    jmpBackAddy = reinterpret_cast<uintptr_t*>(reinterpret_cast<uint32_t> (hookAddress) + hooklength);
    detour_send(hookAddress, reinterpret_cast<uintptr_t*>(&hooked_send), hooklength);
    
    std::vector<int> signature_recv = {0x83, 0xEC, 0x10, 0x8B, 0x41, 0x34, 0x53, 0x8B, 0x5C, 0x24, 0x20, 0x55, 0x56, 0x8D, 0x71, 0x34, 0x57, 0x89, 0x4C, 0x24, 0x10, 0x89, 0x74, 0x24, 0x1C};
    cout << "Search for Recv function." << endl;
    DWORD add_recv = GetAddressFromSignature(signature_recv,0x00000000,0x00000000);
    cout << "Found Recv function at "  << uppercase << hex << add_recv << endl;
    hookAddressRecv = reinterpret_cast<uintptr_t*>(add_recv + 0x2B);
    jmpBackAddyRecv = reinterpret_cast<uintptr_t*>(reinterpret_cast<uint32_t> (hookAddressRecv) + hooklengthRecv);
    detour_send(hookAddressRecv, reinterpret_cast<uintptr_t*>(&hooked_recv), hooklengthRecv);

    while (GetMessage (&messages, NULL, 0, 0)) {
		TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    stopConsole();
    FreeLibraryAndExitThread(inj_hModule, 0);
    return 1;
}

LRESULT CALLBACK DLLWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
        case WM_CREATE:
            TextBox = CreateWindow(L"EDIT", L"", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | WS_HSCROLL | ES_READONLY, 0, 0, 1200, 600, hwnd, (HMENU) 1, NULL, NULL);
            SendBox = CreateWindow(L"EDIT", L"Just usable if injected after choosing char. . .",  WS_BORDER | WS_CHILD | WS_VISIBLE, 0, 610, 1100, 30, hwnd, (HMENU) 1, NULL, NULL);
            SendButton = CreateWindow(L"BUTTON", L"SEND", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 1110, 610, 90, 30, hwnd, (HMENU)MYBUTTON_SEND, NULL, NULL);
            CheckBoxRecv = CreateWindow(TEXT("button"), TEXT("Show Recv"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 20, 650, 185, 30, hwnd, (HMENU)MYCHECKBOX_RECV, NULL, NULL);
            CheckBoxSend = CreateWindow(TEXT("button"), TEXT("Show Send"), WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 250, 650, 185, 30, hwnd, (HMENU)MYCHECKBOX_SEND, NULL, NULL);
            CheckDlgButton(CheckBoxRecv, 1, BST_UNCHECKED);
            CheckDlgButton(CheckBoxSend, 1, BST_UNCHECKED);
            checkbox_recv = false;
            checkbox_send = false;
		case WM_COMMAND:
               switch(wParam){
                    case MYMENU_EXIT: {
						SendMessage(hwnd, WM_CLOSE, 0, 0);
                        break;
                    }
                    case MYMENU_MESSAGEBOX: {
                        int index = GetWindowTextLength (TextBox);
                        SetFocus (TextBox);
                        SendMessage(TextBox, EM_SETREADONLY, FALSE, 0);
                        SendMessage(TextBox, EM_SETSEL, (WPARAM)0, (LPARAM)index);
                        SendMessage(TextBox, WM_CLEAR, (WPARAM)0, (LPARAM)index);
                        SendMessage(TextBox, EM_SETREADONLY, TRUE, 0);
                        break;
                    }
                    case MYBUTTON_SEND: {
                        cout << "YOU SEND." << endl;
                        int len = SendMessage(SendBox, WM_GETTEXTLENGTH, 0, 0);
                        cout << len << endl;
                        TCHAR* display = new TCHAR[len];
                        SendMessage(SendBox,WM_GETTEXT,len+1,(LPARAM)display);
                        //_tprintf(display);
                        wstring test(&display[0]);
                        string test2(test.begin(), test.end());
                        cout << test2 << endl;
                        if(len%2==0) {
                            int j=0;
                            for(int i=0; i < test2.length()-1; i=i+2) {
                                cout << stoi(test2.substr(i,2).c_str(), 0, 16) << endl;
                                mySendBuffer[j] = static_cast<int>(stoi(test2.substr(i,2).c_str(), 0, 16));
                                j++;
                            }
                            Send(len/2, &mySendBuffer[0]);
                        }
                        int index = GetWindowTextLength (SendBox);
                        cout << index << endl;
                        SetFocus (SendBox); // set focus
                        SendMessage(SendBox, EM_SETSEL, (WPARAM)0, (LPARAM)index);
                        SendMessage(SendBox, WM_CLEAR, (WPARAM)0, (LPARAM)index);
                        break;
                    }
                    case MYCHECKBOX_RECV: {
                        if (checkbox_recv) {
                            CheckDlgButton(CheckBoxRecv, 1, BST_UNCHECKED);
                            checkbox_recv = false;
                        } else {
                            CheckDlgButton(CheckBoxRecv, 1, BST_CHECKED);
                            checkbox_recv = true;
                        }
                        break;
                    }
                    case MYCHECKBOX_SEND: {
                        if (checkbox_send) {
                            CheckDlgButton(CheckBoxSend, 1, BST_UNCHECKED);
                            checkbox_send = false;
                        } else {
                            CheckDlgButton(CheckBoxSend, 1, BST_CHECKED);
                            checkbox_send = true;
                        }
                        break;
                    }
               }
               break;
		case WM_DESTROY:
			PostQuitMessage (0);
			break;
		default:
			return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call,LPVOID lpReserved) {
	if(ul_reason_for_call==DLL_PROCESS_ATTACH) {
		inj_hModule = hModule;
		CreateThread(0, NULL, ThreadProc, (LPVOID)L"GAME NETWORK TRACKER", NULL, NULL);
	}
	return TRUE;
}