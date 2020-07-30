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

#ifndef ZIPARCHIVE_ZIPFILE_DOT_H
	#error Do not include this file directly. Include ZipFile.h instead
#endif

#if _MSC_VER > 1000 && defined ZIP_HAS_DLL
		#pragma warning (push)
		#pragma warning( disable : 4275 ) // non dll-interface used as base for dll-interface class
#endif

#include "ZipAbstractFile.h"
#include "ZipExport.h"

class ZIP_API CZipFile : public CZipAbstractFile, public CFile
{
public:
	DECLARE_DYNAMIC(CZipFile)
	void Flush(){CFile::Flush();}	
	CZipString GetFilePath() const
	{		
		try
		{
			// it throws an exception when working on an offline file
			return CFile::GetFilePath();
		}
		catch (CException* e)
		{
			e->Delete();
			return this->m_strFileName;
		}
	}
	
	ZIP_FILE_USIZE GetPosition() const {return CFile::GetPosition() ;}
	void SetLength(ZIP_FILE_USIZE nNewLen) {CFile::SetLength(nNewLen);}	
	ZIP_FILE_USIZE Seek(ZIP_FILE_SIZE lOff , int nFrom){return CFile::Seek(lOff, nFrom);}
	ZIP_FILE_USIZE GetLength() const {return CFile::GetLength();}
	bool HasFilePath() const
	{
		return true;
	}

	size_t Read(void *lpBuf, size_t nCount){return (size_t)CFile::Read(lpBuf, (UINT)nCount);}
	void Write(const void* lpBuf, size_t nCount){CFile::Write(lpBuf, (UINT)nCount);}
	bool Open( LPCTSTR lpszFileName, UINT nOpenFlags, bool bThrowExc)
	{
		CFileException* e = new CFileException;
		bool bRet = CFile::Open(lpszFileName, nOpenFlags, e) != 0;
		if (!bRet && bThrowExc)
			throw e;
		e->Delete();
		return bRet;
	}
	CZipFile();
	bool IsClosed() const 
	{
		return m_hFile == CFile::hFileNull;
	}


	CZipFile( LPCTSTR lpszFileName, UINT nOpenFlags ):CFile(lpszFileName, nOpenFlags)
	{
	}

	void Close( )
	{
 		if (!IsClosed())
			CFile::Close();
	}
	operator HANDLE();
	virtual ~CZipFile();

};

#if (_MSC_VER > 1000) && (defined ZIP_HAS_DLL)
	#pragma warning (pop)	
#endif

