#pragma once

#include <string>

using namespace std;

class CLayoutEncrypt
{
public:
	static BOOL EncodeFile(LPCTSTR szSource, LPCTSTR szTarget);

	static BOOL DecodeFile(LPCTSTR szPath, wstring& strContent);
	static BOOL DecodeMem(const unsigned char* buf, size_t len, wstring& strContent);

private:
	static BOOL GetFileData(LPCTSTR szFilePath, PVOID* ppBuffer, DWORD* pdwSize);
};

