#pragma once
#ifndef HK_FUNCTIONS_H
#define HK_FUNCTIONS_H
#pragma region Function Defines

struct file_info_t
{
	char pad[0x20];
	const char* source = nullptr;
	uint32_t source_length;
	int32_t tabSize;
};

typedef file_info_t* (__fastcall* get_file_info_t)(void*, const char*);

struct buf_state_t
{
	const char* buf;
	uintptr_t ptr;
	uintptr_t size;
};

typedef uintptr_t(__fastcall sq_compile_t)(void*, void*, buf_state_t*, char*, bool, uintptr_t);

#pragma endregion

class hkFunctions {
private:
	HANDLE gameH;
	MODULEINFO moduleInfo;
	UINT64 getFileInfoAddr, sqCompileAddr;
protected:
	VOID FindFunctions();
public:
	hkFunctions();
	MH_STATUS HookFunctions();
};

#endif