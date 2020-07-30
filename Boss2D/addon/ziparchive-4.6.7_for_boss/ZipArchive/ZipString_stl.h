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

#ifndef ZIPARCHIVE_ZIPSTRING_DOT_H
	#error Do not include this file directly. Include ZipString.h instead
#endif

#include "stdafx.h"

#if _MSC_VER > 1000
	#pragma warning( push, 3 ) // STL requirements
	#pragma warning( disable : 4275 ) // non dll-interface class used as base for dll-interface
	#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
#endif


#include <cstring>
#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <cctype>
#include <locale>

#include "ZipExport.h"

#ifndef _vsntprintf 
	#ifndef __GNUC__    
			#ifdef  _UNICODE
				#define _vsntprintf _vsnwprintf
			#else
				#define _vsntprintf _vsnprintf
			#endif
	#else 
		#ifdef  _UNICODE
			#define _vsntprintf vswprintf
		#else
			#define _vsntprintf vsnprintf
		#endif
	#endif
#endif

typedef std::basic_string<TCHAR> stdbs;

/**	
	It contains mostly the methods required by ZipArchive Library.
*/
class ZIP_API CZipString 
{ 
	static std::locale m_gCurrentLocale;
#if defined __GNUC__ && !defined __MINGW32__ 
	static std::locale m_gGlobalLocale;
	static std::locale m_gClassicLocale;
#endif

    stdbs m_str; 

    void TrimInternalL(stdbs::size_type iPos) 
    { 
        if (iPos == stdbs::npos) 
                m_str.erase (); 
        if (iPos) 
                m_str.erase(0, iPos); 
    } 
    void TrimInternalR(stdbs::size_type iPos) 
    { 
        if (iPos == stdbs::npos) 
                m_str.erase (); 
        m_str.erase(++iPos); 
    } 
        
#ifndef __GNUC__ 
    static int zslen(const TCHAR* lpsz) 
    { 
        if (!lpsz) return 0; 

        // we want to take into account the locale stuff (by using standard templates) 

        #ifdef _UNICODE 
                return (int)std::wstring(lpsz).length(); 
        #else 
                return (int)std::string(lpsz).length(); 
        #endif 
    }         
#else 
    static int zslen(const TCHAR* lpsz) 
    { 
        #if (__GNUC__ < 3) // I'm not sure which precisely version should be put here 
            return lpsz ? (int)std::string_char_traits<TCHAR>::length(lpsz) : 0;
        #else 
            return lpsz ? (int)std::char_traits<TCHAR>::length(lpsz) : 0;
        #endif 

	}         
#endif 

	static TCHAR tl(TCHAR c) 
	{ 
			// use_facet doesn't work here well (doesn't convert all the local characters properly) 
		return std::tolower(c, m_gCurrentLocale); 
	} 
	static TCHAR tu(TCHAR c) 
	{ 
			// use_facet doesn't work here well (doesn't convert all the local characters properly) 
		return std::toupper(c, m_gCurrentLocale); 
	} 

public: 
    CZipString(){} 
    explicit CZipString (TCHAR ch, int nRepeat = 1):m_str(nRepeat, ch){} 
	CZipString( const CZipString& stringSrc ) {m_str.assign(stringSrc.m_str);} 
    CZipString( const stdbs& stringSrc ) {m_str.assign(stringSrc);} 
    CZipString( LPCTSTR lpsz ){if (!lpsz) Empty(); else m_str.assign(lpsz);} 
    operator LPCTSTR() const{return m_str.c_str();} 		
	operator stdbs&() {return m_str;} 
	operator const stdbs&() const {return m_str;} 
	void SetString(stdbs& stringSrc)
	{
		m_str = stringSrc;
	}
        
    int GetLength() const {return (int) m_str.size();} 
    bool IsEmpty() const {return m_str.empty();} 
    void Empty() {m_str.erase(m_str.begin(), m_str.end());} 
    TCHAR GetAt (int iIndex) const{return m_str.at(iIndex);} 
    TCHAR operator[] (int iIndex) const{return m_str.at(iIndex);} 
    void SetAt( int nIndex, TCHAR ch ) {m_str.at(nIndex) = ch;} 
    LPTSTR GetBuffer(int nMinBufLength) 
    { 
		if ((int)m_str.size() < nMinBufLength) 
				m_str.resize(nMinBufLength); 
		return m_str.empty() ? const_cast<TCHAR*>(m_str.data()) : &(m_str.at(0)); 
    } 
    void ReleaseBuffer( int nNewLength = -1 ) { m_str.resize(nNewLength > -1 ? nNewLength : zslen(m_str.c_str()));} 
    void TrimLeft( TCHAR chTarget ) 
    { 
		TrimInternalL(m_str.find_first_not_of(chTarget)); 
    } 
    void TrimLeft( LPCTSTR lpszTargets ) 
    { 
		TrimInternalL(m_str.find_first_not_of(lpszTargets)); 
    } 
    void TrimRight( TCHAR chTarget ) 
    { 
		TrimInternalR(m_str.find_last_not_of(chTarget)); 
    } 
    void TrimRight( LPCTSTR lpszTargets ) 
    { 
		TrimInternalR(m_str.find_last_not_of(lpszTargets)); 
    } 

    CZipString& operator+=(TCHAR str) 
	{
		return operator+=(CZipString(str));        
	} 

    CZipString& operator+=(const CZipString& str) 
	{
		m_str += str.m_str;
		return *this; 
	} 

    CZipString& operator+=(const stdbs& str)
	{ 
		return operator+=(CZipString(str));        
	} 

    CZipString& operator+=(LPCTSTR str)
	{
		return operator+=(CZipString(str)); 
	} 
        
    CZipString operator+(TCHAR str) const 
	{
		return operator+(CZipString(str));        
	} 

    CZipString operator+(const CZipString& str) const 
	{
		return CZipString(m_str + str.m_str);
	} 

    CZipString operator+(const stdbs& str) const
	{
		return operator+(CZipString(str)); 		
	} 

    CZipString operator+(LPCTSTR str) const
	{ 
		return operator+(CZipString(str)); 
	} 


#if _MSC_VER >= 1300 
    #pragma warning( push ) 
    #pragma warning (disable : 4793) // 'vararg' : causes native code generation for function 'void CZipString::Format(LPCTSTR,...)' 
#if _MSC_VER > 1310 
	#pragma warning (disable : 6001)
#endif
#endif 

	void Format(LPCTSTR lpszFormat, ...) 
	{ 
		va_list arguments; 
		va_start (arguments, lpszFormat); 
		TCHAR* pBuf = NULL; 
		int iCounter = 1, uTotal = 0; 
		do 
		{ 
				int nChars = iCounter * 1024; 
				int nLen = sizeof(TCHAR) * nChars; 
                        
				TCHAR* pTempBuf = (TCHAR*)realloc((void*)pBuf, nLen);                         
				if (!pTempBuf) 
				{ 
						if (pBuf != NULL) 
								free(pBuf); 
						va_end (arguments); 
						return; 
				} 
				pBuf = pTempBuf; 

	#if _MSC_VER >= 1400         
				uTotal = _vsntprintf_s(pBuf, nChars, nChars - 1, lpszFormat, arguments); 
	#else 
				uTotal = _vsntprintf(pBuf, nChars - 1, lpszFormat, arguments); 
	#endif 
                        
				if (uTotal == -1 || (uTotal == nChars - 1) ) // for some implementations 
				{ 
						pBuf[nChars - 1] = _T('\0'); 
						if (iCounter == 7) 
								break; 
				} 
				else 
				{ 
						pBuf[uTotal] = _T('\0'); 
						break; 
				} 
				iCounter++; 

		} 
		while (true); 
                
		va_end (arguments); 
		*this = pBuf; 
		free(pBuf); 
	} 

#if _MSC_VER >= 1300 
	#pragma warning( pop ) 
#endif 

    void Insert( int nIndex, LPCTSTR pstr ){m_str.insert(nIndex, pstr, zslen(pstr));} 
    void Insert( int nIndex, TCHAR ch ) {m_str.insert(nIndex, 1, ch);} 
    int Delete( int nIndex, int nCount = 1 ) 
    { 
        int iSize = (int) m_str.size(); 
        int iToDelete = iSize < nIndex + nCount ? iSize - nIndex : nCount; 
        if (iToDelete > 0) 
        { 
                m_str.erase(nIndex, iToDelete); 
                iSize -= iToDelete; 
        } 
        return iSize; 
    } 
#ifndef __MINGW32__         
    void MakeLower() 
    { 
		std::transform(m_str.begin(),m_str.end(),m_str.begin(),tl); 
    } 
    void MakeUpper() 
    { 
		std::transform(m_str.begin(),m_str.end(),m_str.begin(),tu); 
    } 
#else 
    void MakeLower() 
    { 
		std::transform(m_str.begin(),m_str.end(),m_str.begin(),tolower); 
    } 
    void MakeUpper() 
    { 
		std::transform(m_str.begin(),m_str.end(),m_str.begin(),toupper); 
    } 
#endif         
    void MakeReverse() 
    { 
            std::reverse(m_str.begin(), m_str.end()); 
    } 
    CZipString Left( int nCount ) const { return m_str.substr(0, nCount);} 
    CZipString Right( int nCount) const 
    { 
	int s = (int)m_str.size();
	nCount = s < nCount ? s : nCount;
	return m_str.substr(s - nCount);
    } 
    CZipString Mid( int nFirst ) const {return m_str.substr(nFirst);} 
    CZipString Mid( int nFirst, int nCount ) const {return m_str.substr(nFirst, nCount);} 
    int Collate( LPCTSTR lpsz ) const 
    { 
#if !defined __GNUC__ || defined __MINGW32__ 
		return _tcscoll(m_str.c_str(), lpsz); 
#else 
	#ifdef _UNICODE                
		return wcscoll(m_str.c_str(), lpsz); 
	#else
		return strcoll(m_str.c_str(), lpsz); 
	#endif
#endif 
     } 

	int CollateNoCase( LPCTSTR lpsz ) const 
    { 
#if !defined __GNUC__ || defined __MINGW32__ 
		return _tcsicoll(m_str.c_str(), lpsz); 
#else 
	#ifndef _UNICODE
		if (m_gGlobalLocale == m_gClassicLocale) 
			return strcasecmp(m_str.c_str(), lpsz); 
	#endif
		CZipString first(m_str.c_str());
		CZipString second(lpsz);
		first.MakeLower();
		second.MakeLower();
		return first.Collate(second);
#endif 
	} 

	int Compare( LPCTSTR lpsz ) const 
	{ 
			return m_str.compare(lpsz); 
	} 

	int CompareNoCase( LPCTSTR lpsz ) const 
	{ 
#if !defined __GNUC__ || defined __MINGW32__ 
		return _tcsicmp(m_str.c_str(), lpsz); 
#else 
	#ifndef _UNICODE			
		return strcasecmp(m_str.c_str(), lpsz); 
	#else
		CZipString first(m_str.c_str());
		CZipString second(lpsz);
		first.MakeLower();
		second.MakeLower();
		return first.Compare(second);
	#endif
#endif 
	} 
        
	bool operator != (LPCTSTR lpsz) 
	{ 
		return Compare(lpsz) != 0; 
	} 
	bool operator == (LPCTSTR lpsz) 
	{ 
		return Compare(lpsz) == 0; 
	} 
	int Find( TCHAR ch, int nStart = 0) const 
	{ 
		return (int) m_str.find(ch, nStart); 
	} 

	int Find( LPCTSTR pstr, int nStart = 0) const 
	{ 
		return (int) m_str.find(pstr, nStart); 
	} 

	int Replace( TCHAR chOld, TCHAR chNew ) 
	{ 
		int iCount = 0; 
		for (stdbs::iterator it = m_str.begin(); it != m_str.end(); ++it) 
				if (*it == chOld) 
				{ 
						*it = chNew; 
						iCount++; 
				} 
		return iCount; 
	}         
}; 

inline bool operator<(
	const CZipString& left,
	const CZipString& right)
{	
	return (left.Compare(right) < 0);
}

inline bool operator>(
	const CZipString& left,
	const CZipString& right)
{	
	return (left.Compare(right) > 0);
}

inline bool operator<=(
	const CZipString& left,
	const CZipString& right)
{	
	return (left.Compare(right) <= 0);
}

inline bool operator>=(
	const CZipString& left,
	const CZipString& right)
{	
	return (left.Compare(right) >= 0);
}

inline bool operator==(
	const CZipString& left,
	const CZipString& right)
{	
	return (left.Compare(right) == 0);
}

inline bool operator!=(
	const CZipString& left,
	const CZipString& right)
{	
	return (left.Compare(right) != 0);
}

/**
	A pointer type to point to CZipString to Collate or CollateNoCase
	or Compare or CompareNoCase
*/
typedef int (CZipString::*ZIPSTRINGCOMPARE)( LPCTSTR ) const;

/**
	return a pointer to the function in CZipString structure, 
	used to compare elements depending on the arguments
*/
	ZIP_API ZIPSTRINGCOMPARE GetCZipStrCompFunc(bool bCaseSensitive, bool bCollate = true);


#if _MSC_VER > 1000
	#pragma warning( pop)
#endif
