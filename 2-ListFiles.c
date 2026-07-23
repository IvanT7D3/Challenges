//x86_64-w64-mingw32-gcc 2-ListFiles.c -fno-ident -s -Wl,--no-insert-timestamp -o 2-ListFiles.exe

#include <wchar.h>
#include <windows.h>

void ConvertTimestamps(const FILETIME CreationTime, const FILETIME LastAccessTime, const FILETIME LastWriteTime)
{
	SYSTEMTIME UTCTime;
	SYSTEMTIME LocalTime;
	BOOL ConvertedToUTCTime = FileTimeToSystemTime(&CreationTime, &UTCTime);
	if (ConvertedToUTCTime == 0)
	{
		wprintf(L"[-] FileTimeToSystemTime failed to convert Creation timestamp: %lu\n", GetLastError());
	}
	BOOL ConvertedToLocalTime = SystemTimeToTzSpecificLocalTime(NULL, &UTCTime, &LocalTime);
	if (ConvertedToLocalTime == 0)
	{
		wprintf(L"[-] SystemTimeToTzSpecificLocalTime failed to convert Creation timestamp to local time: %lu\n", GetLastError());
	}
	else
	{
		wprintf(L"[*] Creation: %04d-%02d-%02d %02d:%02d:%02d\n", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
	}

	ConvertedToUTCTime = FileTimeToSystemTime(&LastAccessTime, &UTCTime);
	if (ConvertedToUTCTime == 0)
	{
		wprintf(L"[-] FileTimeToSystemTime failed to convert LastAccess timestamp: %lu\n", GetLastError());
	}
	ConvertedToLocalTime = SystemTimeToTzSpecificLocalTime(NULL, &UTCTime, &LocalTime);
	if (ConvertedToLocalTime == 0)
	{
		wprintf(L"[-] SystemTimeToTzSpecificLocalTime failed to convert LastAccess timestamp to local time: %lu\n", GetLastError());
	}
	else
	{
		wprintf(L"[*] LastAccess: %04d-%02d-%02d %02d:%02d:%02d\n", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
	}

	ConvertedToUTCTime = FileTimeToSystemTime(&LastWriteTime, &UTCTime);
	if (ConvertedToUTCTime == 0)
	{
		wprintf(L"[-] FileTimeToSystemTime failed to convert LastWrite timestamp: %lu\n", GetLastError());
	}
	ConvertedToLocalTime = SystemTimeToTzSpecificLocalTime(NULL, &UTCTime, &LocalTime);
	if (ConvertedToLocalTime == 0)
	{
		wprintf(L"[-] SystemTimeToTzSpecificLocalTime failed to convert LastWrite timestamp to local time: %lu\n", GetLastError());
	}
	else
	{
		wprintf(L"[*] LastWrite: %04d-%02d-%02d %02d:%02d:%02d\n", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
	}
	wprintf(L"\n");
}

void ListFilesRecursively(wchar_t *StartingPath)
{
	wchar_t SearchPath[MAX_PATH];
	wchar_t FullPath[MAX_PATH];
	WIN32_FIND_DATAW FindDataW;
	LARGE_INTEGER FileSize;
	HANDLE FileHandle = NULL;

	swprintf(SearchPath, MAX_PATH, L"%ls\\*", StartingPath);

	FileHandle = FindFirstFileW(SearchPath, &FindDataW);
	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		wprintf(L"[-] FindFirstFileW failed for: StartingPath: %ls | SearchPath: %ls | Error: %lu\n", StartingPath, SearchPath, GetLastError());
		return;
	}

	do
	{
		if ((wcscmp(FindDataW.cFileName, L".") == 0) || wcscmp(FindDataW.cFileName, L"..") == 0)
		{
			continue;
		}

		swprintf(FullPath, MAX_PATH, L"%ls\\%ls", StartingPath, FindDataW.cFileName);

		if (FindDataW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			wprintf(L"[*] DIR: %ls | Timestamps:\n", FullPath);
			ConvertTimestamps(FindDataW.ftCreationTime, FindDataW.ftLastAccessTime, FindDataW.ftLastWriteTime);
			ListFilesRecursively(FullPath);
		}
		else
		{
			FileSize.LowPart = FindDataW.nFileSizeLow;
			FileSize.HighPart = FindDataW.nFileSizeHigh;
			wprintf(L"[*] FILE: %ls | Size: %lld Bytes. Timestamps:\n", FullPath, FileSize.QuadPart);
			ConvertTimestamps(FindDataW.ftCreationTime, FindDataW.ftLastAccessTime, FindDataW.ftLastWriteTime);
		}
	} while (FindNextFileW(FileHandle, &FindDataW) != 0);
	FindClose(FileHandle);
}

int main()
{
	wchar_t *BasePath = L"C:";
	wprintf(L"[*] Logging started from %ls\n", BasePath);
	ListFilesRecursively(BasePath);
	return 0;
}
