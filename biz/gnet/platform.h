#pragma once

#ifdef WIN32

#ifdef _WIN64
#error "__i386__ only"
#endif

#ifndef __i386__
#define __i386__
#endif

#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")

#endif