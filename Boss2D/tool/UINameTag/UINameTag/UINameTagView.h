
// UINameTagView.h : CUINameTagView Ŭ������ �������̽�
//

#pragma once


class CUINameTagView : public CView
{
protected: // serialization������ ��������ϴ�.
	CUINameTagView();
	DECLARE_DYNCREATE(CUINameTagView)

// Ư���Դϴ�.
public:
	CUINameTagDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CUINameTagView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // UINameTagView.cpp�� ����� ����
inline CUINameTagDoc* CUINameTagView::GetDocument() const
   { return reinterpret_cast<CUINameTagDoc*>(m_pDocument); }
#endif

