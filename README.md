# cpp_game_net_emulator

# Trash (not working)
install from https://github.com/TsudaKageyu/minhook
using
```
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg integrate install
.\vcpkg\vcpkg install minhook
```

# important
https://guidedhacking.com/threads/c-detour-hooking-function-tutorial.7930/<br>
Common Problem: Mixing up hexadecimal and decimal notation​<br>
Cheat Engine Display:<br>
mov edi, [esp + 014]<br>
^ this is actually 0x14 (cheat engine doesn't say 0x so you may not realize it's hexadecimal)<br>
<br>
In Visual Studio you must do:<br>
mov edi, [esp + 0x14]<br>
<br>
just doing esp + 14 in Visual Studio = decimal<br>
<br>
# more
create naked function? declscpec?<br>
```
void __declspec(naked) ourFunct() {

    __asm {
        mov qword ptr[rcx], 0x46
        jmp[jmpBackAddy]
    }
}
```
pushad pushfd? <br>

```
https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
int no = 100, val ;
    asm ("movl %1, %%ebx;"
         "movl %%ebx, %0;"
         : "=r" ( val )        /* output */
         : "r" ( no )         /* input */
         : "%ebx"         /* clobbered register */
     );
```

# change compiler
g++ -c main.cpp -masm=intel -m32
