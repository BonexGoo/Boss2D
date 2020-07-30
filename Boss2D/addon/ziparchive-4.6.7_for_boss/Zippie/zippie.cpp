/////////////////////////////////////////////////////////////////////////////////
// zippie.cpp : Defines the entry point for the console application.
// STL program that uses the ZipArchive Library
//  
// Copyright (c) 2000 - 2019 Artpol Software - Tadeusz Dracz
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// For the licensing details of GNU General Public License 
// please refer to the "License GNU GPL.txt" file.
//
// Web Site: http://www.artpol-software.com
////////////////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
	// STL warnings
	#pragma warning (disable : 4786) // 'identifier' : identifier was truncated to 'number' characters in the debug information
#endif

#if defined __GNUC__ && !defined __MINGW32__
	#include "ZipArchive.h"
	#include "ZipPlatform.h"
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
	#include <fnmatch.h>
	#include <unistd.h>
	#include <errno.h>
	#define _tprintf printf
	#define _tfopen fopen
	#define _tcscmp strcmp
	#define _tcscspn strspn
	#define _ttoi atoi	
#else
	#include "../ZipArchive/ZipArchive.h"
	#include "../ZipArchive/ZipPlatform.h"
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
		#include <vcl.h>
//#ifdef _DEBUG
//		#pragma comment (lib, "../ZipArchive/Debug/ZipArchive.lib")
//#else
//		#pragma comment (lib, "../ZipArchive/Release/ZipArchive.lib")
//#endif
	#endif	
#endif
#include "CmdLine.h"
#include <stdlib.h>
#include <list>
#include <time.h>
#include <stdio.h>

#ifndef __MINGW32__
	#include <locale>
#endif	

#ifdef _ZIP_SYSTEM_WIN
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef _WIN32_WINNT            
	#define _WIN32_WINNT 0x0501
#endif
#endif

#if (defined _UNICODE || defined _ZIP_SYSTEM_LINUX) && defined _ZIP_UNICODE
	#define _ZIPPIE_USES_UNICODE
#endif

//#include <vld.h>
ZIPSTRINGCOMPARE pZipComp;

struct CZipAddFileInfo
{
	CZipAddFileInfo(const CZipString& szFilePath, const CZipString& szFileNameInZip)
		:m_szFilePath(szFilePath),	m_szFileNameInZip(szFileNameInZip)
	{
		int is = szFileNameInZip.GetLength();
		m_iSeparators = 0;
		for (int i = 0; i < is; i++)
			if (CZipPathComponent::IsSeparator(szFileNameInZip[i]))
				m_iSeparators++;
	}
	CZipString m_szFilePath, m_szFileNameInZip;
	
	bool CheckOrder(const CZipString& sz1, const CZipString& sz2, 
		int iSep1, int iSep2, bool bCheckTheBeginning = false) const
	{
		if (iSep1)
		{
			if (iSep2)
			{
				if (iSep1 == iSep2)
					return (sz1.*pZipComp)(sz2) < 0;

				if (bCheckTheBeginning)
				{

					int is = sz1.GetLength() > sz2.GetLength() ? sz2.GetLength() : sz1.GetLength();
					int iSeparators = 0;
					// find the common path beginning
					int iLastSepPos = -1;
					for (int i = 0; i < is; i++)
					{
						CZipString sz = sz2.Mid(i, 1);
						if ((sz1.Mid(i, 1).*pZipComp)(sz) != 0) // must be Mid 'cos of case sens. here
							break;
						else if (CZipPathComponent::IsSeparator(sz[0]))
						{
							iLastSepPos = i;
							iSeparators++;
						}
					}
					
					// if necessary remove the common path beginning and check again
					if (iLastSepPos != -1)
						return CheckOrder(sz1.Mid(iLastSepPos), sz2.Mid(iLastSepPos), iSep1 - iSeparators, iSep2 - iSeparators);
							
				}
				return (sz1.*pZipComp)(sz2) < 0;		
			} 
			else
				return false;
		}
		else
			if (iSep2)
				return true;
			else
				return (sz1.*pZipComp)(sz2) < 0;
	}
	bool operator>(const CZipAddFileInfo& wz) const
	{
		bool b = CheckOrder(m_szFileNameInZip, wz.m_szFileNameInZip,
			m_iSeparators, wz.m_iSeparators, true);
		return b;
	}
protected:
	int m_iSeparators; // for a sorting puroposes
};


typedef list<CZipString> FILELIST;
typedef list<CZipString>::iterator FILELISTIT;
typedef list<struct CZipAddFileInfo> FILELISTADD;
typedef list<struct CZipAddFileInfo>::iterator FILELISTADDIT;

struct AddDirectoryInfo
{
	AddDirectoryInfo(FILELIST& l):m_l(l){}
	FILELIST& m_l;
	CZipString m_lpszFile;
	bool m_bRecursive;
	bool m_bAddEmpty;
};

void DisplayUsage()
{
	_tprintf(_T("\
\n\
Zippie v3.3\n\
Copyright (c) 2000 - 2019 Artpol Software - Tadeusz Dracz\n\
E-Mail: tdracz@artpol-software.com\n\
Web   : http://www.artpol-software.com\n\
\n\
This program is free software; you can redistribute it and/or\n\
modify it under the terms of the GNU General Public License\n\
as published by the Free Software Foundation; either version 2\n\
of the License, or (at your option) any later version.\n\
\n\
This is a zipping and unzipping program.\n\
It was created using ZipArchive library in the STL version.\n\
\n\
\n\
USAGE: zippie <commands and (or) switches>\n\
\n\
Notes:\n\
	- the order of commands and switches doesn't matter\n\
	- if some files have spaces inside, put them inside quotation marks\n\
\n"));
	_tprintf(_T("\
************  Default switches  ************\n\
\n\
-f <archive>[.zip]\n\
	<archive> file to create or open (can be with or without extension)\n\
	the extension .zip is added automatically if not present\n\
-st\n\
	use this switch if you're opening an existing split archive\n\
	on a removable device (or under Linux)\n\
	(doesn't matter for commands that modify the archive \n\
	e.g. add or delete commands)\n\
\n\
************  Add files to archive commands  ************\n\
\n\
-a <files>\n\
	add <files> to the <archive>; separate them with spaces;\n\
	you can use wildcards (*?)\n\
-af <file>\n\
	add files listed in <file> (one file per line);\n\
	no wildcards allowed in the files\n\
-ax <files>\n\
	prevent <files> from being added to the <archive>;\n\
	separate them with spaces; you can use wildcards (*?)\n\
-afx <file>\n\
	prevent files listed in <file> (one file per line) to be added \n\
	to the <archive>; no wildcards allowed in the files\n\
	you can use switches -a, -af, -ax, -afx simultaneously\n"));
	_tprintf(_T("\
-r\n\
	recurse subdirectories; don't include in the <archive> directories\n\
	from which no file is added; under linux put file masks into \n\
	quotation marks otherwise they will be automatically expaned by \n\
	the shell and the recursion will be one level only\n\
-re\n\
	recurse subdirectories and add all subdirectories even empty to the\n\
	<archive>; see a note above about using file masks under Linux\n\
-u\n\
	this switch tells the program to not delete the <archive> if it\n\
	already	exists (and if it is not a segmented archive), but add\n\
	the <files> to it;\n\
	if this switch is not specified, there is a new archive created\n\
	even if one with the same name already exists (it is overwritten)\n"));
	_tprintf(_T("\
-as [<path>]\n\
	(add smartly) if this switch is specified, for a segmented archive\n\
	the file is first compressed to a temporary directory to see whether \n\
	it is smaller after compression; if it is not it stored then without \n\
	a compression; (you can specify a <path> where the temporary files \n\
	will be created, if you do not specify it, they will be created in \n\
	the directory specified by the TMP system variable or if this variable\n\
	is not defined the file will be created in the current directory;\n\
	in normal archive if there is a such a situation the file is removed\n\
	from the archive and then stored again but with no compression \n\
	(the <path> argument is ignored)\n"));
#ifdef _ZIP_BZIP2
	_tprintf(_T("\
-bz\n\
	use bzip2 compression algorithm instead of deflate which is used by default\n"));
#endif
	_tprintf(_T("\
-c <number>\n\
	set the compression level (from 0 to 9);\n\
	if not specified, 5 is assumed\n\
	0 means no compression - only storing\n\
-v <size>\n\
	create a segmented archive\n\
	if <size> is 0 create a spanning archive,\n\
	if <size> is greater than 0 create a split archive\n\
\n"));
		_tprintf(_T("\
************  Extract commands  ************\n\
\n\
-xp <path>\n\
	it specifies the path the files	will be extracted to; if not present,\n\
	the current path is assumed\n\
-x  <files>\n\
	extract <files> to the destination <path>;\n\
	separate them with spaces; wildcards are allowed\n\
-xr <numbers>\n\
	extract files with the given <numbers> to the destination <path>\n\
	separate number with spaces; to specify a range of numbers \n\
	put between them a hyphen without spaces e.g. 3-6; \n\
	put before an exclamation mark to exclude the numbers from the set\n\
	(e.g. 2-20 !6-8 !10);\n\
	(use -lr command to list the files with numbers)\n\
-xf <file>\n\
	extract files listed in <file> (one file per line)\n\
	to the destination <path>; wildcards are allowed\n\
-xa \n\
	extract all files from the archive to the <path>\n\
\n\
	you can use switches -x , -xr and -xf (and even -xa) simultaneously\n\
\n"));
		_tprintf(_T("\
************  Rename command  ************\n\
-ren <index> <new name>\n\
	Rename the file with the given <index> to <new name>\n\
\n"));
	_tprintf(_T("\
************  Delete commands  ************\n\
\n\
-d  <files>\n\
	delete <files> separate them with spaces; wildcards are allowed\n"));
		_tprintf(_T("\
-dr <numbers>\n\
	delete files with the given <numbers>\n\
	separate number with spaces; to specify a range of numbers \n\
	put between them a hyphen without spaces e.g. 3-6 \n\
	put before an exclamation mark to exclude the numbers from the set\n\
	(e.g. 2-20 !6-8 !10);\n\
	(use -lr command to list the files with numbers)\n\
-df <file>\n\
	delete files listed in <file> (one file per line)\n\
	wildcards are allowed\n\
-da \n\
	delete all the files from the archive\n\
\n\
	you can use switches -d , -dr and -df (and even -da) simultaneously\n\
\n"));
	_tprintf(_T("\
************  Other commands  ************\n\
\n\
-t\n\
	test the <archive>\n\
-tv\n\
	test all the volumes of a split archive for presence (implies -st switch)\n\
-p <password>\n\
	set the <password> when adding files to <archive>\n\
	or extracting them; the <password> is set for all the files\n\
	- you cannot set it separately for each file in one session\n\
-en <0|1|2|3>\n\
	set the encryption method when encrypting; the password\n\
	be set for the file(s) to be encrypted; the meaning of the\n\
	values is following:\n\
	0: standard PKZIP (default)\n\
	1: AES 128-bit WinZip\n\
	2: AES 192-bit WinZip\n\
	3: AES 256-bit WinZip\n\
-l\n\
	list the files inside <archive>\n\
-lr\n\
	list the files inside <archive> with the numbers\n\
-ll\n\
	list the files inside <archive> (only filenames)\n\
	when redirected to a file, it can be used then with command -xf\n\
\n"));
	_tprintf(_T("\
************  Special switches  ************\n\
\n\
-cs \n\
	enable case sensitivity when:\n\
	- searching for files inside <archive> while using the command\n\
		-x or -xf (if not specified a search is non-case-sensitive)\n\
	- adding files to <archive> and trying to avoid the same\n\
		filenames in the archive\n\
-g <comment>\n\
	Set the global <comment> of the <archive>\n\
	(cannot be used on an existing segmented archive).\n\
	If the <comment> contains spaces, put the whole <comment>\n\
	inside quotation marks.\n\
-rp <path>\n\
	set root <path> (see CZipArchive::SetRootPath() function description)\n\
-nd\n\
	skip removing duplicates when adding files; may be usefull if adding many files\n\
-nfp\n\
	the same as bFullPath set to false in functions\n\
	CZipArchive::AddNewFile() and CZipArchive::ExtractFile()\n\
	(if not present, bFullPath is assumed to be true; if -rp specified\n\
	bFullPath is always false)\n\
-in\n\
	overwrite files during extraction only if the file being extracted is newer\n\
"));
#ifdef _ZIPPIE_USES_UNICODE
_tprintf(_T("-utfe\n\
	store filenames and comments encoded as UTF8 in extra fields;\n\
	in the central and local directory	filenames are stored as usually\n\
	(default under Windows)\n\
-utf\n\
	store filenames and comments encoded as UTF8 (default under Linux/Mac OS X)\n\
"));
#endif
_tprintf(_T("\
-w\n\
	Wait for a key after finishing the work to let the user read the output\n\
	in some environments\n\
-dse\n\
	Display only errors when adding or extracting files\n\
\n\
************  Sample commands  ************\n\
\n\
zippie -f a:\\archive -a * -v 0 -as -c 9 \n\
	(create a disk spanning archive adding all the files from the current \n\
	directory, smart add is used, compression level set to maximum)\n\
\n\
zippie -f a:\\archive -xp d:\\a -x zippie.cpp -xr 2-12 !5-7 !9\n\
	(extract file zippie.cpp from the archive and the files with numbers \n\
	from 2 to 12 and the file number 15 apart from 5 to 7 and 9)\n\
\n\
zippie -f example -xa\n\
	extract all files from example.zip file to the current directory\n\
\n\
zippie -f example -ren 2 \"New Name\"\n\
	rename the file with the index 2 to \"New Name\"\n\
\n\
"));
}

char ReadKey()
{
	fflush (stdin);
	char c = (char) tolower(getchar());
	return c;
}

struct SpanCallback : public CZipSegmCallback
{
	bool Callback(ZIP_SIZE_TYPE uProgress)
	{
		_tprintf(_T("Insert disk number %d and hit ENTER to continue \n or press 'n' key followed by ENTER to abort (code = %d)\n"), m_uVolumeNeeded, m_iCode);	
		return ReadKey() != 'n';
	}
};

void FillFromFile(FILELIST& l, LPCTSTR lpszFile, bool bCheck)
{
	FILE* f;
#if _MSC_VER >= 1400
	if (_tfopen_s(&f, lpszFile, _T("rt")) != 0)
		f = NULL;
#else
	f = _tfopen(lpszFile, _T("rt"));
#endif
	
	if (!f)
	{
		_tprintf(_T("File %s could not be opened\n"), lpszFile);
		return;
	}
	fseek(f, 0, SEEK_END);
	size_t iSize = ftell(f);
	fseek(f, 0, SEEK_SET);
	CZipAutoBuffer buf((DWORD)(iSize + 1));
	iSize = fread(buf, 1, iSize, f);
	fclose(f);
	char* sEnd = (char*)buf + iSize;
	char* sBeg = buf;
	for (char* pos = buf; ; pos++)
	{
		bool bEnd = pos == sEnd; // there may be no newline at the end
		if (strncmp(pos, "\n", 1) == 0 || bEnd)
		{
			*pos = '\0';
			int length = (int)(pos - sBeg + 1);
			CZipAutoBuffer temp(length);
			memcpy(temp, sBeg, length);
			CZipString s;
			ZipCompatibility::ConvertBufferToString(s, temp, CP_ACP);
			
			s.TrimLeft(_T(" "));
			s.TrimRight(_T(" "));
			if (!s.IsEmpty() && (!bCheck || ZipPlatform::FileExists(s) != 0))
				l.push_back(s);
			if (bEnd)
				break;
			sBeg = pos + 1;			
		}
	}
}

bool IsDots(LPCTSTR lpsz)
{
	return _tcscmp(lpsz, _T(".")) == 0 || _tcscmp(lpsz, _T("..")) == 0;
}

void AddDirectory(CZipString szPath, struct AddDirectoryInfo& info, bool bDoNotAdd)
{
	if (!szPath.IsEmpty())
		CZipPathComponent::AppendSeparator(szPath);

	bool bPathAdded = info.m_bAddEmpty || bDoNotAdd;
	if (info.m_bAddEmpty && !szPath.IsEmpty() && !bDoNotAdd)
		info.m_l.push_back(szPath);

#if defined __GNUC__ && !defined __MINGW32__
	DIR* dp = opendir(szPath.IsEmpty() ? _T(".") : szPath);
	if (!dp)
		return;
	struct dirent* entry;
	while (entry = readdir(dp))
	{
		struct stat sStats;
		CZipString szFullFileName = szPath + entry->d_name;
		if (stat(szFullFileName, &sStats) == -1)
			continue;
		if (S_ISDIR(sStats.st_mode))
		{
			if (info.m_bRecursive)
			{
				if (IsDots(entry->d_name))
					continue;
				
				AddDirectory(szFullFileName, info, false);
			}
		}
		else if (fnmatch(info.m_lpszFile, entry->d_name, FNM_NOESCAPE |FNM_PATHNAME) == 0)
		{
			if (!bPathAdded)
			{
				if (!szPath.IsEmpty())
					info.m_l.push_back(szPath);
				bPathAdded = true;
			}
			info.m_l.push_back(szPath + entry->d_name);
		}
	}
	closedir(dp);

#else
    CZipString szFullFileName = szPath + info.m_lpszFile;

	intptr_t hFile;
	_tfinddatai64_t c_file;
	if( (hFile = _tfindfirsti64( (LPTSTR)(LPCTSTR)szFullFileName, &c_file )) != -1L )
	{
		do
		{
			if (!(c_file.attrib & FILE_ATTRIBUTE_DIRECTORY))
			{
				// add it when the first file comes
				if (!bPathAdded)
				{
					if (!szPath.IsEmpty())
						info.m_l.push_back(szPath);
					bPathAdded = true;
				}
				info.m_l.push_back(szPath + c_file.name);
			}
		}
		while (_tfindnexti64(hFile, &c_file) == 0L);
	}
	_findclose(hFile);

	if (info.m_bRecursive)
	{
		szFullFileName = szPath + _T("*");
		if( (hFile = _tfindfirsti64( (LPTSTR)(LPCTSTR)szFullFileName, &c_file )) != -1L )
		{
			do
			{
				if (c_file.attrib & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (IsDots(c_file.name))
						continue;
					szFullFileName = szPath + c_file.name;
					AddDirectory(szFullFileName, info, false);
				}
			}
			while (_tfindnexti64(hFile, &c_file) == 0L);
		}
		_findclose(hFile);		
	}
#endif
}
 
void ExpandFile(FILELIST& l, LPCTSTR lpszPath, 
			 	bool bRecursive, bool bAddEmpty, bool bFullPath)
{
// check if we need to expand it
//         size_t pos = strcspn(lpszFile, "*?");
//         if (pos == strlen(lpszFile))
//         {
//                 l.push_back(lpszFile);
//                 return;
//         }

	CZipPathComponent zpc(lpszPath);
	CZipString szDir = zpc.GetFilePath();
// 	if (szDir.IsEmpty())
// 		if (!ZipPlatform::GetCurrentDirectory(szDir))
// 			return;
	struct AddDirectoryInfo adi(l);
	adi.m_bAddEmpty = bAddEmpty;
	adi.m_bRecursive = bRecursive;
	adi.m_lpszFile = zpc.GetFileName();
	AddDirectory(szDir, adi, !bFullPath); // when not full path is specified for a single file with a path, do not add a directory then
}


void FindInZip(CZipArchive& zip, FILELIST& l, CZipIndexesArray& n)
{

	for (FILELISTIT it = l.begin(); it != l.end(); ++it)
		zip.FindMatches(*it, n);
}

void ProcessData(CZipArchive& zip, CCmdLine& cmd, CZipIndexesArray& vRevised, bool bExtract)
{		
		if (cmd.HasSwitch(bExtract ? _T("-xa") : _T("-da")))
		{
			ZIP_INDEX_TYPE iMax = zip.GetCount();
			for (ZIP_INDEX_TYPE i = 0; i < iMax; i++)
				vRevised.Add(i);
		}
		else
		{
			CZipIndexesArray numbers;
			CZipString temp = bExtract ? _T("-x") : _T("-d");
			int iCount = cmd.GetArgumentCount(temp);
			if (iCount > 0)
			{
				FILELIST lFiles;
				for (int i = 0; i < iCount; i++)
					lFiles.push_back(cmd.GetArgument(temp, i));
				FindInZip(zip, lFiles, numbers);
			}
			temp = bExtract ? _T("-xf") : _T("-df");
			if (cmd.GetArgumentCount(temp) > 0)
			{
				FILELIST lFiles;
				FillFromFile(lFiles, cmd.GetArgument(temp, 0), false);
				FindInZip(zip, lFiles, numbers);
			}

			temp = bExtract ? _T("-xr") : _T("-dr");
			iCount = cmd.GetArgumentCount(temp);
			CZipIndexesArray notNumbers;
			if (iCount > 0)
			{
				for (int i = 0; i < iCount; i++)
				{
					CZipString sz = cmd.GetArgument(temp, i);
					bool bNot = !sz.IsEmpty() && sz[0] == _T('!');
					CZipIndexesArray& vN = bNot ? notNumbers : numbers;
					if (bNot)
						sz.TrimLeft(_T('!'));
					size_t pos = _tcscspn(sz, _T("-"));
					if (pos == (size_t)sz.GetLength() )
					{
						int idx = _ttoi(sz) - 1;
						if (idx >= 0)
							vN.Add((ZIP_INDEX_TYPE) idx);
					}
					else
					{
						int b = _ttoi(sz.Left((int)pos));
						if (b > 0)
						{
							int e = _ttoi (sz.Mid((int)(pos + 1)));
							for (int i = b; i <= e ; i++)
								vN.Add((ZIP_INDEX_TYPE)(i - 1));
						}
					}
				}
				
			}
			ZIP_ARRAY_SIZE_TYPE iSize = notNumbers.GetSize();
			if (iSize)
			{
				for (ZIP_ARRAY_SIZE_TYPE j = 0; j < iSize; ++j)
				{
					// cannot use a decreasing loop because i may be unsigned and instead negative at the end of the loop it will be maximum positive
					ZIP_ARRAY_SIZE_TYPE i = numbers.GetSize();
					while (i > 0)
					{
						i--;
						if (numbers[i] == notNumbers[j])
							numbers.RemoveAt(i);
					}
				}
			}
			
			ZIP_INDEX_TYPE iMax = zip.GetCount();
			if (iMax > 0)
			{
				iMax--;
				for (ZIP_ARRAY_SIZE_TYPE i = 0; i < numbers.GetSize(); ++i)
				{
					ZIP_INDEX_TYPE x = numbers[i];
					if (x > iMax)
						continue;
					bool bNew = true;
					for (ZIP_ARRAY_SIZE_TYPE j = 0; j < vRevised.GetSize(); ++j)
						if (vRevised[j] == numbers[i])
						{
							bNew = false;
							break;
						}
					if (bNew)
						vRevised.Add(x);
				}
			}
		}
		

}

#ifdef __GNUC__
int main(int argc, TCHAR* argv[])
#elif  defined __BORLANDC__
int _tmain(int argc, _TCHAR* argv[])
#else
int _tmain(int argc, TCHAR* argv[])
#endif
{	
#if !defined __MINGW32__  && !defined __APPLE__
	// set the locale the same as the system locale
	// to handle local characters (non-English) properly by CZipString
	std::locale oldLocale = std::locale::global(std::locale(""));
#endif	

	int iRet = 0;	
	
	CCmdLine cmd;
#ifndef _ZIP_TRIAL
	CZipArchive zip;	
#else
	CZipArchive zip(_T("ZipArchive.license"));	
#endif
	CZipString szArchive;
	try
	{
		
		if (cmd.SplitLine(argc, argv) < 1)
			throw 0;
		if (cmd.GetArgumentCount(_T("-f")) <= 0)
			throw 0;
		int iVolumeSize = 0;
		int iMode = CZipArchive::zipOpen;
		bool bIsAdding = cmd.GetArgumentCount(_T("-a")) > 0 || cmd.GetArgumentCount(_T("-af")) > 0;
		bool bIsExtracting = cmd.GetArgumentCount(_T("-x")) > 0 || cmd.GetArgumentCount(_T("-xr")) > 0
			|| cmd.GetArgumentCount(_T("-xf")) > 0 || cmd.HasSwitch(_T("-xa"));
		bool bIsDeleting = cmd.GetArgumentCount(_T("-d")) > 0 || cmd.GetArgumentCount(_T("-dr")) > 0 
			|| cmd.GetArgumentCount(_T("-df")) > 0 || cmd.HasSwitch(_T("-da"));
		bool bIsRenaming = cmd.GetArgumentCount(_T("-ren")) == 2;

		szArchive = cmd.GetArgument(_T("-f"), 0);

		CZipPathComponent zpc(szArchive);
		if (zpc.GetFileExt().IsEmpty())
			szArchive += _T(".zip");
		bool bUpdateMode = cmd.HasSwitch(_T("-u"));
		bool bSetComment = cmd.GetArgumentCount(_T("-g")) > 0;
		bool bIsListing = cmd.HasSwitch(_T("-l")) || cmd.HasSwitch(_T("-ll")) || 
			cmd.HasSwitch(_T("-lr"));
		bool bOnlyErrors = cmd.HasSwitch(_T("-dse"));
		if (bIsAdding)
		{
			if (cmd.GetArgumentCount(_T("-v")) >= 0)
			{
				iVolumeSize = _ttoi(cmd.GetArgument(_T("-v"), 0));
				iMode = iVolumeSize == 0 ? CZipArchive::zipCreateSpan : CZipArchive::zipCreateSplit;
			}
			else
			{
				if (!bUpdateMode || !ZipPlatform::FileExists(szArchive))
					iMode = CZipArchive::zipCreate;
			}
		}
		else 
		{
			if (bIsExtracting)
			{
				iMode = CZipArchive::zipOpenReadOnly;
			}
			if (bIsExtracting || cmd.HasSwitch(_T("-t")) || bIsListing)
			{
				if (cmd.HasSwitch(_T("-st")))
				{				
					iMode = CZipArchive::zipOpenSplit;
				}
			}
			else if (cmd.HasSwitch(_T("-tv")))
				iMode = CZipArchive::zipOpenSplit;
			else if (!bSetComment && !bIsDeleting && !bIsRenaming)
				throw 0;
		}

		SpanCallback span;		
		zip.SetSegmCallback(&span);

		bool bAddEmpty = cmd.HasSwitch(_T("-re"));
		bool bRecursive = cmd.HasSwitch(_T("-r")) || bAddEmpty;
		
		bool bCaseSensitiveInZip = cmd.HasSwitch(_T("-cs"));
		pZipComp = GetCZipStrCompFunc(bCaseSensitiveInZip);
	
		zip.SetCaseSensitivity(bCaseSensitiveInZip);
		
		zip.Open(szArchive, iMode, iVolumeSize);
		if (cmd.GetArgumentCount(_T("-p")) > 0)
		{
			zip.SetPassword(cmd.GetArgument(_T("-p"), 0));
			if (cmd.GetArgumentCount(_T("-en")) > 0)
				zip.SetEncryptionMethod(_ttoi(cmd.GetSafeArgument(_T("-en"), 0, _T("0"))));
		}

		if (cmd.GetArgumentCount(_T("-rp")) > 0)
			zip.SetRootPath(cmd.GetArgument(_T("-rp"), 0));

		bool bFullPath = !cmd.HasSwitch(_T("-nfp")) && zip.GetRootPath().IsEmpty();

		bool bIsSegm = zip.GetStorage()->IsSegmented();
		if (bSetComment && !bIsSegm)
		{
			CZipString sz = cmd.GetArgument(_T("-g"), 0);
			sz.TrimLeft(_T("\""));
			sz.TrimRight(_T("\""));
			zip.SetGlobalComment(sz);
		}

		if (bIsAdding)
		{
			if (bUpdateMode && bIsSegm)
			{
				_tprintf(_T("Cannot update an existing disk spanning archive\n"));
				zip.Close();
				return 1;
			}
			int iLevel = _ttoi(cmd.GetSafeArgument(_T("-c"), 0, _T("5")));
			int iSmartLevel;
			if (cmd.HasSwitch(_T("-as")))
			{
				iSmartLevel = CZipArchive::zipsmSmartest;
				zip.SetTempPath(cmd.GetSafeArgument(_T("-as"), 0, _T("")));
			}
			else
				iSmartLevel = CZipArchive::zipsmSafeSmart;

#ifdef _ZIP_BZIP2
			if (cmd.HasSwitch(_T("-bz")))
				zip.SetCompressionMethod(CZipCompressor::methodBzip2);
#endif
			FILELIST lFiles;
			int iCount = cmd.GetArgumentCount(_T("-a"));
			if (iCount > 0)
			{
				for (int i = 0; i < iCount; i++)
				{
					ExpandFile(lFiles, cmd.GetArgument(_T("-a"), i), bRecursive, bAddEmpty, bFullPath);
				}
			}
			
			iCount = cmd.GetArgumentCount(_T("-af"));
			if (iCount > 0)
				FillFromFile(lFiles, cmd.GetArgument(_T("-af"), 0), true);

			FILELIST excl;

			iCount = cmd.GetArgumentCount(_T("-ax"));
			if (iCount > 0)
			{
				for (int i = 0; i < iCount; i++)
				{
					ExpandFile(excl, cmd.GetArgument(_T("-ax"), i), bRecursive, bAddEmpty, bFullPath);
				}
			}
			
			iCount = cmd.GetArgumentCount(_T("-afx"));
			if (iCount > 0)
				FillFromFile(excl, cmd.GetArgument(_T("-afx"), 0), true);

			FILELISTADD rev;
			for (FILELISTIT it = lFiles.begin(); it != lFiles.end(); ++it)				
			{
				// that is how the filename will look in the archive
				CZipString sz = zip.PredictFileNameInZip(*it, bFullPath);
				if (!sz.IsEmpty())
				{
					bool bAdd = true;
					for (FILELISTIT itt = excl.begin(); itt != excl.end(); ++itt)
					{
						if (!((*itt).*pZipComp)(*it))
						{
							bAdd = false;
							break;
						}
					}
					if (bAdd)
						rev.push_back(CZipAddFileInfo(*it, sz));
				}
			}
			lFiles.clear();
			excl.clear();
	
			if (!cmd.HasSwitch(_T("-nd")))
			{
				// remove duplicates
				FILELISTADDIT it1;
				for (it1 = rev.begin(); it1 != rev.end();)
				{
					bool bErase = false;
					FILELISTADDIT it2 = it1;
					for (++it2; it2 != rev.end(); ++it2)
					{					
						int x = ((*it1).m_szFileNameInZip.*pZipComp)((*it2).m_szFileNameInZip);
						if (x == 0)
						{
							bErase = true;
							break;
						}
					}
					if (bErase)
						rev.erase(it1++);
					else
						++it1;
				}
			}
					
			
			// sort
			//rev.sort(std::greater<CZipAddFileInfo>());
			_tprintf(_T("\n"));
			
#ifdef _ZIPPIE_USES_UNICODE
			if (cmd.HasSwitch(_T("-utf")))
				zip.SetUnicodeMode(CZipArchive::umString);
			else if (cmd.HasSwitch(_T("-utfe")))
				zip.SetUnicodeMode(CZipArchive::umExtra);
			else
				zip.SetUnicodeMode(CZipArchive::umNone);
#endif
			for (FILELISTADDIT it1 = rev.begin(); it1 != rev.end(); ++it1)
			{	
				if (zip.AddNewFile((*it1).m_szFilePath, iLevel, bFullPath, iSmartLevel))
				{
					if (!bOnlyErrors)
						_tprintf(_T("%s added\n"), (LPCTSTR)(*it1).m_szFileNameInZip);
				}
				else
					_tprintf(_T("%s not added\n"), (LPCTSTR)(*it1).m_szFilePath);
			}

		}
		else if (bIsExtracting)
		{
			CZipString szPath = cmd.GetSafeArgument(_T("-xp"), 0, _T("."));
			
			CZipIndexesArray vRevised;
			ProcessData(zip, cmd, vRevised, true);
			ZipPlatform::FileOverwriteMode overwriteMode = cmd.HasSwitch(_T("-in")) ? ZipPlatform::fomOnlyIfNewer : ZipPlatform::fomRegular;
			_tprintf(_T("\n"));
			for (ZIP_ARRAY_SIZE_TYPE k = 0; k < vRevised.GetSize(); ++k)
			{
				ZIP_INDEX_TYPE iFile = vRevised[k];
				bool ok;
				try
				{
					ok = zip.ExtractFile(iFile, szPath, bFullPath, NULL, overwriteMode);
				}
				catch (...)
				{					
					ok = false;
				}
				CZipFileHeader fh;
				if (ok)
				{
					if (zip.GetFileInfo(fh, iFile) && !bOnlyErrors)
						_tprintf(_T("%s extracted\n"), (LPCTSTR)fh.GetFileName());
				}
				else
				{
					if (zip.GetFileInfo(fh, iFile))
						_tprintf(_T("Error while extracting file %s\n"), (LPCTSTR)fh.GetFileName());
					else
#if _MSC_VER >= 1900
						_tprintf(_T("Error while extracting a file. There are troubles with getting info from file number %llu\n"), iFile);
#else
						_tprintf(_T("Error while extracting a file. There are troubles with getting info from file number %u\n"), iFile);
#endif
					break;
				}
			}
			_tprintf(_T("\n"));
		}
		else if (bIsDeleting)
		{
			if (bIsSegm)
			{
				_tprintf(_T("Cannot delete from an existing disk spanning archive\n"));
				zip.Close();
				return 1;
			}
			CZipIndexesArray vRevised;
			ProcessData(zip, cmd, vRevised, false);			
			try
			{
				zip.RemoveFiles(vRevised);
			}
			catch (...)
			{
				_tprintf(_T("Error occured while deleting files\n"));
			}
		}
		else if (bIsRenaming)
		{
			zip[_ttoi(cmd.GetArgument(_T("-ren"), 0))]->SetFileName(cmd.GetArgument(_T("-ren"), 1));
		}
		// TODO: replace all %u with something that recognizes i64 and works on all platforms
		else if (cmd.HasSwitch(_T("-t")))
		{			
			bool allGood = true;
			ZIP_INDEX_TYPE iCount = zip.GetCount();
#if _MSC_VER >= 1900
			_tprintf(_T("Total files to test: %llu \r\n"), iCount);
#else
			_tprintf(_T("Total files to test: %u \r\n"), iCount);
#endif
			for (ZIP_INDEX_TYPE i = 0; i < iCount; i++)
			{
				bool bOK = false;
				try	
				{
					_tprintf(_T("Testing file: %u                \r"), (unsigned int)(i + 1));					
					bOK = zip.TestFile(i);
				}
				catch(CZipException& ex)
				{
					allGood = false;
					// display the exception message
					CZipFileHeader fh;
					if (zip.GetFileInfo(fh, i))
					{
						_tprintf(_T("\t[%u] ERROR %i (0x%X): Exception occurred when testing %s: %s\n"), (unsigned int)i, ex.m_iCause, ex.m_iSystemError, (LPCTSTR)fh.GetFileName(), (LPCTSTR)ex.GetErrorDescription());
					}
					else
					{
						_tprintf(_T("\t[%u] ERROR %i (0x%X): %s\n"), (unsigned int)i, ex.m_iCause, ex.m_iSystemError, (LPCTSTR)ex.GetErrorDescription());
					}
					
					bOK = true; // don't display the duplicated error
				}
				catch (...)
				{
					
				}
				if (!bOK)
				{	
					allGood = false;
					CZipFileHeader fh;
					if (zip.GetFileInfo(fh, i))
					{
						_tprintf(_T("\t[%u] ERROR: File %s is invaild.\n"), (unsigned int)i, (LPCTSTR)fh.GetFileName());
					}
					else
					{
						_tprintf(_T("\t[%u] ERROR:There are problems getting info from the file.\n"), (unsigned int)i);
					}
				}
			}
			if (allGood)
				_tprintf(_T("There were no errors found in the archive.\n"));
			else
				_tprintf(_T("\n"));
		}
		else if (cmd.HasSwitch(_T("-tv")))
		{
			CZipStorage* pStorage = zip.GetStorage();
			if (pStorage->IsSplit())
			{
				CZipCentralDir::CInfo info;
				zip.GetCentralDirInfo(info);
				ZIP_VOLUME_TYPE uLastVolume = info.m_uLastVolume + 1;
				bool bAllExist = true;
				for(ZIP_VOLUME_TYPE i = 1; i <= uLastVolume; i++)
				{
					CZipString szName = pStorage->GetSplitVolumeName(i - 1, i == uLastVolume);
					if (!ZipPlatform::FileExists(szName))
					{
						bAllExist = false;
						_tprintf(_T("The archive part %u was not found. Expected path: %s.\n"), i, (LPCTSTR)szName);
					}
					
				}
				if (bAllExist)
					_tprintf(_T("All volumes are present.\n"));
			}
			else
				_tprintf(_T("The archive is not a split archive.\n"));
		}
		else if (bIsListing)
		{
			bool bNumbers = cmd.HasSwitch(_T("-lr"));
			bool bDescription = !cmd.HasSwitch(_T("-ll"));
			ZIP_INDEX_TYPE iCount = zip.GetCount();
			if (bDescription)
				_tprintf(_T("\n  File name\tSize\t\tRatio\tTime Stamp\n\n"));
			for (ZIP_INDEX_TYPE i = 0; i < iCount; i++)
			{
				CZipFileHeader fh;
				if (zip.GetFileInfo(fh, i))
				{
					if (bNumbers)
#if _MSC_VER >= 1900
						_tprintf(_T("%llu.  "), i + 1);
#else
						_tprintf(_T("%u.  "), i + 1);
#endif

					_tprintf(_T("%s\n"), (LPCTSTR)fh.GetFileName());
					if (bDescription)
					{
						_tprintf(_T("\t\t"));
						if (fh.IsDirectory())
							_tprintf(_T("<DIR>\t\t"));
						else
						{
#if _MSC_VER >= 1900
							_tprintf(_T("%llu Bytes\t"), fh.m_uUncomprSize);
#else
							_tprintf(_T("%u Bytes\t"), fh.m_uUncomprSize);
#endif
							_tprintf(_T("%.2f%%"), fh.GetCompressionRatio());
						}
						time_t t = fh.GetModificationTime();
#if _MSC_VER >= 1400	
						const int bufSize = 26;
						char buf[bufSize];
						ctime_s(buf, bufSize, &t);
	#if _MSC_VER >= 1900
						_tprintf(_T("\t%hs"), buf);
	#else
						_tprintf(_T("\t%s"), buf);
	#endif
#else
						_tprintf(_T("\t%s"), ctime(&t));
#endif						
					}
				}
				else
#if _MSC_VER >= 1900
					_tprintf(_T("There are troubles with getting info from file number %llu\n"), i);
#else
					_tprintf(_T("There are troubles with getting info from file number %u\n"), i);
#endif

			}
			_tprintf(_T("\n"));
			CZipString sz = zip.GetGlobalComment();
			if (!sz.IsEmpty())
				_tprintf(_T("Global archive comment:\n%s\n"), (LPCTSTR)sz);
		}

		zip.Close();
	}
	catch (int)
	{
		DisplayUsage();
		iRet = 1;
	}
	catch (CZipException& e)
	{
		_tprintf(_T("Error while processing archive %s\n%s\n"), (LPCTSTR) szArchive, (LPCTSTR)e.GetErrorDescription());
		if (e.m_szFileName.IsEmpty())
			_tprintf(_T("\n"));
		else
			_tprintf(_T("Filename in error object: %s\n\n"), (LPCTSTR)e.m_szFileName);
		zip.Close(true);
		iRet = 1;
	}
	catch (...)
	{
		_tprintf(_T("Unknown error while processing archive %s\n\n"), (LPCTSTR) szArchive);
		zip.Close(true);
		iRet = 1;

	}

	if (cmd.HasSwitch(_T("-w")))
	{
		_tprintf(_T("\nPress <ENTER> to exit.\n"));
		ReadKey();
		_tprintf(_T("\n"));
	}
	
#if !defined __MINGW32__  && !defined __APPLE__
	std::locale::global(oldLocale);
	oldLocale = std::locale();
#endif	

	return iRet;
}

