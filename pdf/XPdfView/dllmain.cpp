// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "PdfShow.h"

HINSTANCE g_hInstance = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			g_hInstance = hModule;			
		}
		break;
	case DLL_THREAD_ATTACH:
		{

		}
		break;
	case DLL_THREAD_DETACH:
		{

		}
		break;
	case DLL_PROCESS_DETACH:
		{	
			g_hInstance = NULL;
		}
		break;
	}
	return TRUE;
}

