// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"

#include "hk_Functions.h"

VOID main() {
    hkFunctions* hkFuncs = new hkFunctions();
    hkFuncs->HookFunctions();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        main();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

