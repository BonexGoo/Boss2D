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

#if defined _MSC_VER && _MSC_VER < 1300
	// STL warnings
	#pragma warning (push, 3) 
#endif

#include "DirEnumerator.h"
#include "FileFilter.h"
#include "ZipAutoBuffer.h"

#include <queue>

#if defined __GNUC__ && !defined __MINGW32__
	#include <sys/stat.h>
	#include <dirent.h>	
#else
	#include <io.h>
	#ifdef __BORLANDC__
		#ifndef _tfindfirsti64
			#define _tfindfirsti64 __tfindfirsti64
		#endif
		#ifndef _tfindnexti64
			#define _tfindnexti64 __tfindnexti64
		#endif
		#ifndef _tfinddatai64_t
			#define _tfinddatai64_t __tfinddatai64_t
		#endif		
	#endif	
#endif

namespace ZipArchiveLib
{

#if defined __GNUC__ && !defined __MINGW32__
	#define ZIP_ENUMERATOR_FOR_GNUC
#endif

#ifdef ZIP_ENUMERATOR_FOR_GNUC
	class CFindHandleReleaser
	{
		DIR* m_handle;
	public: 
		CFindHandleReleaser(DIR* handle)
			:m_handle(handle)
		{
		}				
		~CFindHandleReleaser()
		{
			closedir(m_handle);
		}
	};
#else
	class CFindHandleReleaser
	{
#if _MSC_VER > 1200
		intptr_t m_handle;
#else
		long m_handle;
#endif
	public: 
#if _MSC_VER > 1200
		CFindHandleReleaser(intptr_t handle)
#else
		CFindHandleReleaser(long handle)
#endif		
			:m_handle(handle)
		{
		}				
		~CFindHandleReleaser()
		{
			_findclose(m_handle);
		}
	};
#endif


bool CDirEnumerator::Start(CFileFilter& filter)
{	
	OnEnumerationBegin();
	std::queue<CZipString> dirs;
	dirs.push(CZipString(m_lpszDirectory));
	bool ret = true;
	do
	{
		m_szCurrentDirectory = dirs.front();
		dirs.pop();
		CZipPathComponent::AppendSeparator(m_szCurrentDirectory);
		EnterDirectory();

#ifdef ZIP_ENUMERATOR_FOR_GNUC
		_ZIP_WIDE_TO_MULTIBYTE(m_szCurrentDirectory, currentDirectory)
                              
		DIR* dp = opendir(currentDirectory);
		if (dp)
		{
			CFindHandleReleaser handleReleaser(dp);
			while (true)
			{
				struct dirent* entry = readdir(dp);
				if (!entry)
					break;	
				LPCTSTR name;
        #ifdef _UNICODE
                CZipString nameWide;                                
                ZipPlatform::MultiByteToWide(entry->d_name, -1, nameWide, 0);
                name = nameWide;
        #else
                name = entry->d_name;
        #endif                                
				CZipString path(m_szCurrentDirectory + name);
				_ZIP_WIDE_TO_MULTIBYTE(path, pathMultiByte)
                                
	#if !defined __APPLE__ && !defined __CYGWIN__
				struct stat64 sStats;
				if (stat64(pathMultiByte, &sStats) == -1)
	#else
				struct stat sStats;
				if (stat(pathMultiByte, &sStats) == -1)
	#endif
					continue;
				
				CFileInfo info;
				info.m_uAttributes = sStats.st_mode;				
				
#else
		CZipString szFullFileName = m_szCurrentDirectory + _T("*");
		
		_tfinddatai64_t ffInfo;
#if _MSC_VER > 1200
		intptr_t hFile;
#else
		long hFile;
#endif
		if( (hFile = _tfindfirsti64( (LPTSTR)(LPCTSTR)szFullFileName, &ffInfo )) != -1L )
		{
			CFindHandleReleaser handleReleaser(hFile);
			do
			{				
				LPCTSTR name = ffInfo.name;
				CFileInfo info;
				info.m_uAttributes = ffInfo.attrib;				
#endif				
				bool isDir;
				if (ZipPlatform::IsDirectory(info.m_uAttributes))
				{
					if (!m_bRecursive || IsDots(name))
						continue;					
					isDir = true;
				}
				else
					isDir = false;									

#ifdef ZIP_ENUMERATOR_FOR_GNUC
				info.m_uSize  = (ZIP_FILE_USIZE)sStats.st_size;
				info.m_tCreationTime = sStats.st_ctime;
				info.m_tModificationTime = sStats.st_mtime;
				info.m_tLastAccessTime = sStats.st_atime;				
#else
				info.m_uSize = (ZIP_FILE_USIZE)ffInfo.size;
				info.m_tCreationTime = ffInfo.time_create;
				info.m_tModificationTime = ffInfo.time_write;
				info.m_tLastAccessTime = ffInfo.time_access;	
				CZipString path(m_szCurrentDirectory + ffInfo.name);
#endif				
				
				if (isDir)
				{
					bool bAllow;
					if (filter.HandlesFile(info))
						bAllow = filter.Evaluate(path, name, info) && Process(path, info);
					else
						// examine directory, if the filter cannot decide
						bAllow = true;

					if (bAllow)
						dirs.push(path);
				}
				else 
				{
					bool bAllow;
					if (filter.HandlesFile(info))
						bAllow = filter.Evaluate(path, name, info);
					else
						// skip file, if the filter cannot decide
						bAllow = false;

					if (bAllow && !Process(path, info))
					{
						ret = false;
						break;
					}
				}

#ifdef ZIP_ENUMERATOR_FOR_GNUC
			}			
		}		
#else
			}
			while (_tfindnexti64(hFile, &ffInfo) == 0L);			
		}		
#endif		
		ExitDirectory();
	}
	while(!dirs.empty() && ret);
	OnEnumerationEnd(ret);
	return ret;
}

bool CDirEnumerator::IsDots(LPCTSTR lpszName)
{
	CZipString name(lpszName);
	return name.Compare(_T(".")) == 0 || name.Compare(_T("..")) == 0;
}

} // namespace

#if defined _MSC_VER && _MSC_VER < 1300
	// STL warnings 
	#pragma warning (pop) 
#endif

