////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive Library Open Source distribution 
// and is Copyrighted 2000 - 2019 by Artpol Software - Tadeusz Dracz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// For the licensing details refer to the License.txt file.
//
// Web Site: http://www.artpol-software.com
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _ZIP_SYSTEM_WIN

#define _ZIP_SAFE_WINDOWS_API

#include "ZipPlatform.h"
#include "ZipFileHeader.h"
#include "ZipException.h"
#include "ZipAutoBuffer.h"
#include <sys/stat.h>

#ifndef __BORLANDC__
        #include <sys/utime.h>
#else
	#ifndef _UTIMBUF_DEFINED
		#define _utimbuf utimbuf
		#define _UTIMBUF_DEFINED
	#endif
        #include <utime.h>
#endif

#ifndef INVALID_FILE_ATTRIBUTES
	#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

#ifndef INVALID_SET_FILE_POINTER
	#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

#include <direct.h>
#include <io.h>
#include <share.h>
#include <time.h> 
#include <locale>
#include "ZipPathComponent.h"
#include "ZipCompatibility.h"

const TCHAR CZipPathComponent::m_cSeparator = _T('\\');

ULONGLONG ZipPlatform::GetDeviceFreeSpace(LPCTSTR lpszPath)
{
	ULONGLONG uFreeBytesToCaller = 0, uTotalBytes = 0, uFreeBytes = 0;

	CZipPathComponent zpc (lpszPath);
	CZipString szDrive = zpc.GetFileDrive();

	if (!GetDiskFreeSpaceEx(
		szDrive,
		(PULARGE_INTEGER)&uFreeBytesToCaller,
		(PULARGE_INTEGER)&uTotalBytes,
        (PULARGE_INTEGER)&uFreeBytes))

	{
		CZipPathComponent::AppendSeparator(szDrive); // in spite of what is written in MSDN it is sometimes needed (on fixed disks)
		if (!GetDiskFreeSpaceEx(
			szDrive,
			(PULARGE_INTEGER)&uFreeBytesToCaller,
			(PULARGE_INTEGER)&uTotalBytes,
			(PULARGE_INTEGER)&uFreeBytes))	
				return 0;
	}
	return uFreeBytes;
}

CZipString ZipPlatform::GetTmpFileName(LPCTSTR lpszPath, ZIP_SIZE_TYPE uSizeNeeded)
{
		TCHAR empty[] = _T("");
		CZipString tempPath;
		bool bCheckTemp = true;
		if (lpszPath)
		{
			tempPath = lpszPath;
			bCheckTemp = uSizeNeeded > 0 && GetDeviceFreeSpace(tempPath) < uSizeNeeded;

		}
		if (bCheckTemp)
		{
			DWORD size = GetTempPath(0, empty);
			if (size == 0)
				return (CZipString)empty;
		
			GetTempPath(size, tempPath.GetBuffer(size));
			tempPath.ReleaseBuffer();
			if (GetDeviceFreeSpace(tempPath) < uSizeNeeded)
			{
				if (!GetCurrentDirectory(tempPath) || (uSizeNeeded > 0 && GetDeviceFreeSpace(tempPath) < uSizeNeeded))
					return (CZipString)empty;
			}
		}
		CZipPathComponent::AppendSeparator(tempPath);
		tempPath += _T("ZAR");
		tempPath += _T("XXXXXX");
		int length = tempPath.GetLength() + 1;
#if _MSC_VER >= 1400	
		errno_t err = _tmktemp_s(tempPath.GetBuffer(length), (size_t)length);
#else // defined __MINGW32__ || _MSC_VER >= 1200		
		LPTSTR err = _tmktemp(tempPath.GetBuffer(length));
#endif
		tempPath.ReleaseBuffer();
#if _MSC_VER >= 1400	
		if (err != 0)
#else
		if (err == NULL)
#endif
		{
			return (CZipString)empty;
		}
		return tempPath;
}


bool ZipPlatform::GetCurrentDirectory(CZipString& sz)
{
	DWORD i = ::GetCurrentDirectory(0, NULL);
	if (!i)
		return false;
	TCHAR* pBuf = new TCHAR[i];
	bool b = true;
	if (!::GetCurrentDirectory(i, pBuf))
		b = false;
	else
		sz = pBuf;
	delete[] pBuf;
	return b;
}

bool ZipPlatform::SetFileAttr(LPCTSTR lpFileName, DWORD uAttr)
{
	return ::SetFileAttributes(CZipPathComponent::AddPrefix(lpFileName), uAttr) != 0;
}

bool ZipPlatform::GetFileAttr(LPCTSTR lpFileName, DWORD& uAttr)
{
	DWORD temp = ::GetFileAttributes(CZipPathComponent::AddPrefix(lpFileName));
	if (temp == INVALID_FILE_ATTRIBUTES)
		return false;
	uAttr = temp;
	return true;
	
}

bool ZipPlatform::GetFileTimes(LPCTSTR lpFileName, time_t* tModificationTime, time_t* tCreationTime, time_t* tLastAccessTime)
{
	WIN32_FIND_DATA findData = {0};
	CZipString szPath = CZipPathComponent::AddPrefix(lpFileName);
	HANDLE handle = ::FindFirstFile(szPath, &findData);

	if (tModificationTime != NULL)
	{
		*tModificationTime = 0;
	}
	if (tCreationTime != NULL)
	{
		*tCreationTime = 0;
	}
	if (tLastAccessTime != NULL)
	{
		*tLastAccessTime = 0;
	}
	bool ret = false;
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindClose(handle);
		ret = true;			
	}
	else
	{
		// handle the situation when there are no permissions for reading the list of files, but there are permissions for reading the file
		handle = ::CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != INVALID_HANDLE_VALUE)
		{
			ZBOOL bResult = ::GetFileTime(handle, &findData.ftCreationTime, &findData.ftLastAccessTime, &findData.ftLastWriteTime);
			CloseHandle(handle);
			if (bResult != FALSE)
			{
				ret = true;				
			}						
		}
	}

	if (ret)
	{
		if (tModificationTime != NULL && !ConvertFileTimeToTime(findData.ftLastWriteTime, *tModificationTime))
			ret = false;
		if (tCreationTime != NULL && !ConvertFileTimeToTime(findData.ftCreationTime, *tCreationTime))
			ret = false;
		if (tLastAccessTime != NULL && !ConvertFileTimeToTime(findData.ftLastAccessTime, *tLastAccessTime))
			ret = false;
	}

	if (!ret)
	{
		time_t tNow = time(NULL);
		if (tModificationTime != NULL && *tModificationTime <= 0)
		{
			*tModificationTime = tNow;
		}
		if (tCreationTime != NULL && *tCreationTime <= 0)
		{
			*tCreationTime = tNow;
		}
		if (tLastAccessTime != NULL && *tLastAccessTime <= 0)
		{
			*tLastAccessTime = tNow;
		}
	}
	return ret;
}

bool ZipPlatform::SetFileTimes(LPCTSTR lpFileName, const time_t* tModificationTime, const time_t* tCreationTime, const time_t* tLastAccessTime)
{
	HANDLE handle = ::CreateFile(CZipPathComponent::AddPrefix(lpFileName), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	bool ret = ZipPlatform::SetFileTimes(handle, tModificationTime, tCreationTime, tLastAccessTime);	
	CloseHandle(handle);
	return ret;
}


bool ZipPlatform::SetFileTimes(HANDLE handle, const time_t* tModificationTime, const time_t* tCreationTime, const time_t* tLastAccessTime)
{	
	ZFILETIME ftModTime = {0};
	ZFILETIME* pftModTime;
	if (tModificationTime != NULL)
	{
		ZipPlatform::ConvertTimeToFileTime(*tModificationTime, ftModTime);
		pftModTime = &ftModTime;
	}
	else
	{
		pftModTime = NULL;
	}
	ZFILETIME ftCreationTime = {0};
	ZFILETIME* pftCreationTime;
	if (tCreationTime != NULL)
	{
		ZipPlatform::ConvertTimeToFileTime(*tCreationTime, ftCreationTime);
		pftCreationTime = &ftCreationTime;
	}
	else
	{
		pftCreationTime = NULL;
	}
	ZFILETIME ftLastAccessTime = {0};
	ZFILETIME* pftLastAccessTime;
	if (tLastAccessTime != NULL)
	{
		ZipPlatform::ConvertTimeToFileTime(*tLastAccessTime, ftLastAccessTime);
		pftLastAccessTime = &ftLastAccessTime;
	}
	else
	{
		pftLastAccessTime = NULL;
	}
	
	return ::SetFileTime(handle, pftCreationTime, pftLastAccessTime , pftModTime) != 0;
}

bool ZipPlatform::ChangeDirectory(LPCTSTR lpDirectory)
{
	return _tchdir(lpDirectory) == 0; // returns 0 if ok
}

int ZipPlatform::FileExists(LPCTSTR lpszName)
{
	DWORD attributes = ::GetFileAttributes(CZipPathComponent::AddPrefix(lpszName));
	if (attributes == INVALID_FILE_ATTRIBUTES)
		return 0;
	return ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ? -1 : 1;
}

ZIPINLINE  bool ZipPlatform::IsDriveRemovable(LPCTSTR lpszFilePath)
{
	CZipPathComponent zpc(lpszFilePath);
	return ::GetDriveType(zpc.GetFileDrive()) == DRIVE_REMOVABLE;
}

ZIPINLINE  bool ZipPlatform::SetVolLabel(LPCTSTR lpszPath, LPCTSTR lpszLabel)
{
	CZipPathComponent zpc(lpszPath);
	CZipString szDrive = zpc.GetFileDrive();
	CZipPathComponent::AppendSeparator(szDrive);
	return ::SetVolumeLabel(szDrive, lpszLabel) != 0;
}

ZIPINLINE void ZipPlatform::AnsiOem(CZipAutoBuffer& buffer, bool bAnsiToOem)
{
#ifdef _ZIP_SAFE_WINDOWS_API
	UINT cpIn, cpOut;
	if (bAnsiToOem)
	{
		cpIn = CP_ACP;
		cpOut = CP_OEMCP;
	}
	else
	{
		cpIn = CP_OEMCP;
		cpOut = CP_ACP;
	}

	CZipAutoBuffer interBuffer;

	int size = buffer.GetSize();
	// iLen doesn't include terminating character
	int iLen = ::MultiByteToWideChar(cpIn, MB_PRECOMPOSED, buffer, size, NULL, 0);
	if (iLen <= 0)
		return;
	interBuffer.Allocate(iLen * sizeof(wchar_t));
	LPWSTR lpszWide = (LPWSTR)(char*)interBuffer;
	iLen = ::MultiByteToWideChar(cpIn, MB_PRECOMPOSED, buffer, size, lpszWide, iLen);
	ASSERT(iLen != 0);

	// iLen does not include terminating character
	size = ::WideCharToMultiByte(cpOut, 0, lpszWide, iLen, NULL, 0, NULL, NULL);
	if (size <= 0)
		return;
	buffer.Allocate(size);
	size = ::WideCharToMultiByte(cpOut, 0, lpszWide, iLen, buffer, size, NULL, NULL);
	ASSERT(size != 0);
#else
	if (bAnsiToOem)
		::CharToOemBuffA(buffer, buffer, buffer.GetSize());
	else
		::OemToCharBuffA(buffer, buffer, buffer.GetSize());
#endif
}

ZIPINLINE  bool ZipPlatform::RemoveFile(LPCTSTR lpszFileName, bool bThrow, int iMode)
{
	if ((iMode & ZipPlatform::fomRemoveReadOnly) != 0)
	{
		DWORD attr;
		if (ZipPlatform::GetFileAttr(lpszFileName, attr)
			&& (ZipCompatibility::GetAsInternalAttributes(attr, ZipPlatform::GetSystemID()) & ZipCompatibility::attROnly) != 0)
		{
			ZipPlatform::SetFileAttr(lpszFileName, ZipPlatform::GetDefaultAttributes());
		}
	}
#ifdef SHFileOperation
	if ((iMode & ZipPlatform::fomRecycleBin) == 0)
	{
#endif
		if (::DeleteFile((LPTSTR)(LPCTSTR)CZipPathComponent::AddPrefix(lpszFileName, false)))		
			return true;
#ifdef SHFileOperation
	}
	else
	{
		CZipString file = lpszFileName;
#if defined _UNICODE && _MSC_VER >= 1400
		if (file.GetLength() >= MAX_PATH)
		{
			// cannot prefix for SHFileOperation, use short path
			CZipString temp = CZipPathComponent::AddPrefix(lpszFileName, false);
			DWORD size = ::GetShortPathName(temp, NULL, 0);
			if (size != 0)
			{
				size = ::GetShortPathName(temp, file.GetBuffer(size), size);
				file.ReleaseBuffer();
			}			
			if (size == 0)
			{
				if (bThrow)
					CZipException::Throw(CZipException::notRemoved, lpszFileName);
				return false;
			}			
			// GetShortPathName keeps the prefix - remove it
			int prefixLength = CZipPathComponent::IsPrefixed(file);
			if (prefixLength > 0)
			{
				file = file.Mid(prefixLength);
			}
		}
#endif
		int length = file.GetLength();
		CZipAutoBuffer buffer((length + 2) * sizeof(TCHAR), true); // double NULL is required
		memcpy(buffer, (LPCTSTR)file, length * sizeof(TCHAR));
		SHFILEOPSTRUCT shFileOp = {0};
		shFileOp.wFunc = FO_DELETE;
		shFileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI; 
		shFileOp.pFrom = (LPCTSTR)(char*)buffer;
		if (SHFileOperation(&shFileOp) == 0 && !shFileOp.fAnyOperationsAborted)
			return true;
	}
#endif
	if (bThrow)
		CZipException::Throw(CZipException::notRemoved, lpszFileName);
	return false;		
}

ZIPINLINE  bool ZipPlatform::RenameFile( LPCTSTR lpszOldName, LPCTSTR lpszNewName, bool bThrow)
{
	if (!::MoveFile((LPTSTR)(LPCTSTR)CZipPathComponent::AddPrefix(lpszOldName, false), 
		(LPTSTR)(LPCTSTR)CZipPathComponent::AddPrefix(lpszNewName, false)))
		if (bThrow)
			CZipException::Throw(CZipException::notRenamed, lpszOldName);
		else 
			return false;
	return true;

}

ZIPINLINE  bool ZipPlatform::IsDirectory(DWORD uAttr)
{
	return (uAttr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

ZIPINLINE  bool ZipPlatform::CreateNewDirectory(LPCTSTR lpDirectory)
{
	return ::CreateDirectory(CZipPathComponent::AddPrefix(lpDirectory), NULL) != 0;
}

ZIPINLINE  DWORD ZipPlatform::GetDefaultAttributes()
{
	return FILE_ATTRIBUTE_ARCHIVE;
}

ZIPINLINE  DWORD ZipPlatform::GetDefaultDirAttributes()
{
	return FILE_ATTRIBUTE_DIRECTORY;
}

ZIPINLINE  int ZipPlatform::GetSystemID()
{
	return ZipCompatibility::zcDosFat;
}

ZIPINLINE bool ZipPlatform::GetSystemCaseSensitivity()
{
	return false;
}


#ifdef _UNICODE

#ifdef _ZIP_UNICODE_NORMALIZE
	#pragma comment(lib, "Normaliz.lib")
#endif

int ZipPlatform::WideToMultiByte(LPCWSTR lpszIn, CZipAutoBuffer &szOut, bool bAddNull, UINT uCodePage)
{
	size_t wideLen = wcslen(lpszIn);
	if (wideLen == 0)
	{
		szOut.Release();
		return 0;
	}

	// iLen does not include terminating character
	int iLen = ::WideCharToMultiByte(uCodePage, 0, lpszIn, (int)wideLen, szOut,
		0, NULL, NULL);
	if (iLen > 0)
	{
		szOut.Allocate(bAddNull ? iLen + 1 : iLen, true);
		iLen = ::WideCharToMultiByte(uCodePage, 0, lpszIn, (int)wideLen, szOut,
			iLen, NULL, NULL);
		ASSERT(iLen > 0);
		if (bAddNull)
		{
			szOut[iLen] = 0;
		}
		return iLen > 0 ? iLen : -1;
	}
	else // here it means error
	{
		szOut.Release();
		return -1;
	}
}
int ZipPlatform::MultiByteToWide(const char* szIn, int iInSize, CZipString& szOut, UINT uCodePage)
{	
	szOut.Empty();
	if (iInSize < 0)
	{
		iInSize = (int)std::string(szIn).length();
	}	
	if (iInSize == 0)
	{		
		return 0;
	}
	
	// iLen doesn't include terminating character
	DWORD dwFlags = uCodePage <= CP_OEMCP ? MB_PRECOMPOSED : 0;
	int iLen = ::MultiByteToWideChar(uCodePage, dwFlags, szIn, iInSize, NULL, 0);
	if (iLen > 0)
	{
		iLen = ::MultiByteToWideChar(uCodePage, dwFlags, szIn, iInSize, szOut.GetBuffer(iLen), iLen);
		szOut.ReleaseBuffer(iLen);
		ASSERT(iLen > 0);
		if (iLen == 0)
			return -1;
#ifdef _ZIP_UNICODE_NORMALIZE
		// if there is a problem with compilation here, you may need uncomment the block defining WINVER = 0x0600 at the bottom of _features.h file
		if (::IsNormalizedString(NormalizationC, szOut, iLen + 1) != FALSE)
			return iLen;		
		int iNewLen = ::NormalizeString(NormalizationC, szOut, iLen, NULL, 0);
		if (iNewLen <= 0)
		{
			return iLen;
		}
		CZipString szNormalized;
		iNewLen = ::NormalizeString(NormalizationC, szOut, iLen, szNormalized.GetBuffer(iNewLen), iNewLen);		
		if (iNewLen <= 0)
		{
			szNormalized.ReleaseBuffer(0);
			return iLen;
		}
		szNormalized.ReleaseBuffer(iNewLen);
		szOut = szNormalized;
		return iNewLen;
#else
		return iLen;
#endif
	}
	else
	{		
		return -1;
	}	
}
#endif

#if defined _ZIP_IMPL_STL || _ZIP_FILE_IMPLEMENTATION == ZIP_ZFI_STL

#if _MSC_VER > 1000
	#pragma warning( push )
	#pragma warning (disable : 4702) // unreachable code
#endif


bool ZipPlatform::TruncateFile(int iDes, ULONGLONG uSize)
{	
#if (_MSC_VER >= 1400)
	return _chsize_s(iDes, uSize) == 0;
#else
	if (uSize > _I64_MAX)
		CZipException::Throw(CZipException::tooBigSize);
	else
	{
		HANDLE handle = (HANDLE)GetFileSystemHandle(iDes);
		ULARGE_INTEGER li;
		li.QuadPart = uSize;
		li.LowPart = ::SetFilePointer(handle, li.LowPart, (LONG*)&li.HighPart, FILE_BEGIN);
		if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
			return false;
		return ::SetEndOfFile(handle) != FALSE;
	}
	return false; // for the compiler
#endif

}

#if _MSC_VER > 1000
	#pragma warning( pop )
#endif


int ZipPlatform::OpenFile(LPCTSTR lpszFileName, UINT iMode, int iShareMode)
{
	switch (iShareMode)
	{
	case (CZipFile::shareExclusive):
		iShareMode = SH_DENYRW;
		break;
	case (CZipFile::shareDenyRead):
		iShareMode = SH_DENYRD;
		break;
	case (CZipFile::shareDenyWrite):
		iShareMode = SH_DENYWR;
		break;
	default:
		iShareMode = SH_DENYNO;
	}
#if _MSC_VER >= 1400	
	int handle;
	if (_tsopen_s(&handle, lpszFileName, iMode, iShareMode, S_IREAD | S_IWRITE /*required only when O_CREAT mode*/) != 0)
		return -1;
	else
		return handle;
#else
	return  _tsopen(lpszFileName, iMode, iShareMode, S_IREAD | S_IWRITE /*required only when O_CREAT mode*/);
#endif
	
}

bool ZipPlatform::FlushFile(int iDes)
{
	return _commit(iDes) == 0;
}

intptr_t ZipPlatform::GetFileSystemHandle(int iDes)
{
	return _get_osfhandle(iDes);
}


#endif // _ZIP_IMPL_STL

#endif // _ZIP_SYSTEM_WIN
