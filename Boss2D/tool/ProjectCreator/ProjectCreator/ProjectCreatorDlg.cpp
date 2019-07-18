
// ProjectCreatorDlg.cpp : ���� ����
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

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CProjectCreatorDlg ��ȭ ����



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


// CProjectCreatorDlg �޽��� ó����

BOOL CProjectCreatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CProjectCreatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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

    String GetPath;
    if(Platform::Popup::FileDialog(DST_Dir, GetPath, nullptr, "������ Ǯ ������ �����ϼ���"))
    if(HRSRC ResInfo = FindResourceW(NULL, MAKEINTRESOURCEW(IDR_ZIP1), L"ZIP"))
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
