// Task.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "Task.h"

// CTask
CTask::CTask()
{
	m_szCanPort = _T("");
	index = 0;
}

CTask::~CTask()
{
}

//����Task
int CTask::Run()
{
	try
	{
		pFuncCallback(this, _T("������"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("Task���й����г����쳣: id=%d"), this->m_CanMsgSend.id);
		return -1;
	}
}


// CTask ��Ա����
