// Task.cpp : 实现文件
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

//运行Task
int CTask::Run()
{
	try
	{
		pFuncCallback(this, _T("哈哈哈"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("Task运行过程中出现异常: id=%d"), this->m_CanMsgSend.id);
		return -1;
	}
}


// CTask 成员函数
