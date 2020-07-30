/*

   SAPrefsDialog

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
// Code Changes:
// 
// 2001.10 Adapted to ZipArc project needs (by Tadeusz Dracz)
// 
// SAPrefsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SAPrefsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSAPrefsDialog dialog


CSAPrefsDialog::CSAPrefsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSAPrefsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSAPrefsDialog)
	//}}AFX_DATA_INIT

   m_iCurPage = -1;
   m_pages.RemoveAll();

	m_iStartPage = 0;

/*
	added by Tadeusz Dracz
	2001.10
*/
#ifdef IDR_MAINFRAME
		m_csConstantText.LoadString(IDR_MAINFRAME);
#endif //IDR_MAINFRAME

   m_csTitle = _T("Options");
}

/////////////////////////////////////////////////////////////////////////////

CSAPrefsDialog::~CSAPrefsDialog()
{
	// clean up
   for (int i=0;i<m_pages.GetSize();i++)
   {
      pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
      if (pPS)
         delete pPS;
   }
}

/////////////////////////////////////////////////////////////////////////////

void CSAPrefsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSAPrefsDialog)
	DDX_Control(pDX, IDC_PAGE_TREE, m_pageTree);
	DDX_Control(pDX, IDC_DLG_FRAME, m_boundingFrame);
	DDX_Control(pDX, IDC_CAPTION_BAR, m_captionBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSAPrefsDialog, CDialog)
	//{{AFX_MSG_MAP(CSAPrefsDialog)
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_PAGE_TREE, OnSelchangedPageTree)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_PAGE_TREE, OnGetdispinfoPageTree)
	ON_BN_CLICKED(IDC_SETDEFAULT, OnSetdefault)
	//}}AFX_MSG_MAP
	
#ifdef IDC_PHELP
	ON_BN_CLICKED(IDC_PHELP, OnPhelp)
#endif //IDC_PHELP

	ON_MESSAGE(WM_CHANGE_PAGE, OnChangePage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSAPrefsDialog message handlers

/////////////////////////////////////////////////////////////////////////////

BOOL CSAPrefsDialog::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT(pMsg != NULL);
   ASSERT_VALID(this);
   ASSERT(m_hWnd != NULL);

	// Don't let CDialog process the Escape key.
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}

	if (CWnd::PreTranslateMessage(pMsg))
      return TRUE;

   // don't translate dialog messages when 
   // application is in help mode
   CFrameWnd* pFrameWnd = GetTopLevelFrame();
   if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
      return FALSE;

   // ensure the dialog messages will not
   // eat frame accelerators
   pFrameWnd = GetParentFrame();
   while (pFrameWnd != NULL)
   {
      if (pFrameWnd->PreTranslateMessage(pMsg))
         return TRUE;
      pFrameWnd = pFrameWnd->GetParentFrame();
   }

   return PreTranslateInput(pMsg);

}

/////////////////////////////////////////////////////////////////////////////

int CSAPrefsDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CSAPrefsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

   long l = GetWindowLong(m_pageTree.m_hWnd, GWL_STYLE);
#if (_MSC_VER > 1100)
   l = l | TVS_TRACKSELECT ;
#else
	//#define TVS_TRACKSELECT         0x0200
   l = l | 0x0200;
#endif
   SetWindowLong(m_pageTree.m_hWnd, GWL_STYLE, l);

   // where will the dlgs live?
   m_boundingFrame.GetWindowRect(m_frameRect);
   ScreenToClient(m_frameRect);
   m_frameRect.DeflateRect(2,2);

   SetWindowText(m_csTitle);

	// set some styles for the pretty page indicator bar
   m_captionBar.m_textClr     = ::GetSysColor(COLOR_3DFACE);
	m_captionBar.m_fontWeight  = FW_BOLD;
	m_captionBar.m_fontSize    = 14;
   m_captionBar.m_csFontName  = _T("Verdana");
	m_captionBar.SetConstantText(m_csConstantText);

	// fill the tree. we'll create the pages as we need them
   for (int i=0;i<m_pages.GetSize();i++)
   {
      pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
      ASSERT(pPS);
      ASSERT(pPS->pDlg);
      if (pPS)
      {
         TV_INSERTSTRUCT tvi;

         // find this node's parent...
         tvi.hParent = FindHTREEItemForDlg(pPS->pDlgParent);

         tvi.hInsertAfter = TVI_LAST;
         tvi.item.cchTextMax = 0;
         tvi.item.pszText = LPSTR_TEXTCALLBACK;
         tvi.item.lParam = (LPARAM)pPS;
         tvi.item.mask = TVIF_PARAM | TVIF_TEXT;

         HTREEITEM hTree = m_pageTree.InsertItem(&tvi);

         // keep track of the dlg's we've added (for parent selection)
         if (hTree)
         {
            m_dlgMap.SetAt(pPS->pDlg, hTree);
         }
      }
   }

	if (ShowPage(m_iStartPage))
		m_iCurPage = m_iStartPage;
	else if (ShowPage(0))
		m_iCurPage = 0;
	else
		return FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

HTREEITEM CSAPrefsDialog::FindHTREEItemForDlg(CSAPrefsSubDlg *pParent)
{
   // if you didn't specify a parent in AddPage(...) , the
   // dialog becomes a root-level entry
	if (pParent==NULL)
	{
		return TVI_ROOT;
	}
   else
   {
	   HTREEITEM dwHTree;
      if (m_dlgMap.Lookup(pParent, dwHTree))
      {
          return dwHTree;
      }
      else
      {
         // you have specified a parent that has not 
         // been added to the tree - can't do that.
         ASSERT(FALSE);
         return TVI_ROOT;
      }
   }
}

/////////////////////////////////////////////////////////////////////////////

LRESULT CSAPrefsDialog::OnChangePage(WPARAM u, LPARAM l)
{
   if (ShowPage((int)u))
   {
      m_iCurPage = (int)u;   
   }

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////

bool CSAPrefsDialog::AddPage(CSAPrefsSubDlg &dlg, LPCTSTR pCaption, CSAPrefsSubDlg* pDlgParent /*=NULL*/)
{
	if (m_hWnd)
	{
		// can't add once the window has been created
		ASSERT(0);
		return false;
	}

   pageStruct *pPS = new pageStruct;
   pPS->pDlg = &dlg;
   pPS->id = dlg.GetID();
   pPS->csCaption = pCaption;
   pPS->pDlgParent = pDlgParent;

   m_pages.Add(pPS);

   return true;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CSAPrefsDialog::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
      return FALSE;

   cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS, NULL, NULL, NULL);
   cs.style |= WS_CLIPCHILDREN;
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

bool CSAPrefsDialog::ShowPage(CSAPrefsSubDlg * pPage)
{
   // find that page
	for (int i=0;i<m_pages.GetSize();i++)
	{
		pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
		ASSERT(pPS);
		if (pPS)
		{
			ASSERT(pPS->pDlg);
			if (pPS->pDlg == pPage)
			{
				ShowPage(i);
				m_iCurPage = i;
				return true;
			}
		}
	}

   return false;
}

/////////////////////////////////////////////////////////////////////////////

bool CSAPrefsDialog::ShowPage(int iPage)
{
   m_captionBar.SetWindowText(_T(""));

	// turn off the current page
   if ((m_iCurPage >= 0) && (m_iCurPage < m_pages.GetSize()))
   {
      pageStruct *pPS = (pageStruct *)m_pages.GetAt(m_iCurPage);
      ASSERT(pPS);
      if (pPS)
      {
         ASSERT(pPS->pDlg);
         if (pPS->pDlg)
         {
            if (::IsWindow(pPS->pDlg->m_hWnd))
            {
               pPS->pDlg->ShowWindow(SW_HIDE);
            }
         }
      }
      else
      {
         return false;
      }
   }

	// show the new one
   if ((iPage >= 0) && (iPage < m_pages.GetSize()))
   {
      pageStruct *pPS = (pageStruct *)m_pages.GetAt(iPage);
      ASSERT(pPS);

      if (pPS)
      {
         ASSERT(pPS->pDlg);
         if (pPS->pDlg)
         {

			
			m_captionBar.SetWindowText(pPS->csCaption);
			m_captionBar.RedrawWindow();
            // if we haven't already, Create the dialog
            if (!::IsWindow(pPS->pDlg->m_hWnd))
            {
               pPS->pDlg->Create(pPS->pDlg->GetID(), this);
            }
         
			  // update caption bar
            

            // move, show, focus
            if (::IsWindow(pPS->pDlg->m_hWnd))
            {
               pPS->pDlg->MoveWindow(m_frameRect.left, m_frameRect.top, m_frameRect.Width(), m_frameRect.Height());
               pPS->pDlg->ShowWindow(SW_SHOW);
               pPS->pDlg->SetFocus();
            }

            // change the tree

            // find this in our map
            HTREEITEM hItem = FindHTREEItemForDlg(pPS->pDlg);
            if (hItem)
            {
               // select it
               m_pageTree.SelectItem(hItem);
            }


			GetDlgItem(IDC_SETDEFAULT)->ShowWindow(pPS->pDlg->m_bNeedDefault ? SW_SHOW : SW_HIDE);
            return true;
         }
      }
   }

   return false;
}

/////////////////////////////////////////////////////////////////////////////

void CSAPrefsDialog::OnOK() 
{
   // if EndOK returns true, all of the UpdateData(TRUE)'s succeeded
   if (EndOK())
   {
	   CDialog::OnOK();
   }
}

/////////////////////////////////////////////////////////////////////////////

bool CSAPrefsDialog::EndOK()
{
   bool bOK = true;

   CSAPrefsSubDlg * pPage = NULL;

	// first, UpdateData...
   int i;
   for (i=0;i<m_pages.GetSize();i++)
   {
      pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
      ASSERT(pPS);
      if (pPS)
      {
         ASSERT(pPS->pDlg);
         if (pPS->pDlg)
         {
            if (::IsWindow(pPS->pDlg->m_hWnd))
            {
               if (!pPS->pDlg->UpdateData(TRUE))
               {
                  bOK = false;
                  pPage = pPS->pDlg;
                  break;
               }
            }
         }
      }
   }

   // were there any UpdateData errors?
   if ((!bOK) && (pPage!=NULL))
   {
      ShowPage(pPage);
      return false;
   }

   // tell all of the sub-dialogs "OK"
   for (i=0;i<m_pages.GetSize();i++)
   {
      pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
      ASSERT(pPS);
      if (pPS)
      {
         ASSERT(pPS->pDlg);
         if (pPS->pDlg)
         {
            if (::IsWindow(pPS->pDlg->m_hWnd))
            {
               pPS->pDlg->OnOK();
            }
         }
      }
   }

   return true;
}

/////////////////////////////////////////////////////////////////////////////

void CSAPrefsDialog::OnCancel() 
{
	// tell all of the sub-dialogs "Cancel"
   for (int i=0;i<m_pages.GetSize();i++)
   {
      pageStruct *pPS = (pageStruct *)m_pages.GetAt(i);
      ASSERT(pPS);
      
      if (pPS)
      {
         ASSERT(pPS->pDlg);
         if (pPS->pDlg)
         {
            if (::IsWindow(pPS->pDlg->m_hWnd))
            {
               pPS->pDlg->OnCancel();
            }
         }
      }
   }

	CDialog::OnCancel();
}


/////////////////////////////////////////////////////////////////////////////

void CSAPrefsDialog::EndSpecial(UINT res, bool bOk)
{
   if (bOk)
   {
      EndOK();
   }

	EndDialog(res);
}

/////////////////////////////////////////////////////////////////////////////

void CSAPrefsDialog::OnSelchangedPageTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

   if (pNMTreeView->itemNew.lParam)
   {
		// find out which page was selected
      int iIdx = -1;
      for (int i=0;i<m_pages.GetSize();i++)
      {
         if (m_pages.GetAt(i)==(pageStruct *)pNMTreeView->itemNew.lParam)
         {
            iIdx = i;
            break;
         }
      }

		// show that page
      if ((iIdx >= 0) && (iIdx < m_pages.GetSize()))
      {
         pageStruct *pPS = (pageStruct *)m_pages.GetAt(iIdx);
         if (m_iCurPage!=iIdx)
         {
				PostMessage(WM_CHANGE_PAGE, iIdx);
         }
      }
   }

	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////

void CSAPrefsDialog::OnGetdispinfoPageTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	// return the caption of the appropriate dialog
   if (pTVDispInfo->item.lParam)
   {
      if (pTVDispInfo->item.mask & TVIF_TEXT)
      {
         pageStruct *pPS = (pageStruct *)pTVDispInfo->item.lParam;
#if _MSC_VER >= 1400	
		 _tcscpy_s(pTVDispInfo->item.pszText, pTVDispInfo->item.cchTextMax, pPS->csCaption);
#else
		 _tcscpy(pTVDispInfo->item.pszText, pPS->csCaption);
#endif
		 
      }
   }
   
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////


#ifdef IDC_PHELP
	void CSAPrefsDialog::OnPhelp() 
	{
		// simulate the property sheet method of sending Help (with WM_NOTIFY)
	   if ((m_iCurPage >= 0) && (m_iCurPage < m_pages.GetSize()))
	   {
	      pageStruct *pPS = (pageStruct *)m_pages.GetAt(m_iCurPage);
	      ASSERT(pPS);
	      ASSERT(pPS->pDlg);
	      if (pPS)
	      {
	         if (pPS->pDlg)
	         {
	            if (::IsWindow(pPS->pDlg->m_hWnd))
	            {
	               // help!
				      NMHDR nm;
				      nm.code=PSN_HELP;
				      nm.hwndFrom=m_hWnd;
				      nm.idFrom=CSAPrefsDialog::IDD;
				      pPS->pDlg->SendMessage(WM_NOTIFY, 0, (long)&nm);
	            }
	         }
		}
	}
}

#endif //IDC_PHELP

void CSAPrefsDialog::OnSetdefault() 
{
   if ((m_iCurPage >= 0) && (m_iCurPage < m_pages.GetSize()))
   {
	  pageStruct *pPS = (pageStruct *)m_pages.GetAt(m_iCurPage);
	  ASSERT(pPS);
	  ASSERT(pPS->pDlg);
	  if (pPS && pPS->pDlg && pPS->pDlg->m_bNeedDefault && ::IsWindow(pPS->pDlg->m_hWnd))
		  pPS->pDlg->SendMessage(WM_SETDEFAULT);
	}
	
}
