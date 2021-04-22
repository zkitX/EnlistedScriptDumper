#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <TlHelp32.h>
#include <io.h>
#include <fcntl.h>
#include <Psapi.h>
#include <DbgHelp.h>
#include <stdio.h>

#include "MinHook.h"
