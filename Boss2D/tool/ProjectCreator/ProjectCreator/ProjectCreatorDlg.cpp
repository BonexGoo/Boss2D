
// ProjectCreatorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ProjectCreator.h"
#include "ProjectCreatorDlg.h"
#include "afxdialogex.h"

#include <boss.hpp>
#include <platform/boss_platform.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProjectCreatorDlg 대화 상자



CProjectCreatorDlg::CProjectCreatorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROJECTCREATOR_DIALOG, pParent)
    , mTitleName(_T(""))
    , mProjName(_T(""))
    , mCorpName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProjectCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, mTemplateCombo);
    DDX_Text(pDX, IDC_EDIT1, mTitleName);
    DDX_Text(pDX, IDC_EDIT2, mProjName);
    DDX_Text(pDX, IDC_EDIT3, mCorpName);
}

BEGIN_MESSAGE_MAP(CProjectCreatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CProjectCreatorDlg::OnCreateClick)
END_MESSAGE_MAP()


// CProjectCreatorDlg 메시지 처리기

BOOL CProjectCreatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	mTemplateCombo.AddString(_T("Basic"));
    mTemplateCombo.AddString(_T("Daddy"));
    mTemplateCombo.SetCurSel(0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CProjectCreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CProjectCreatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CProjectCreatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static buffer ReplaceContent(buffer data, chars oldtext, chars newtext)
{
    static sint08s Collector;
    Collector.SubtractionAll();

    const sint32 OldTextSize = boss_strlen(oldtext);
    const sint32 NewTextSize = boss_strlen(newtext);

    chars Binary = (chars) data;
    const sint32 BinarySize = Buffer::SizeOf(data) * Buffer::CountOf(data);
    sint32 iOld = 0;
    for(sint32 i = 0; i < BinarySize; ++i)
    {
        if(Binary[i] == oldtext[0] && !boss_strncmp(&Binary[i], oldtext, OldTextSize))
        {
            if(iOld < i)
            {
                const sint32 CopyLen = i - iOld;
                Memory::Copy(Collector.AtDumpingAdded(CopyLen), &Binary[iOld], CopyLen);
            }
            Memory::Copy(Collector.AtDumpingAdded(NewTextSize), newtext, NewTextSize);
            iOld = i + OldTextSize;
            i += OldTextSize - 1;
        }
    }

    if(0 < Collector.Count())
    {
        if(iOld < BinarySize)
        {
            const sint32 CopyLen = BinarySize - iOld;
            Memory::Copy(Collector.AtDumpingAdded(CopyLen), &Binary[iOld], CopyLen);
        }
        Buffer::Free(data);
        return ((Share*)(id_share) Collector)->CopiedBuffer();
    }
    return data;
}

void CProjectCreatorDlg::OnCreateClick()
{
    CDialogEx::OnOK();

    String TitleName = String::FromWChars((LPCWSTR) mTitleName);
    if(TitleName.Length() == 0) TitleName = "Untitle";
    String ProjectName = String::FromWChars((LPCWSTR) mProjName);
    if(ProjectName.Length() == 0) ProjectName = "noname";
    String CorpName = String::FromWChars((LPCWSTR) mCorpName);
    if(CorpName.Length() == 0) CorpName = "mycompany";

    int ResourceID = IDR_ZIP_BASIC;
    switch(mTemplateCombo.GetCurSel())
    {
    case 0: ResourceID = IDR_ZIP_BASIC; break;
    case 1: ResourceID = IDR_ZIP_DADDY; break;
    }

    String GetPath;
    if(Platform::Popup::FileDialog(DST_Dir, GetPath, nullptr, "압축을 풀 폴더를 지정하세요"))
    if(HRSRC ResInfo = FindResourceW(NULL, MAKEINTRESOURCEW(ResourceID), L"ZIP"))
    if(HANDLE Res = LoadResource(NULL, ResInfo))
    {
        DWORD ResSize = SizeofResource(NULL, ResInfo);
        LPVOID ResBits = (LPVOID) LockResource(Res);

        sint32 FileCount = 0;
        if(id_zip NewZip = AddOn::Zip::Create((bytes) ResBits, ResSize, &FileCount))
        {
            for(sint32 i = 0; i < FileCount; ++i)
            {
                bool IsDir = false;
                String FileName = AddOn::Zip::GetFileInfo(NewZip, i, &IsDir);
                if(!IsDir)
                {
                    buffer LastFileName = ((Share*)(id_share) FileName)->CopiedBuffer();
                    LastFileName = ReplaceContent(LastFileName, "[[[DISPNAME]]]", TitleName);
                    LastFileName = ReplaceContent(LastFileName, "[[[PROJNAME]]]", ProjectName);
                    LastFileName = ReplaceContent(LastFileName, "[[[CORPNAME]]]", CorpName);
                    const String ChangedFileName((chars) LastFileName);
                    Buffer::Free(LastFileName);

                    if(buffer NewBuffer = AddOn::Zip::ToFile(NewZip, i))
                    {
                        NewBuffer = ReplaceContent(NewBuffer, "[[[DISPNAME]]]", TitleName);
                        NewBuffer = ReplaceContent(NewBuffer, "[[[PROJNAME]]]", ProjectName);
                        NewBuffer = ReplaceContent(NewBuffer, "[[[CORPNAME]]]", CorpName);

                        id_file NewFile = Platform::File::OpenForWrite(GetPath + '/' + ProjectName + '/' + ChangedFileName, true);
                        Platform::File::Write(NewFile, (bytes) NewBuffer, Buffer::SizeOf(NewBuffer) * Buffer::CountOf(NewBuffer));
                        Platform::File::Close(NewFile);
                        Buffer::Free(NewBuffer);
                    }
                }
            }
            AddOn::Zip::Release(NewZip);
        }

        UnlockResource(Res);
        FreeResource(Res);
    }
}
