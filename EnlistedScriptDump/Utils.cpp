#include "pch.h"

#include "Utils.h"

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

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

VOID create_console()
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