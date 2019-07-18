// GetTextDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UINameTag.h"
#include "GetTextDlg.h"
#include "afxdialogex.h"

#include <platform/boss_platform.hpp>
#include <service/boss_boxr.hpp>
#include <format/boss_bmp.hpp>

// CGetTextDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGetTextDlg, CDialogEx)

CGetTextDlg::CGetTextDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_GETTEXT, pParent)
{
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
	DDX_Control(pDX, IDC_EDIT1, Text);
}

BEGIN_MESSAGE_MAP(CGetTextDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGetTextDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGetTextDlg 메시지 처리기입니다.


void CGetTextDlg::OnBnClickedOk()
{
	CStringW TagNameMFC;
	Text.GetWindowTextW(TagNameMFC);
	String TagName = String::FromWChars((LPCWSTR) TagNameMFC);

	Platform::File::ResetAssetsRoot("./");
	Platform::File::ResetAssetsRemRoot("./");
	id_bitmap NewBitmap = BoxrBuilder::MakeTagBitmap("ui_atlaskey.png", TagName);
	if(NewBitmap)
	{
		const sint32 BitmapSize = Bmp::GetFileSizeWithoutBM(NewBitmap) - sizeof(Bmp::bitmapfile);
		NewGlobalBitmap = GlobalAlloc(GHND, BitmapSize);
		void* NewGlobalBitmapPtr = GlobalLock(NewGlobalBitmap);
		memcpy(NewGlobalBitmapPtr, ((bytes) NewBitmap) + sizeof(Bmp::bitmapfile), BitmapSize);
		GlobalUnlock(NewGlobalBitmapPtr);

		if(NewGlobalBitmap)
		{
			if(OpenClipboard())
			{
				EmptyClipboard();
				SetClipboardData(CF_DIB, NewGlobalBitmap);
				CloseClipboard();
			}
		}
		Bmp::Remove(NewBitmap);
	}
	else AfxMessageBox(L"ui_atlaskey.png파일이 없습니다.");
}
