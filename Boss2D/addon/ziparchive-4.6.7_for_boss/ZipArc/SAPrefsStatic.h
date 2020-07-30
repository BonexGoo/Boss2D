/*

   Copyright (C) 2000 Smaller Animals Software, Inc.

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.

   http://www.smalleranimals.com
   smallest@smalleranimals.com

**********************************************************************/

#if !defined(AFX_PREFSSTATIC_H__1B15B006_9152_11D3_A10C_00500402F30B__INCLUDED_)
#define AFX_PREFSSTATIC_H__1B15B006_9152_11D3_A10C_00500402F30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PrefsStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefsStatic window

class CSAPrefsStatic : public CStatic
{
// Construction
public:
	CSAPrefsStatic();

// Attributes
public:

// Operations
public:
   CString m_csFontName;

	void		SetConstantText(LPCTSTR pText)	{m_csConstantText = pText;}

   int m_fontSize, m_fontWeight;
   BOOL m_grayText;
   COLORREF m_textClr;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSAPrefsStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSAPrefsStatic();

protected:
   CFont m_captionFont, m_nameFont;

   CBitmap m_bm;

	CString m_csConstantText;
	

   void MakeCaptionBitmap();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSAPrefsStatic)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetText (WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFSSTATIC_H__1B15B006_9152_11D3_A10C_00500402F30B__INCLUDED_)
