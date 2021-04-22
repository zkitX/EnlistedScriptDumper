#pragma once

using namespace std;

BOOLEAN bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
UINT64 FindPattern(UINT64 dwAddress, UINT64 dwLen, BYTE* bMask, const char* szMask);
string ReplaceAll(std::string str, const string& from, const std::string& to);
VOID create_console();