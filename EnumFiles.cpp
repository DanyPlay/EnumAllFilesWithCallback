// EnumFiles.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EnumFiles.h"
#include <windows.h>
//#include <strsafe.h>
#include <Shlwapi.h>

#pragma comment(lib, "advapi32")
#pragma comment(lib, "Shlwapi")

#pragma comment(linker, "/entry:wWinMain")
//#pragma comment(linker, "/merge:.rdata=.text")

#pragma function(memcpy)
#pragma function(memmove)
#pragma function(memset)


/* NOTE: This code is duplicated in memcpy function */
void * __cdecl memmove(void *dest, const void *src, size_t count)
{
	char *char_dest = (char *)dest;
	char *char_src = (char *)src;

	if ((char_dest <= char_src) || (char_dest >= (char_src + count)))
	{
		/*  non-overlapping buffers */
		while (count > 0)
		{
			*char_dest = *char_src;
			char_dest++;
			char_src++;
			count--;
		}
	}
	else
	{
		/* overlaping buffers */
		char_dest = (char *)dest + count - 1;
		char_src = (char *)src + count - 1;

		while (count > 0)
		{
			*char_dest = *char_src;
			char_dest--;
			char_src--;
			count--;
		}
	}

	return dest;
}
void * __cdecl memset (
					   void *dst,
					   int val,
					   size_t count
					   )
{
	void *start = dst;

#if defined (_M_IA64) || defined (_M_AMD64)

	{


		__declspec(dllimport)


			void RtlFillMemory( void *, size_t count, char );

		RtlFillMemory( dst, count, (char)val );

	}

#else  /* defined (_M_IA64) || defined (_M_AMD64) */
	while (count--) {
		*(char *)dst = (char)val;
		dst = (char *)dst + 1;
	}
#endif  /* defined (_M_IA64) || defined (_M_AMD64) */

	return(start);
}

// BOOL EnumAndDeleteFile()
// {
// 	BOOL bRet = FALSE;
// 
// 	LPCWSTR pszPath = ;
// 	WCHAR szPattern[MAX_PATH * 2];
// 	szPattern[0] = L'\0';
// 	StringCchCopy(szPattern, _countof(szPattern), pszPath);
// 	LPCWSTR strPath = pszPath;
// 
// 	StringCbCat(szPattern, sizeof(szPattern), L"\\*.exe_cq.log");
// 
// 	WIN32_FIND_DATA fd;
// 	my_memset(&fd, 0, sizeof(fd));
// 	HANDLE hFind = FindFirstFile(szPattern, &fd);
// 
// 	if (hFind == INVALID_HANDLE_VALUE)
// 	{
// 		DWORD dwErr = GetLastError();
// 		if (ERROR_FILE_NOT_FOUND == dwErr)
// 		{
// 			return TRUE;
// 		}
// 		else
// 		{
// 			return FALSE;
// 		}
// 	}
// 
// 	do
// 	{
// 		WCHAR szRunFile[MAX_PATH * 2];
// 		szRunFile[0] = L'\0';
// 		PathCombine(szRunFile, strPath, fd.cFileName);
// 		BOOL bDel = DeleteFile(szRunFile);
// 	}while(FindNextFile(hFind, &fd));
// 
// 	if (0)
// 	{
// 	}
// 	else
// 	{
// 		bRet = TRUE;
// 	}
// 
// 	FindClose(hFind);
// 
// 	return bRet;
// }

BOOL WINAPI CallbackDeleteFile(LPCWSTR pszFileName)
{
	if (!pszFileName)
		return FALSE;

	BOOL bDel = DeleteFile(pszFileName);
	return bDel;
}

typedef BOOL ((WINAPI* FUNC_CallbackFile)(LPCWSTR pszFileName));
void EnumAllFiles(const WCHAR* pszDir, FUNC_CallbackFile pCallback, LPCWSTR pszPattern = L"*.*")
{
	WIN32_FIND_DATA fd;
	memset(&fd, 0, sizeof(fd));

	WCHAR szFullPath[MAX_PATH * 2];
	lstrcpy(szFullPath, pszDir);
	lstrcat(szFullPath, L"\\");
	lstrcat(szFullPath, pszPattern);

	HANDLE hFind = FindFirstFile(szFullPath, &fd);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (fd.cFileName[0] == L'.' && fd.cFileName[1] == 0)
		{
			BOOL bFind = FindNextFile(hFind, &fd);
			if (!bFind)
			{
				break;
			}
			continue;
		}
		if (fd.cFileName[0] == L'.' && fd.cFileName[1] == L'.' && fd.cFileName[2] == 0)
		{
			BOOL bFind = FindNextFile(hFind, &fd);
			if (!bFind)
			{
				break;
			}
			continue;
		}

		lstrcpy(szFullPath, pszDir);
		PathAppend(szFullPath, fd.cFileName);
		DWORD dwAttrib = GetFileAttributes(szFullPath);
		if (INVALID_FILE_ATTRIBUTES == dwAttrib)
		{
			BOOL bFind = FindNextFile(hFind, &fd);
			if (!bFind)
			{
				break;
			}
			continue;
		}

		BOOL bIsDir = dwAttrib & FILE_ATTRIBUTE_DIRECTORY;

		if (bIsDir)
		{
		}
		else
		{
			if (pCallback)
			{
				pCallback(szFullPath);
			}
		}

		BOOL bFind = FindNextFile(hFind, &fd);
		if (!bFind)
		{
			break;
		}
	}

	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}
}
void EnumAllFilesRecursive(const WCHAR* pszDir, FUNC_CallbackFile pCallback, LPCWSTR pszPattern = L"*.*", BOOL bRecursive = TRUE)
{
	EnumAllFiles(pszDir, pCallback, pszPattern);



	WIN32_FIND_DATA fd;
	memset(&fd, 0, sizeof(fd));

	WCHAR szFullPath[MAX_PATH * 2];
	lstrcpy(szFullPath, pszDir);
	lstrcat(szFullPath, L"\\*.*");
	//StringCbCat(szFullPath, sizeof(szFullPath), pszPattern);

	HANDLE hFind = FindFirstFile(szFullPath, &fd);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (fd.cFileName[0] == L'.' && fd.cFileName[1] == 0)
		{
			BOOL bFind = FindNextFile(hFind, &fd);
			if (!bFind)
			{
				break;
			}
			continue;
		}
		if (fd.cFileName[0] == L'.' && fd.cFileName[1] == L'.' && fd.cFileName[2] == 0)
		{
			BOOL bFind = FindNextFile(hFind, &fd);
			if (!bFind)
			{
				break;
			}
			continue;
		}

		lstrcpy(szFullPath, pszDir);
		PathAppend(szFullPath, fd.cFileName);
		DWORD dwAttrib = GetFileAttributes(szFullPath);
		if (INVALID_FILE_ATTRIBUTES == dwAttrib)
		{
			BOOL bFind = FindNextFile(hFind, &fd);
			if (!bFind)
			{
				break;
			}
			continue;
		}

		BOOL bIsDir = dwAttrib & FILE_ATTRIBUTE_DIRECTORY;

		if (bIsDir)
		{
			if (bRecursive)
			{
				EnumAllFilesRecursive(szFullPath, pCallback, pszPattern, bRecursive);
			}
		}
		else
		{
			//if (pCallback)
			//{
			//	pCallback(szFullPath);
			//}
		}

		BOOL bFind = FindNextFile(hFind, &fd);
		if (!bFind)
		{
			break;
		}
	}

	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
	}
}

BOOL EnumAndDeleteFileRecurse()
{
	WCHAR szPath[MAX_PATH * 2];
	EnumAllFilesRecursive(szPath, CallbackDeleteFile, L"*.log", TRUE);

	return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	EnumAndDeleteFileRecurse();

	ExitProcess(0);
	return 0;
}
