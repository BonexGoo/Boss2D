
// UINameTagView.cpp : CUINameTagView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "UINameTag.h"
#endif

#include "UINameTagDoc.h"
#include "UINameTagView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUINameTagView

IMPLEMENT_DYNCREATE(CUINameTagView, CView)

BEGIN_MESSAGE_MAP(CUINameTagView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CUINameTagView ����/�Ҹ�

CUINameTagView::CUINameTagView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CUINameTagView::~CUINameTagView()
{
}

BOOL CUINameTagView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CUINameTagView �׸���

void CUINameTagView::OnDraw(CDC* /*pDC*/)
{
	CUINameTagDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}

void CUINameTagView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CUINameTagView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CUINameTagView ����

#ifdef _DEBUG
void CUINameTagView::AssertValid() const
{
	CView::AssertValid();
}

void CUINameTagView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUINameTagDoc* CUINameTagView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUINameTagDoc)));
	return (CUINameTagDoc*)m_pDocument;
}
#endif //_DEBUG


// CUINameTagView �޽��� ó����
