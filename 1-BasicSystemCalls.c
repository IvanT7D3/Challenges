//x86_64-w64-mingw32-gcc 1-BasicSystemCalls.c -fno-ident -s -Wl,--no-insert-timestamp -o 1-BasicSystemCalls.exe

#include <wchar.h>
#include <windows.h>

int main()
{
	wchar_t ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD Size1 = MAX_COMPUTERNAME_LENGTH + 1;

	wchar_t SystemDir[MAX_PATH];
	UINT Size2 = MAX_PATH;

	wchar_t WinDir[MAX_PATH];
	UINT Size3 = MAX_PATH;

	BOOL Name = GetComputerNameW(ComputerName, &Size1);
	if (Name == 0)
	{
		wprintf(L"[-] GetComputerNameW failed: %lu\n", GetLastError());
		return 1;
	}

	wprintf(L"[+] ComputerName: %ls\n", ComputerName);

	UINT Dir = GetSystemDirectoryW(SystemDir, Size2);
	if (Dir == 0)
	{
		wprintf(L"[-] GetSystemDirectoryW failed: %lu\n", GetLastError());
		return 1;
	}

	wprintf(L"[+] System Directory: %ls\n", SystemDir);

	UINT WindowsDir = GetSystemWindowsDirectoryW(WinDir, Size3);
	if (WindowsDir == 0)
	{
		wprintf(L"[-] GetSystemWindowsDirectoryW failed: %lu\n", GetLastError());
		return 1;
	}

	wprintf(L"[+] Windows Directory: %ls\n\n", WinDir);

	DWORD Bitmask = GetLogicalDrives();
	if (Bitmask == 0)
	{
		wprintf(L"[-] GetLogicalDrives failed: %lu\n", GetLastError());
		return 1;
	}

	for (int i = 0; i < 26; i++)
	{
		if (Bitmask & (1 << i))
		{
			wchar_t DriveLetter[4];
			_snwprintf(DriveLetter, sizeof(DriveLetter) / sizeof(DriveLetter[0]), L"%c:\\", L'A' + i);
			UINT DriveType = GetDriveTypeW(DriveLetter);

			if (DriveType == DRIVE_REMOTE)
			{
				wprintf(L"[+] '%ls' is a remote drive\n", DriveLetter);
			}
			else if (DriveType == DRIVE_FIXED)
			{
				wprintf(L"[+] '%ls' is a fixed drive\n", DriveLetter);
			}
			else if (DriveType == DRIVE_REMOVABLE)
			{
				wprintf(L"[+] '%ls' is a removable drive\n", DriveLetter);
			}
			else if (DriveType == DRIVE_CDROM)
			{
				wprintf(L"[+] '%ls' is a CD-ROM\n", DriveLetter);
			}
			else if (DriveType == DRIVE_RAMDISK)
			{
				wprintf(L"[+] '%ls' is a ramdisk\n", DriveLetter);
			}
			else if (DriveType == DRIVE_NO_ROOT_DIR)
			{
				wprintf(L"[+] '%ls' is an invalid root path: DRIVE_NO_ROOT_DIR\n", DriveLetter);
				continue;
			}
			else
			{
				wprintf(L"[-] Unknown DriveType: %u\n", DriveType);
				continue;
			}

			wprintf(L"[+] Volume Info for '%ls' Drive:\n", DriveLetter);
			BOOL GotAll = 0;
			DWORD SectorsPerCluster = 0;
			DWORD BytesPerSector = 0;
			DWORD NumberOfFreeClusters = 0;
			DWORD TotalNumberOfClusters = 0;
			GotAll = GetDiskFreeSpaceW(DriveLetter, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);
			if (GotAll == 0)
			{
				wprintf(L"[-] GetDiskFreeSpaceW failed: Failed obtaining info for disk: '%ls' (%lu)\n", DriveLetter, GetLastError());
				continue;
			}
			ULARGE_INTEGER FreeBytesAvailableToCaller = { 0 };
			ULARGE_INTEGER TotalNumberOfBytes = { 0 };
			ULARGE_INTEGER TotalNumberOfFreeBytes = { 0 };
			GotAll = GetDiskFreeSpaceExW(DriveLetter, &FreeBytesAvailableToCaller, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);
			if (GotAll == 0)
			{
				wprintf(L"[-] GetDiskFreeSpaceExW failed: Failed obtaining space amount for disk: '%ls' (%lu)\n", DriveLetter, GetLastError());
				continue;
			}

			wchar_t VolumeName[MAX_PATH] = { 0 };
			wchar_t FileSystemName[MAX_PATH] = { 0 };
			DWORD SerialNumber = 0;
			DWORD MaxComponentLength = 0;
			DWORD FileSystemFlags = 0;

			BOOL OkInfo = GetVolumeInformationW(DriveLetter, VolumeName, sizeof(VolumeName) / sizeof(VolumeName[0]), &SerialNumber, &MaxComponentLength, &FileSystemFlags, FileSystemName, sizeof(FileSystemName) / sizeof(FileSystemName[0]));
			if (OkInfo == 0)
			{
				wprintf(L"[-] GetVolumeInformationW failed for drive '%ls': %lu\n", DriveLetter, GetLastError());
				continue;
			}

			wprintf(L" [*] Volume Name: %ls\n [*] File System: %ls\n [*] Serial Number: %08lX\n [*] Max Name Length: %lu\n", VolumeName, FileSystemName, SerialNumber, MaxComponentLength);

			double MB = 1048576.0;
			double FreeMBs = (double)TotalNumberOfFreeBytes.QuadPart / MB;
			double TotalMBs = (double)TotalNumberOfBytes.QuadPart / MB;
			wprintf(L" [*] MBs (Total/Free) %.3f/%.3f\n [*] SectorsPerCluster: %lu\n [*] BytesPerSector: %lu\n [*] FreeClusters: %lu\n [*] TotalClusters: %lu\n\n", TotalMBs, FreeMBs, SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters);
		}
	}
	return 0;
}
