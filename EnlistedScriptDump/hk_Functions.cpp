#include "pch.h"
#include "hk_Functions.h"
#include "Utils.h"

get_file_info_t o_get_file_info = NULL;
sq_compile_t* o_sq_compile = NULL;

file_info_t* __fastcall hk_get_file_info(void* thisptr, const char* file_name)
{
    file_info_t* info = o_get_file_info(thisptr, file_name);

    // SO SCUFFED LMAOOO
    int v4 = *(file_name + 23);
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

VOID hkFunctions::FindFunctions()
{
    sqCompileAddr = (UINT64)FindPattern((UINT64)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (BYTE*)"\x56\x57\x48\x83\xEC\x68\x48\x89\xCE\x48\x8B\x84\x24\xA8\x00\x00", "xxxxxxxxxxxxxxx");
    getFileInfoAddr = (UINT64)FindPattern((UINT64)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage, (BYTE*)"\x41\x57\x41\x56\x41\x55\x41\x54\x56\x57\x53\x48\x83\xEC\x20\x49\x89\xD6\x49\x89\xCD\x44\x0F\xB6\x42\x17\x48\x89\xD3\x45\x84\xC0", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
}

hkFunctions::hkFunctions()
{
    create_console();
	gameH = GetCurrentProcess();
	GetModuleInformation(gameH, GetModuleHandle(NULL), &moduleInfo, sizeof(MODULEINFO));
	FindFunctions();
}

MH_STATUS hkFunctions::HookFunctions()
{
    MH_STATUS status;

    status = MH_Initialize();

    printf("MinHook intialization was: %s\n", status != MH_STATUS::MH_OK ? "Not successful" : "Successful");

    status = MH_CreateHook((LPVOID)getFileInfoAddr, hk_get_file_info, reinterpret_cast<LPVOID*>(&o_get_file_info));
    printf("MinHook hook creation getFileInfoAddr: %s\n", status != MH_STATUS::MH_OK ? "Not successful" : "Successful");

    status = MH_CreateHook((LPVOID)sqCompileAddr, hk_sq_compile, reinterpret_cast<LPVOID*>(&o_sq_compile));
    printf("MinHook hook creation sqCompileAddr: %s\n", status != MH_STATUS::MH_OK ? "Not successful" : "Successful");

    status = MH_EnableHook(MH_ALL_HOOKS);
    printf("MinHook enable was: %s\n", status != MH_STATUS::MH_OK ? "Not successful" : "Successful");

    return status;
}