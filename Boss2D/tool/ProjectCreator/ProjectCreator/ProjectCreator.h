
// ProjectCreator.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CProjectCreatorApp:
// �� Ŭ������ ������ ���ؼ��� ProjectCreator.cpp�� �����Ͻʽÿ�.
//

class CProjectCreatorApp : public CWinApp
{
public:
	CProjectCreatorApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CProjectCreatorApp theApp;