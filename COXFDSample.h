
// COXFDSample.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CCOXFDSampleApp:
// �� Ŭ������ ������ ���ؼ��� COXFDSample.cpp�� �����Ͻʽÿ�.
//

class CCOXFDSampleApp : public CWinApp
{
public:
	CCOXFDSampleApp();

	virtual ~CCOXFDSampleApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CCOXFDSampleApp theApp;