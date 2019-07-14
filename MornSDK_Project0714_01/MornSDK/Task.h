#pragma once

#include "General.h"

class CTask;
typedef int(*TaskCallback)(CTask* pTask, void* pData);
//int CMornSDKDlg::FeedBack(CTask* pTask, void* pData)
class CTask : public CObject
{
public:
	//构造函数
	CTask();
	//析构函数
	virtual ~CTask();

	//运行Task
	int Run();

public:
	//CAN端口名
	CString m_szCanPort;
	//CAN发送数据
	canmsg_t m_CanMsgSend;
	//CAN接受数据
	canmsg_t m_CanMsgRecv;
	//回调函数指针
	TaskCallback pFuncCallback;

	int index;
};


