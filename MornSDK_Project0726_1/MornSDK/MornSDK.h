
// MornSDK.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

// CMornSDKApp:
// �йش����ʵ�֣������ MornSDK.cpp
//

class CMornSDKApp : public CWinApp
{
public:
	CMornSDKApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMornSDKApp theApp;