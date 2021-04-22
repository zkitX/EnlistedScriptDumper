// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"

using namespace std;

BOOLEAN bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)
            return 0;

    return (*szMask) == 0;
}

UINT64 FindPattern(UINT64 dwAddress, UINT64 dwLen, BYTE* bMask, const char* szMask)
{
    for (UINT64 i = 0; i < dwLen; i++)
        if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask))
            return (UINT64)(dwAddress + i);

    return 0;
}

void create_console()
{
    if (!AllocConsole())
    {
        char buffer[1024] = { 0 };
        return;
    }

    auto lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    auto hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
    auto fp = _fdopen(hConHandle, "w");

    freopen_s(&fp, "CONOUT$", "w", stdout);

    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

struct file_info_t
{
    char pad[0x20];
    const char* source = nullptr;
    uint32_t source_length;
    int32_t tabSize;
};

typedef file_info_t* (__fastcall* get_file_info_t)(void*, const char*);
get_file_info_t o_get_file_info;

file_info_t* __fastcall hk_get_file_info(void* thisptr, const char* file_name)
{
    int v4;

    file_info_t* info = o_get_file_info(thisptr, file_name);

    // SO SCUFFED LMAOOO
    v4 = *(file_name + 23);

    if ((v4 & 0x80u) != 0)
        file_name = *(const char**)file_name;

    std::string name = std::string(file_name);
    size_t pos = name.find('./');
    if (pos != std::string::npos)
        name = ReplaceAll(name.substr(pos + 1), "/", "\\");

    if (MakeSureDirectoryPathExists(std::string("C:\\Enlisted\\dascript\\" + name).c_str()))
    {
        std::ofstream file("C:\\Enlisted\\dascript\\" + name, std::ofstream::out);
        file << std::string(info->source, info->source + info->source_length).c_str();
        file.close();
    }

    return info;
}

struct buf_state_t
{
    const char* buf;
    uintptr_t ptr;
    uintptr_t size;
};

typedef uintptr_t(__fastcall sq_compile_t)(void*, void*, buf_state_t*, char*, bool, uintptr_t);
sq_compile_t* o_sq_compile = NULL;

uintptr_t __fastcall hk_sq_compile(void* v, void* read, buf_state_t* p, char* source_name, bool raise_error, uintptr_t unk)
{
    std::string name = ReplaceAll(std::string(source_name), "/", "\\");
    if (MakeSureDirectoryPathExists(std::string("C:\\enlisted\\quirrel\\" + name).c_str()))
    {
        std::ofstream file("C:\\enlisted\\quirrel\\" + name, std::ofstream::out);
        file << std::string(p->buf, p->buf + p->size).c_str();
        file.close();
    }

    return o_sq_compile(v, read, p, source_name, raise_error, unk);
}

bool HookFunctions() {
    UINT64 getFileInfoAddr, sqCompileAddr;
    HANDLE gameH = GetCurrentProcess();
    MH_STATUS status;
    MODULEINFO moduleInfo;

    create_console();

    status = MH_Initialize();

    printf("MinHook intialization was: %s\n", status != MH_STATUS::MH_OK ? "Not successful" : "Successful");

    GetModuleInformation(gameH, GetModuleHandle(NULL), &moduleInfo, sizeof(MODULEINFO));

    sqCompileAddr = (UINT64)FindPattern((UINT64)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (BYTE*)"\x56\x57\x48\x83\xEC\x68\x48\x89\xCE\x48\x8B\x84\x24\xA8\x00\x00", "xxxxxxxxxxxxxxx");
    getFileInfoAddr = (UINT64)FindPattern((UINT64)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (BYTE*)"\x41\x57\x41\x56\x41\x55\x41\x54\x56\x57\x53\x48\x83\xEC\x20\x49\x89\xD6\x49\x89\xCD\x44\x0F\xB6\x42\x17\x48\x89\xD3\x45\x84\xC0", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

    printf("Base address is at: %p\n", moduleInfo.lpBaseOfDll);

    printf("getFileInfoAddr is: %p\n", (PVOID)getFileInfoAddr);
    printf("sqCompileAddr is: %p\n", (PVOID)sqCompileAddr);

    Sleep(10000);

    status = MH_CreateHook((LPVOID)getFileInfoAddr, hk_get_file_info, reinterpret_cast<LPVOID*>(&o_get_file_info));
    printf("MinHook hook creation getFileInfoAddr: %s\n", status != MH_STATUS::MH_OK ? "Not successful" : "Successful");

    status = MH_CreateHook((LPVOID)sqCompileAddr, hk_sq_compile, reinterpret_cast<LPVOID*>(&o_sq_compile));
    printf("MinHook hook creation sqCompileAddr: %s\n", status != MH_STATUS::MH_OK ? "Not successful" : "Successful");

    status = MH_EnableHook(MH_ALL_HOOKS);
    printf("MinHook enable was: %s\n", status != MH_STATUS::MH_OK ? "Not successful" : "Successful");

    return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        return HookFunctions();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

