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

	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	HGLOBAL NewGlobalBitmap;
	CComboBox Version;
	CEdit Text;
	afx_msg void OnBnClickedOk();
};
