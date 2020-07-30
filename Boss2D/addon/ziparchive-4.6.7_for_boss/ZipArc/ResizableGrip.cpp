// ResizableGrip.cpp: implementation of the CResizableGrip class.
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000 by Paolo Messina
// (ppescher@yahoo.com)
//
// Free for non-commercial and commercial use,
// provided that the original author's name
// and copyright is quoted somewhere in the
// final executable and in the program's help
// or documentation.
// You may change the code to your needs,
// provided that credits to the original
// author are given in the modified files.
// Also a copy of your enhancements would
// be nice, but it's not required. Please,
// consider to share your work on CodeProject.
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResizableGrip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResizableGrip::CResizableGrip()
{
	m_sizeGrip.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_sizeGrip.cy = GetSystemMetrics(SM_CYHSCROLL);
}

CResizableGrip::~CResizableGrip()
{

}

void CResizableGrip::UpdateGripPos()
{
	// size-grip goes bottom right in the client area
	// (any right-to-left adjustment should go here)

	RECT rect;
	GetResizableWnd()->GetClientRect(&rect);

	rect.left = rect.right - m_sizeGrip.cx;
	rect.top = rect.bottom - m_sizeGrip.cy;

	// must stay below other children
	m_wndGrip.SetWindowPos(&CWnd::wndBottom, rect.left, rect.top, 0, 0,
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREPOSITION);

	// maximized windows cannot be resized
	if (GetResizableWnd()->IsZoomed())
		m_wndGrip.EnableWindow(FALSE);
	else
		m_wndGrip.EnableWindow(TRUE);
}

void CResizableGrip::ShowSizeGrip(BOOL bShow)
{
	m_wndGrip.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

#define RSZ_GRIP_OBJ	_T("ResizableGrip")

BOOL CResizableGrip::InitGrip()
{
	CRect rect(0 , 0, m_sizeGrip.cx, m_sizeGrip.cy);

	BOOL bRet = m_wndGrip.Create(WS_CHILD | WS_CLIPSIBLINGS | SBS_SIZEGRIP,
		rect, GetResizableWnd(), 0);

	if (bRet)
	{
		// set a triangular window region
		CRgn rgnGrip, rgn;
		rgn.CreateRectRgn(0,0,1,1);
		rgnGrip.CreateRectRgnIndirect(&rect);
	
		for (int y=0; y<m_sizeGrip.cy; y++)
		{
			rgn.SetRectRgn(0, y, m_sizeGrip.cx-y, y+1);
			rgnGrip.CombineRgn(&rgnGrip, &rgn, RGN_DIFF);
		}
		m_wndGrip.SetWindowRgn((HRGN)rgnGrip.Detach(), FALSE);

		// subclass control
#ifdef _WIN64
		::SetProp(m_wndGrip, RSZ_GRIP_OBJ, (HANDLE)::GetWindowLongPtr(m_wndGrip, GWLP_WNDPROC));
		::SetWindowLongPtr(m_wndGrip, GWLP_WNDPROC, (LONG_PTR)GripWindowProc);
#else
		::SetProp(m_wndGrip, RSZ_GRIP_OBJ, (HANDLE)::GetWindowLong(m_wndGrip, GWL_WNDPROC));
		::SetWindowLong(m_wndGrip, GWL_WNDPROC, (LONG)GripWindowProc);
#endif

		// update pos
		UpdateGripPos();
		ShowSizeGrip();
	}

	return bRet;
}

LRESULT CResizableGrip::GripWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldWndProc = (WNDPROC)::GetProp(hwnd, RSZ_GRIP_OBJ);

	switch (msg)
	{
	case WM_NCHITTEST:

		// choose proper cursor shape
		if (IsRTL(hwnd))
			return HTBOTTOMLEFT;
		else
			return HTBOTTOMRIGHT;

	case WM_DESTROY:
		
		// unsubclass
		::RemoveProp(hwnd, RSZ_GRIP_OBJ);
#ifdef _WIN64
		::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oldWndProc);
#else
		::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)oldWndProc);
#endif

		break;
	}

	return ::CallWindowProc(oldWndProc, hwnd, msg, wParam, lParam);
}
