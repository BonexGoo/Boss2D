
// ProjectCreatorDlg.h : ��� ����
//

#pragma once


// CProjectCreatorDlg ��ȭ ����
class CProjectCreatorDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CProjectCreatorDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJECTCREATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CString mTitleName;
    CString mProjName;
    CString mCorpName;
    afx_msg void OnCreateClick();
};
