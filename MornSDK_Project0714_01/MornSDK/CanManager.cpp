// CanManager.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MornSDK.h"
#include "CanManager.h"
#include "General.h"
#include "MornSDKDlg.h"



//�����̺߳���
UINT ThreadQueueFunc(LPVOID pParam)
{
	DWORD ulWritten;
	DWORD ulRead;
	CCanManager* pManager = (CCanManager*)pParam;
	while(g_bCanManagerThreadReadFlag)
	{	
		if(pManager != NULL)
		{
			if (!pManager->queueTasks.empty())
			{
				//��ȡ���е�һ��Task
				try
				{
					//��������
					pManager->m_csQueueTask.Lock();
					CTask* pTask = pManager->queueTasks.front();
					pManager->queueTasks.pop();
					pManager->m_csQueueTask.Unlock();
					//csQueueTask.Unlock();
					//g_Log.WriteLog(LOG_DBG, "Queue count:%d", pManager->queueTasks.size());
					if (pManager->m_hDevice != INVALID_HANDLE_VALUE && pManager->m_hDevice != NULL)
					{
						//����CAN֡
						OVERLAPPED ov;
						memset(&ov, 0, sizeof(OVERLAPPED));
						ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
						int ret = acCanWrite(pManager->m_hDevice, &pTask->m_CanMsgSend, 1, &ulWritten, &ov);

						CString msg; 
						msg.Format("%s %02X %02X %02X %02X %02X %02X %02X %02X %s %d", 
							"���͵�����֡Ϊ��", 
							pTask->m_CanMsgSend.data[0],
							pTask->m_CanMsgSend.data[1],
							pTask->m_CanMsgSend.data[2],
							pTask->m_CanMsgSend.data[3],
							pTask->m_CanMsgSend.data[4],
							pTask->m_CanMsgSend.data[5],
							pTask->m_CanMsgSend.data[6],
							pTask->m_CanMsgSend.data[7],
							"���͵�״̬��Ϊ ", ret);
						//g_Log.WriteLog(LOG_INF, msg);

						if(ret == SUCCESS)	 //��ʧ��
						{
							//���ջ�ִ
							memset(&ov, 0, sizeof(OVERLAPPED));
							ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
							ret = acCanRead(pManager->m_hDevice, &pTask->m_CanMsgRecv, 1, &ulRead, &ov);
							CloseHandle(ov.hEvent);

							if(ret == SUCCESS)
							{
								CString msg; 
								msg.Format("%s %02X %02X %02X %02X %02X %02X %02X %02X %s %d", 
									"���յ�����֡Ϊ��", 
									pTask->m_CanMsgRecv.data[0],
									pTask->m_CanMsgRecv.data[1],
									pTask->m_CanMsgRecv.data[2],
									pTask->m_CanMsgRecv.data[3],
									pTask->m_CanMsgRecv.data[4],
									pTask->m_CanMsgRecv.data[5],
									pTask->m_CanMsgRecv.data[6],
									pTask->m_CanMsgRecv.data[7],
									"���յ�״̬��Ϊ ", ret);
								//g_Log.WriteLog(LOG_INF, msg);

								//���ûص�����
								pTask->pFuncCallback(pTask, "0");
								//g_Log.WriteLog(LOG_INF,"�����߳�Task����ɹ���");
							}
							else if (ret==TIME_OUT)
							{
								pTask->pFuncCallback(pTask, "-2");
								//g_Log.WriteLog(LOG_ERR,"�����߳�Task���ճ�ʱ��");
							}
							else
							{
								pTask->pFuncCallback(pTask, "-3");
								//g_Log.WriteLog(LOG_ERR,"�����߳�Task����ʧ�ܡ�");
							}
						}
						else if (ret < 0)
						{
							//g_Log.WriteLog(LOG_ERR,"�����߳�can����ʧ�ܡ�");
							pTask->pFuncCallback(pTask, "-1");
						}
					}
					else
					{
						pTask->pFuncCallback(pTask, "-9");
					}
					Sleep(5);
					////��������
					////csQueueTask.Lock();
					//pManager->m_csQueueTask.Lock();
					//pManager->queueTasks.pop();
					//pManager->m_csQueueTask.Unlock();
					////csQueueTask.Unlock();
					//g_Log.WriteLog(LOG_DBG, "Queue count:%d", pManager->queueTasks.size());
				}
				catch (CException* e)
				{
					//AfxMessageBox("�����߳�ִ�г����쳣", MB_OK);
					g_Log.WriteLog(LOG_ERR,"�����̳߳����쳣��");
				}
			}
			else
			{
				Sleep(5);
			}
		}
	}
	//delete pManager;
	//pManager = NULL;
	return 0;
}


// CCanManager���캯��
CCanManager::CCanManager()
{
	m_hDevice = INVALID_HANDLE_VALUE;
	m_iBuad = 250;
	m_szPortName = _T("");
	m_pTaskCur = NULL;
	m_pThreadQueue = AfxBeginThread(ThreadQueueFunc, this);
}

CCanManager::~CCanManager()
{
}

//���ò�����
void CCanManager::SetBaud(uint nBaud)
{
	m_iBuad = nBaud;
}

//����Դ�����������
int CCanManager::Output_CurrentSupply(BYTE id, int current1, int current2, int current3, int current4, int timeout, void* pCallbackFunc)
{
	//typedef struct {
	//	int                     flags;                  /* Flags, indicating or controlling special message properties */
	//	int                     cob;                    /* CAN object number, used in Full CAN */
	//	ULONG                   id;                     /* CAN message ID, 4 bytes  */
	//	short int               length;                 /* Number of bytes in the CAN message */
	//	UCHAR                   data[DATALENGTH];       /* Data, 0...8 bytes */
	//} canmsg_t;
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X03;
		pTask->m_CanMsgSend.data[2] = 0X00;
		pTask->m_CanMsgSend.data[3] = current1;
		pTask->m_CanMsgSend.data[4] = current2;
		pTask->m_CanMsgSend.data[5] = current3;
		pTask->m_CanMsgSend.data[6] = current4;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		//���Գɹ����
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("����Դ����������� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("����Դ����������� ʧ��"));
		return -1;
	}
}

//����Դ�������
int CCanManager::SendFrame_SeedSource_Output(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X05;
		pTask->m_CanMsgSend.data[2] = 0X00;//����
		pTask->m_CanMsgSend.data[3] = 0X11;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("����Դ������� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("����Դ������� ʧ��"));
		return -1;
	}
}

//����Դֹͣ���
int CCanManager::SendFrame_SeedSource_StopOutput(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X05;
		pTask->m_CanMsgSend.data[2] = 0X00;//����
		pTask->m_CanMsgSend.data[3] = 0X22;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("����Դֹͣ��� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("����Դֹͣ���ʧ�� ʧ��"));
		return -1;
	}

}

//����Դ��λָ��
int CCanManager::Reset_SeedSource(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X07;
		pTask->m_CanMsgSend.data[2] = 0X11;//reset
		pTask->m_CanMsgSend.data[3] = 0X00;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("����Դ��λָ�� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("����Դ��λָ�� ʧ��"));
		return -1;
	}
}

//������״̬��ѯ
int CCanManager::GetStatus_Laser(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X0D;
		pTask->m_CanMsgSend.data[2] = 0X11;//0x11��ѯ��������Ч
		pTask->m_CanMsgSend.data[3] = 0X00;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������״̬��ѯ �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������״̬��ѯ ʧ��"));
		return -1;
	}
}

//���״̬��ѯ
int CCanManager::GetStatus_Battery(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X0F;
		pTask->m_CanMsgSend.data[2] = 0X11;//0x11��ѯ��������Ч
		pTask->m_CanMsgSend.data[3] = 0X00;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("���״̬��ѯ �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("���״̬��ѯ ʧ��"));
		return -1;
	}
}

//������Դ���ز�������
int CCanManager::SetParameter_PowerSupply(BYTE id, int current1, int current2, int current3, int current4, int delay, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0Xa0;
		pTask->m_CanMsgSend.data[2] = current1;
		pTask->m_CanMsgSend.data[3] = current2;
		pTask->m_CanMsgSend.data[4] = current3;
		pTask->m_CanMsgSend.data[5] = current4;
		pTask->m_CanMsgSend.data[6] = delay;
		pTask->m_CanMsgSend.data[7] = 0X00;//0x00��������
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������Դ���ز������� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������Դ���ز������� ʧ��"));
		return -1;
	}
}

//������Դ��ѯ��������
int CCanManager::QuerySetParameter_PowerSupply(BYTE id, int current1, int current2, int current3, int current4, BYTE delay, int timeout,  void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0Xa0;
		pTask->m_CanMsgSend.data[2] = current1;
		pTask->m_CanMsgSend.data[3] = current2;
		pTask->m_CanMsgSend.data[4] = current3;
		pTask->m_CanMsgSend.data[5] = current4;
		pTask->m_CanMsgSend.data[6] = delay;
		pTask->m_CanMsgSend.data[7] = 0Xff;//0xff������ѯ
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������Դ��ѯ�������� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������Դ��ѯ�������� ʧ��"));
		return -1;
	}
}

//������Դ�������
int CCanManager::Output_PowerSupply(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0xfe;
		pTask->m_CanMsgSend.data[1] = 0X03;
		pTask->m_CanMsgSend.data[2] = 0X00;//����
		pTask->m_CanMsgSend.data[3] = 0X01;//���
		pTask->m_CanMsgSend.data[4] = 0X01;//���
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������Դ������� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������Դ������� ʧ��"));
		return -1;
	}
}

//������Դֹͣ�������
int CCanManager::OutputStop_PowerSupply(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0xfe;
		pTask->m_CanMsgSend.data[1] = 0X03;
		pTask->m_CanMsgSend.data[2] = 0X00;
		pTask->m_CanMsgSend.data[3] = 0X00;//ֹͣ���
		pTask->m_CanMsgSend.data[4] = 0X00;//ֹͣ���
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������Դֹͣ������� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������Դֹͣ������� ʧ��"));
		return -1;
	}
}

//������Դ��λ
int CCanManager::Reset_PowerSupply(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0xfe;
		pTask->m_CanMsgSend.data[1] = 0X05;
		pTask->m_CanMsgSend.data[2] = 0X00;
		pTask->m_CanMsgSend.data[3] = 0X00;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������Դ��λ �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������Դ��λ ʧ��"));
		return -1;
	}
}

//������Դ״̬��ѯ
int CCanManager::GetStatus_PowerSupply(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //��׼֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X07;
		pTask->m_CanMsgSend.data[2] = 0X00;
		pTask->m_CanMsgSend.data[3] = 0X00;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������Դ״̬��ѯ �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������Դ״̬��ѯ ʧ��"));
		return -1;
	}
}

//������
int CCanManager::ChargeControl(DWORD id, int switchCtrl, int changeCtrl, int strong,int stongVoltage, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //��չ֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		/*pTask->m_CanMsgSend.data[0] = (BYTE)switchCtrl;
		pTask->m_CanMsgSend.data[1] = (BYTE)changeCtrl;
		pTask->m_CanMsgSend.data[2] = (BYTE)strong;
		pTask->m_CanMsgSend.data[3] = BYTE(stongVoltage&0x00ff);
		pTask->m_CanMsgSend.data[4] = BYTE(stongVoltage&0xff00);*/
		pTask->m_CanMsgSend.data[0] = (BYTE)switchCtrl;
		pTask->m_CanMsgSend.data[1] = (BYTE)changeCtrl;
		pTask->m_CanMsgSend.data[2] = (BYTE)strong;
		pTask->m_CanMsgSend.data[3] = (BYTE)stongVoltage;
		pTask->m_CanMsgSend.data[4] = (BYTE)stongVoltage;
		pTask->m_CanMsgSend.data[5] =  00;
		pTask->m_CanMsgSend.data[6] =  00;
		pTask->m_CanMsgSend.data[7] =  00;

		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������ �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������ ʧ��"));
		return -1;
	}
}

//���ֹͣ���
int CCanManager::StopChargeControl(DWORD id, int switchCtrl, int changeCtrl, int strong,int stongVoltage, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //��չ֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		/*pTask->m_CanMsgSend.data[0] = (BYTE)switchCtrl;
		pTask->m_CanMsgSend.data[1] = (BYTE)changeCtrl;
		pTask->m_CanMsgSend.data[2] = (BYTE)strong;
		pTask->m_CanMsgSend.data[3] = BYTE(stongVoltage&0x00ff);
		pTask->m_CanMsgSend.data[4] = BYTE(stongVoltage&0xff00);*/
		pTask->m_CanMsgSend.data[0] = (BYTE)switchCtrl;
		pTask->m_CanMsgSend.data[1] = (BYTE)changeCtrl;
		pTask->m_CanMsgSend.data[2] = (BYTE)strong;
		pTask->m_CanMsgSend.data[3] = (BYTE)stongVoltage;
		pTask->m_CanMsgSend.data[4] = (BYTE)stongVoltage;
		pTask->m_CanMsgSend.data[5] =  00;
		pTask->m_CanMsgSend.data[6] =  00;
		pTask->m_CanMsgSend.data[7] =  00;

		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("������ �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("������ ʧ��"));
		return -1;
	}
}

//��طŵ�
int CCanManager::DisChargeControl(DWORD id, int mode, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //��չ֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = (BYTE)mode;
		pTask->m_CanMsgSend.data[1] = 0x00;
		pTask->m_CanMsgSend.data[2] = 0x00;
		pTask->m_CanMsgSend.data[3] = 0x00;
		pTask->m_CanMsgSend.data[4] = 0x00;
		pTask->m_CanMsgSend.data[5] = 0x00;
		pTask->m_CanMsgSend.data[6] = 0x00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("��طŵ� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("��طŵ� ʧ��"));
		return -1;
	}
}

//���ֹͣ�ŵ�
int CCanManager::StopDisChargeControl(DWORD id, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //��չ֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0x00;
		pTask->m_CanMsgSend.data[1] = 0x00;
		pTask->m_CanMsgSend.data[2] = 0x00;
		pTask->m_CanMsgSend.data[3] = 0x00;
		pTask->m_CanMsgSend.data[4] = 0x00;
		pTask->m_CanMsgSend.data[5] = 0x00;
		pTask->m_CanMsgSend.data[6] = 0x00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("���ֹͣ�ŵ� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("���ֹͣ�ŵ� ʧ��"));
		return -1;
	}
}

//��س���������
//UI��ȡ������int��value��Ҫת����
int CCanManager::BatteryChargeParameter_Supply(DWORD id, int chargeCtrl, int chargeMode, int current, int power, int voltage, int chargeStateFlag, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //��չ֡
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = (BYTE)chargeCtrl;
		pTask->m_CanMsgSend.data[1] = (BYTE)chargeMode;
		pTask->m_CanMsgSend.data[2] = (BYTE)(current*2);
		pTask->m_CanMsgSend.data[3] = (BYTE)(power/25);
		pTask->m_CanMsgSend.data[4] = BYTE((voltage*10)>>8);
		pTask->m_CanMsgSend.data[5] = BYTE((voltage*10)%256);
		pTask->m_CanMsgSend.data[6] = (BYTE)chargeStateFlag;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//�����
		//�������гɹ�������0
		//���򷵻�-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("��س��������� �ɹ�"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("��س��������� ʧ��"));
		return -1;
	}
}


//��Can�˿�
BOOL CCanManager::OpenCanPort(CString canPortName, BOOL sync)
{
	try
	{
		int ret;
		//���Ѵ����ȹر�
		if (m_hDevice != INVALID_HANDLE_VALUE && m_hDevice != NULL)
		{
			acCanClose(m_hDevice);
		}
		//��
		//m_hDevice = acCanOpen((LPTSTR)(LPCTSTR)canPortName, sync);
		m_hDevice = acCanOpen((LPTSTR)(LPCTSTR)canPortName, sync);
		if (m_hDevice == INVALID_HANDLE_VALUE || m_hDevice == NULL)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("��Can�˿�%sʧ�ܡ�"), (LPTSTR)(LPCTSTR)canPortName);
			return FALSE;
		}
		//����˿���
		m_szPortName = canPortName;

		ret = acEnterResetMode(m_hDevice);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s�� acEnterResetMode����ʧ�ܡ�"), m_szPortName);
			return FALSE;
		}
		//set param
		ret = acSetBaud(m_hDevice, m_iBuad);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s�� acSetBaud����ʧ�ܡ�"), m_szPortName);
			return FALSE;
		}
		ret = acSetAcceptanceFilter(m_hDevice, 0xffffffff, 0xffffffff);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s�� acSetAcceptanceFilter����ʧ�ܡ�"), m_szPortName);
			return FALSE;
		}
		ret = acSetListenOnlyMode(m_hDevice, 0);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s�� acSetListenOnlyMode����ʧ�ܡ�"), m_szPortName);
			return FALSE;
		}
		ret = acSetSelfReception(m_hDevice, FALSE);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s�� acSetSelfReception����ʧ�ܡ�"), m_szPortName);
			return FALSE;
		}
		ret = acSetAcceptanceFilterMode(m_hDevice, PELICAN_DUAL_FILTER);
		if(ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s�� acSetAcceptanceFilterMode����ʧ�ܡ�"), m_szPortName);
			return FALSE;
		}
		ret = acEnterWorkMode(m_hDevice);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s�� acEnterWorkMode����ʧ�ܡ�"), m_szPortName);
			return FALSE;
		}
		g_Log.WriteLog(LOG_INF, _T("��Can�˿�%s�ɹ���"), m_szPortName);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("��Can�˿�%sʱ�����쳣��"), m_szPortName);
		return FALSE;
	}
}

//�ر�Can�˿�
BOOL CCanManager::CloseCanPort()
{
	try
	{
		acCanClose(m_hDevice);
		m_hDevice = INVALID_HANDLE_VALUE;
		g_Log.WriteLog(LOG_INF, _T("�ѹر�Can�˿�%s��"), m_szPortName);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("�ر�Can�˿�%sʱ�����쳣��"), m_szPortName);
		return FALSE;
	}
}

//������ѹ�����
BOOL CCanManager::PushTaskInQueue(CTask* pTask)
{
	try
	{
		if (pTask != NULL)
		{
			/*csQueueTask.Lock();
			queueTasks.push(pTask);
			csQueueTask.Unlock();*/
			m_csQueueTask.Lock();
			queueTasks.push(pTask);
			m_csQueueTask.Unlock();
			//g_Log.WriteLog(LOG_INF,"����ѹ����гɹ���");
			return TRUE;
		}
		else
		{
			g_Log.WriteLog(LOG_ERR,"��ͼ��NULL����ѹ�����ʱʧ�ܡ�");
			return FALSE;
		}		
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR,"������ѹ�����ʱ�����쳣��");
		return FALSE;
	}
}


