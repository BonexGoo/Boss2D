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

#include "ZipPathComponent.h"
#include "ZipAutoBuffer.h"

const CZipString CZipPathComponent::PathPrefix =  _T("\\\\?\\unc\\");

CZipPathComponent::~CZipPathComponent()
{

}

int CZipPathComponent::IsPrefixed(const CZipString& path)
{
	int i = -1, iLen = PathPrefix.GetLength();
	int pathLen = path.GetLength();
	if (iLen > pathLen)
		iLen = pathLen;
	CZipString szPossiblePrefix = path.Left(iLen);
	szPossiblePrefix.MakeLower(); // must perform case insensitive comparison
	while (++i < iLen && szPossiblePrefix[i] == PathPrefix[i]);
	return i;
}

#if defined _UNICODE && _MSC_VER >= 1400

CZipString CZipPathComponent::AddPrefix(LPCTSTR path, bool isFolder)
{	

	CZipString ret = path;
	AddPrefix(ret, isFolder);
	return ret;
}

void CZipPathComponent::AddPrefix(CZipString& path, bool isFolder)
{	

	if (path.GetLength() >= (isFolder ? 248 : MAX_PATH))
	{
		int prefixLength = IsPrefixed(path);
		if (prefixLength < ptUnicode)
		{			
			if (prefixLength == ptUnc)
			{
				path = path.Mid(prefixLength);
				// long UNC
				path.Insert(0, PathPrefix.Left(CZipPathComponent::ptUncWin));
			}
			else
			{
				path.Insert(0, PathPrefix.Left(CZipPathComponent::ptUnicode));
			}
		}
	}
}


#else

CZipString CZipPathComponent::AddPrefix(LPCTSTR path, bool)
{	
	return path;
}

void CZipPathComponent::AddPrefix(CZipString&, bool)
{	

}

#endif

void CZipPathComponent::SetFullPath(LPCTSTR lpszFullPath)
{
	TCHAR szDrive[_MAX_DRIVE];
	// moving to heap to avoid C6262
	CZipAutoBuffer szDirBuffer;
	
#if defined _UNICODE && _MSC_VER >= 1400
	const int dirBufferLength = 32767;
#else
	const int dirBufferLength = _MAX_DIR;
#endif
	szDirBuffer.Allocate(dirBufferLength * sizeof (TCHAR), true);		
	TCHAR* szDir = (TCHAR*)(char*)szDirBuffer;
	TCHAR szFname[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	
	
	CZipString szTempPath(lpszFullPath);
	int i = IsPrefixed(szTempPath);	
	if (i == ptUnc || i == ptUnicode || i == ptUncWin) // unc path, Unicode path or unc path meeting windows file name conventions
	{
		m_szPrefix = szTempPath.Left(i);
		szTempPath = szTempPath.Mid(i);		
	}
	else
		m_szPrefix.Empty();
#if _MSC_VER >= 1400	
	_tsplitpath_s(szTempPath, szDrive, _MAX_DRIVE, szDir, dirBufferLength, szFname, _MAX_FNAME, szExt, _MAX_EXT);
#elif defined __MINGW32__ || _MSC_VER >= 1200	
	_tsplitpath(szTempPath, szDrive, szDir, szFname, szExt);
#else 
	_tsplitpath(szTempPath, szDrive, _MAX_DRIVE, szDir, dirBufferLength, szFname, _MAX_FNAME, szExt, _MAX_EXT);
#endif
	
	m_szDrive = szDrive;
	m_szDirectory = szDir;
	
	RemoveSeparatorsLeft(m_szDirectory);
	RemoveSeparators(m_szDirectory);
	SetExtension(szExt);
	m_szFileTitle = szFname;
}


CZipString CZipPathComponent::GetNoDrive() const
{
	CZipString szPath = m_szDirectory;
	CZipString szFileName = GetFileName();
	if (!szFileName.IsEmpty() && !szPath.IsEmpty())
		szPath += m_cSeparator;

	szPath += szFileName;
	return szPath;	
}

#endif // _ZIP_SYSTEM_WIN
