#pragma once

#include "General.h"

class CTask;
typedef int(*TaskCallback)(CTask* pTask, void* pData);
//int CMornSDKDlg::FeedBack(CTask* pTask, void* pData)
class CTask : public CObject
{
public:
	//���캯��
	CTask();
	//��������
	virtual ~CTask();

	//����Task
	int Run();

public:
	//CAN�˿���
	CString m_szCanPort;
	//CAN��������
	canmsg_t m_CanMsgSend;
	//CAN��������
	canmsg_t m_CanMsgRecv;
	//�ص�����ָ��
	TaskCallback pFuncCallback;

	int index;
};


