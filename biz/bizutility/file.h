#pragma once

#include <Windows.h>
#include <Shlwapi.h>


namespace biz_utility
{
	class File
	{
	public:
		bool FindFirst(const wchar_t* path)
		{
			_hfind = ::FindFirstFileW(path, &_findData);
			return (_hfind != INVALID_HANDLE_VALUE);
		}

		bool FindNext()
		{
			return !!FindNextFile(_hfind, &_findData);
		}

		std::wstring CurName()
		{
			return _findData.cFileName;
		}

		bool CurIsDir()
		{
			return (_findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0;
		}


	private:
		HANDLE					_hfind;
		WIN32_FIND_DATAW		_findData;
	};
}