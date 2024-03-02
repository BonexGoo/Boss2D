// GetTextDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "UINameTag.h"
#include "GetTextDlg.h"
#include "afxdialogex.h"

#include <platform/boss_platform.hpp>
#include <service/boss_boxr.hpp>
#include <format/boss_bmp.hpp>

// CGetTextDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CGetTextDlg, CDialogEx)

CGetTextDlg::CGetTextDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_GETTEXT, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	NewGlobalBitmap = NULL;
}

CGetTextDlg::~CGetTextDlg()
{
	if(NewGlobalBitmap)
	{
		GlobalFree(NewGlobalBitmap);
		NewGlobalBitmap = NULL;
	}
}

void CGetTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, Version);
	DDX_Control(pDX, IDC_EDIT1, Text);

	Version.AddString(_T("VER.1"));
	Version.AddString(_T("VER.2"));
	Version.SetCurSel(0);
}

BEGIN_MESSAGE_MAP(CGetTextDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGetTextDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGetTextDlg �޽��� ó�����Դϴ�.

BOOL CGetTextDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CGetTextDlg::OnBnClickedOk()
{
	const INT VerCode = Version.GetCurSel() + 1;
	CStringW TagNameMFC;
	Text.GetWindowTextW(TagNameMFC);
	String TagName = String::FromWChars((LPCWSTR) TagNameMFC);

	CStringA AtlasName = (VerCode == 0)? "ui_atlaskey.png" : "ui_atlaskey2.png";
	Platform::File::ResetAssetsRoot("./");
	Platform::File::ResetAssetsRemRoot("./");
	if(id_bitmap NewBitmap = BoxrBuilder::MakeTagBitmap(AtlasName, TagName, VerCode))
	{
		const sint32 BitmapSize = Bmp::GetFileSizeWithoutBM(NewBitmap) - sizeof(Bmp::bitmapfile);
		if(NewGlobalBitmap = GlobalAlloc(GHND, BitmapSize))
		{
			void* NewGlobalBitmapPtr = GlobalLock(NewGlobalBitmap);
			memcpy(NewGlobalBitmapPtr, ((bytes) NewBitmap) + sizeof(Bmp::bitmapfile), BitmapSize);
			GlobalUnlock(NewGlobalBitmapPtr);
			if(OpenClipboard())
			{
				EmptyClipboard();
				SetClipboardData(CF_DIB, NewGlobalBitmap);
				CloseClipboard();
			}
		}
		Bmp::Remove(NewBitmap);
	}
	else AfxMessageBox(AtlasName + L"������ �����ϴ�.");
}
