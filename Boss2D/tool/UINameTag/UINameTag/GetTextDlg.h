#pragma once
#include "afxwin.h"


// CGetTextDlg 대화 상자입니다.

class CGetTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGetTextDlg)

public:
	CGetTextDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGetTextDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GETTEXT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit Text;
	HGLOBAL NewGlobalBitmap;
	afx_msg void OnBnClickedOk();
};
