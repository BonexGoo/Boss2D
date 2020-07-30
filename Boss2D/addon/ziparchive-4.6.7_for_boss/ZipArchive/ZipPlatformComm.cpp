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
#include "ZipFile.h"
#include "ZipPlatform.h"
#include "ZipException.h"
#include "ZipAutoBuffer.h"

using namespace ZipPlatform;

bool ZipPlatform::ForceDirectory(LPCTSTR lpDirectory)
{
	ASSERT(lpDirectory);
	CZipString szDirectory = lpDirectory;
	szDirectory.TrimRight(CZipPathComponent::m_cSeparator);
	CZipPathComponent zpc(szDirectory);
	if ((zpc.GetFilePath().Compare((LPCTSTR)szDirectory)) == 0 ||
		(FileExists(szDirectory) == -1))
		return true;
	if (!ForceDirectory(zpc.GetFilePath()))
		return false;
	if (!CreateNewDirectory(szDirectory))
		return false;
	return true;
}

bool ZipPlatform::GetFileSize(LPCTSTR lpszFileName, ZIP_SIZE_TYPE& dSize)
{
	CZipFile f;
	if (!f.Open(lpszFileName, CZipFile::modeRead | CZipFile::shareDenyNone, false))
		return false;
	bool ret;
	try
	{
		ZIP_FILE_USIZE size = f.GetLength();
		// the file may be too large if zip64 is not enabled
		ret = size <= ZIP_SIZE_TYPE(-1);
		if (ret)
			dSize = (ZIP_SIZE_TYPE)size;
	}
#ifdef _ZIP_IMPL_MFC
	catch(CZipBaseException* e)
	{
		e->Delete();
		ret = false;
	}
#else
	catch(CZipBaseException&)
	{
		ret = false;
	}
#endif

	try
	{
		f.Close();
	}
#ifdef _ZIP_IMPL_MFC
	catch(CZipBaseException* e)
	{
		e->Delete();
	}
#else
	catch(CZipBaseException&)
	{
	}
#endif

	return ret;	
}

void ZipPlatform::ConvertTimeToFileTime(const time_t& ttime, ZFILETIME& fileTime)
{
	time_t t = ttime > 0 ? ttime : time(NULL);
	LONGLONG val = ((LONGLONG)t * 10000000) + SUFFIX_I64(116444736000000000);
	fileTime.dwLowDateTime = (DWORD)(val & 0xFFFFFFFF);
    fileTime.dwHighDateTime = (DWORD)((val >> 32) & 0xFFFFFFFF);
}

bool ZipPlatform::ConvertFileTimeToTime(const ZFILETIME& fileTime, time_t& ttime)
{	
	if ( fileTime.dwLowDateTime == 0 && fileTime.dwHighDateTime == 0)
	{
		ttime = 0;
		return false;
	}
	LONGLONG val = (fileTime.dwLowDateTime & 0xFFFFFFFF);

	val |= ((LONGLONG)fileTime.dwHighDateTime << 32) & SUFFIX_I64(0xFFFFFFFF00000000);
	val -= SUFFIX_I64(116444736000000000);

	ttime = (time_t)(val / 10000000);	
	return ttime > 0;
}

#ifdef _UNICODE

int ZipPlatform::WideToMultiByte(LPCWSTR lpszIn, CZipAutoBuffer &szOut, UINT uCodePage)
{
	return WideToMultiByte(lpszIn, szOut, false, uCodePage);
}

int ZipPlatform::MultiByteToWide(const CZipAutoBuffer &szIn, CZipString& szOut, UINT uCodePage)
{
	int singleLen = szIn.GetSize();	
	if (singleLen == 0)
	{
		szOut.Empty();
		return 0;
	}
	return MultiByteToWide(szIn, singleLen, szOut, uCodePage);
}

#endif

