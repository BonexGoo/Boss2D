#pragma once
#include "afxwin.h"


// CGetTextDlg ��ȭ �����Դϴ�.

class CGetTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGetTextDlg)

public:
	CGetTextDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGetTextDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GETTEXT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit Text;
	HGLOBAL NewGlobalBitmap;
	afx_msg void OnBnClickedOk();
};
