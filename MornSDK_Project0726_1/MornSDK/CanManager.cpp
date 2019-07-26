// CanManager.cpp : 实现文件
//

#include "stdafx.h"
#include "MornSDK.h"
#include "CanManager.h"
#include "General.h"
#include "MornSDKDlg.h"



//队列线程函数
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
				//获取队列第一个Task
				try
				{
					//弹出队列
					pManager->m_csQueueTask.Lock();
					CTask* pTask = pManager->queueTasks.front();
					pManager->queueTasks.pop();
					pManager->m_csQueueTask.Unlock();
					//csQueueTask.Unlock();
					//g_Log.WriteLog(LOG_DBG, "Queue count:%d", pManager->queueTasks.size());
					if (pManager->m_hDevice != INVALID_HANDLE_VALUE && pManager->m_hDevice != NULL)
					{
						//发送CAN帧
						OVERLAPPED ov;
						memset(&ov, 0, sizeof(OVERLAPPED));
						ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
						int ret = acCanWrite(pManager->m_hDevice, &pTask->m_CanMsgSend, 1, &ulWritten, &ov);

						CString msg; 
						msg.Format("%s %02X %02X %02X %02X %02X %02X %02X %02X %s %d", 
							"发送的数据帧为：", 
							pTask->m_CanMsgSend.data[0],
							pTask->m_CanMsgSend.data[1],
							pTask->m_CanMsgSend.data[2],
							pTask->m_CanMsgSend.data[3],
							pTask->m_CanMsgSend.data[4],
							pTask->m_CanMsgSend.data[5],
							pTask->m_CanMsgSend.data[6],
							pTask->m_CanMsgSend.data[7],
							"发送的状态字为 ", ret);
						//g_Log.WriteLog(LOG_INF, msg);

						if(ret == SUCCESS)	 //若失败
						{
							//接收回执
							memset(&ov, 0, sizeof(OVERLAPPED));
							ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
							ret = acCanRead(pManager->m_hDevice, &pTask->m_CanMsgRecv, 1, &ulRead, &ov);
							CloseHandle(ov.hEvent);

							if(ret == SUCCESS)
							{
								CString msg; 
								msg.Format("%s %02X %02X %02X %02X %02X %02X %02X %02X %s %d", 
									"接收的数据帧为：", 
									pTask->m_CanMsgRecv.data[0],
									pTask->m_CanMsgRecv.data[1],
									pTask->m_CanMsgRecv.data[2],
									pTask->m_CanMsgRecv.data[3],
									pTask->m_CanMsgRecv.data[4],
									pTask->m_CanMsgRecv.data[5],
									pTask->m_CanMsgRecv.data[6],
									pTask->m_CanMsgRecv.data[7],
									"接收的状态字为 ", ret);
								//g_Log.WriteLog(LOG_INF, msg);

								//调用回调函数
								pTask->pFuncCallback(pTask, "0");
								//g_Log.WriteLog(LOG_INF,"队列线程Task处理成功。");
							}
							else if (ret==TIME_OUT)
							{
								pTask->pFuncCallback(pTask, "-2");
								//g_Log.WriteLog(LOG_ERR,"队列线程Task接收超时。");
							}
							else
							{
								pTask->pFuncCallback(pTask, "-3");
								//g_Log.WriteLog(LOG_ERR,"队列线程Task接收失败。");
							}
						}
						else if (ret < 0)
						{
							//g_Log.WriteLog(LOG_ERR,"队列线程can发送失败。");
							pTask->pFuncCallback(pTask, "-1");
						}
					}
					else
					{
						pTask->pFuncCallback(pTask, "-9");
					}
					Sleep(5);
					////弹出队列
					////csQueueTask.Lock();
					//pManager->m_csQueueTask.Lock();
					//pManager->queueTasks.pop();
					//pManager->m_csQueueTask.Unlock();
					////csQueueTask.Unlock();
					//g_Log.WriteLog(LOG_DBG, "Queue count:%d", pManager->queueTasks.size());
				}
				catch (CException* e)
				{
					//AfxMessageBox("队列线程执行出现异常", MB_OK);
					g_Log.WriteLog(LOG_ERR,"队列线程出现异常。");
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


// CCanManager构造函数
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

//设置波特率
void CCanManager::SetBaud(uint nBaud)
{
	m_iBuad = nBaud;
}

//种子源输出电流设置
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
		pTask->m_CanMsgSend.flags = 0;  //标准帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		//测试成功与否
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("种子源输出电流设置 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("种子源输出电流设置 失败"));
		return -1;
	}
}

//种子源输出控制
int CCanManager::SendFrame_SeedSource_Output(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //标准帧
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X05;
		pTask->m_CanMsgSend.data[2] = 0X00;//整机
		pTask->m_CanMsgSend.data[3] = 0X11;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("种子源输出控制 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("种子源输出控制 失败"));
		return -1;
	}
}

//种子源停止输出
int CCanManager::SendFrame_SeedSource_StopOutput(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //标准帧
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X05;
		pTask->m_CanMsgSend.data[2] = 0X00;//整机
		pTask->m_CanMsgSend.data[3] = 0X22;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("种子源停止输出 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("种子源停止输出失败 失败"));
		return -1;
	}

}

//种子源复位指令
int CCanManager::Reset_SeedSource(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //标准帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("种子源复位指令 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("种子源复位指令 失败"));
		return -1;
	}
}

//激光器状态查询
int CCanManager::GetStatus_Laser(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //标准帧
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X0D;
		pTask->m_CanMsgSend.data[2] = 0X11;//0x11查询，其余无效
		pTask->m_CanMsgSend.data[3] = 0X00;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("激光器状态查询 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("激光器状态查询 失败"));
		return -1;
	}
}

//电池状态查询
int CCanManager::GetStatus_Battery(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = 0;  //标准帧
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0X0F;
		pTask->m_CanMsgSend.data[2] = 0X11;//0x11查询，其余无效
		pTask->m_CanMsgSend.data[3] = 0X00;
		pTask->m_CanMsgSend.data[4] = 0X00;
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager3->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("电池状态查询 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("电池状态查询 失败"));
		return -1;
	}
}

//驱动电源加载参数设置
int CCanManager::SetParameter_PowerSupply(BYTE id, int current1, int current2, int current3, int current4, int delay, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //标准帧
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0Xa0;
		pTask->m_CanMsgSend.data[2] = current1;
		pTask->m_CanMsgSend.data[3] = current2;
		pTask->m_CanMsgSend.data[4] = current3;
		pTask->m_CanMsgSend.data[5] = current4;
		pTask->m_CanMsgSend.data[6] = delay;
		pTask->m_CanMsgSend.data[7] = 0X00;//0x00参数设置
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("驱动电源加载参数设置 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("驱动电源加载参数设置 失败"));
		return -1;
	}
}

//驱动电源查询参数设置
int CCanManager::QuerySetParameter_PowerSupply(BYTE id, int current1, int current2, int current3, int current4, BYTE delay, int timeout,  void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //标准帧
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0XFE;
		pTask->m_CanMsgSend.data[1] = 0Xa0;
		pTask->m_CanMsgSend.data[2] = current1;
		pTask->m_CanMsgSend.data[3] = current2;
		pTask->m_CanMsgSend.data[4] = current3;
		pTask->m_CanMsgSend.data[5] = current4;
		pTask->m_CanMsgSend.data[6] = delay;
		pTask->m_CanMsgSend.data[7] = 0Xff;//0xff参数查询
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("驱动电源查询参数设置 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("驱动电源查询参数设置 失败"));
		return -1;
	}
}

//驱动电源输出控制
int CCanManager::Output_PowerSupply(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //标准帧
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0xfe;
		pTask->m_CanMsgSend.data[1] = 0X03;
		pTask->m_CanMsgSend.data[2] = 0X00;//整机
		pTask->m_CanMsgSend.data[3] = 0X01;//输出
		pTask->m_CanMsgSend.data[4] = 0X01;//输出
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("驱动电源输出控制 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("驱动电源输出控制 失败"));
		return -1;
	}
}

//驱动电源停止输出控制
int CCanManager::OutputStop_PowerSupply(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //标准帧
		pTask->m_CanMsgSend.id = id;
		pTask->m_CanMsgSend.length = 8;
		pTask->m_CanMsgSend.data[0] = 0xfe;
		pTask->m_CanMsgSend.data[1] = 0X03;
		pTask->m_CanMsgSend.data[2] = 0X00;
		pTask->m_CanMsgSend.data[3] = 0X00;//停止输出
		pTask->m_CanMsgSend.data[4] = 0X00;//停止输出
		pTask->m_CanMsgSend.data[5] = 0X00;
		pTask->m_CanMsgSend.data[6] = 0X00;
		pTask->m_CanMsgSend.data[7] = 0X00;
		pTask->pFuncCallback = g_pMainDlg->FeedBack;
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("驱动电源停止输出控制 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("驱动电源停止输出控制 失败"));
		return -1;
	}
}

//驱动电源复位
int CCanManager::Reset_PowerSupply(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //标准帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("驱动电源复位 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("驱动电源复位 失败"));
		return -1;
	}
}

//驱动电源状态查询
int CCanManager::GetStatus_PowerSupply(BYTE id, int timeout, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_szCanPort = (char*)pCallbackFunc;
		pTask->m_CanMsgSend.flags = 0;  //标准帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		if(pTask->m_szCanPort != "2")
			g_pCanManager1->PushTaskInQueue(pTask);
		else
			g_pCanManager2->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("驱动电源状态查询 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("驱动电源状态查询 失败"));
		return -1;
	}
}

//充电控制
int CCanManager::ChargeControl(DWORD id, int switchCtrl, int changeCtrl, int strong,int stongVoltage, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //扩展帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("充电控制 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("充电控制 失败"));
		return -1;
	}
}

//电池停止充电
int CCanManager::StopChargeControl(DWORD id, int switchCtrl, int changeCtrl, int strong,int stongVoltage, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //扩展帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("充电控制 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("充电控制 失败"));
		return -1;
	}
}

//电池放电
int CCanManager::DisChargeControl(DWORD id, int mode, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //扩展帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("电池放电 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("电池放电 失败"));
		return -1;
	}
}

//电池停止放电
int CCanManager::StopDisChargeControl(DWORD id, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //扩展帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("电池停止放电 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("电池停止放电 失败"));
		return -1;
	}
}

//电池充电参数设置
//UI提取出来是int，value需要转换，
int CCanManager::BatteryChargeParameter_Supply(DWORD id, int chargeCtrl, int chargeMode, int current, int power, int voltage, int chargeStateFlag, void* pCallbackFunc)
{
	try
	{
		CTask* pTask = new CTask();
		pTask->m_CanMsgSend.flags = MSG_EXT;  //扩展帧
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
		//入队列
		//如果入队列成功，返回0
		//否则返回-1
		g_pCanManager4->PushTaskInQueue(pTask);
		//g_Log.WriteLog(LOG_DBG, _T("电池充电参数设置 成功"));
		return 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("电池充电参数设置 失败"));
		return -1;
	}
}


//打开Can端口
BOOL CCanManager::OpenCanPort(CString canPortName, BOOL sync)
{
	try
	{
		int ret;
		//若已打开则先关闭
		if (m_hDevice != INVALID_HANDLE_VALUE && m_hDevice != NULL)
		{
			acCanClose(m_hDevice);
		}
		//打开
		//m_hDevice = acCanOpen((LPTSTR)(LPCTSTR)canPortName, sync);
		m_hDevice = acCanOpen((LPTSTR)(LPCTSTR)canPortName, sync);
		if (m_hDevice == INVALID_HANDLE_VALUE || m_hDevice == NULL)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("打开Can端口%s失败。"), (LPTSTR)(LPCTSTR)canPortName);
			return FALSE;
		}
		//保存端口名
		m_szPortName = canPortName;

		ret = acEnterResetMode(m_hDevice);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s： acEnterResetMode调用失败。"), m_szPortName);
			return FALSE;
		}
		//set param
		ret = acSetBaud(m_hDevice, m_iBuad);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s： acSetBaud调用失败。"), m_szPortName);
			return FALSE;
		}
		ret = acSetAcceptanceFilter(m_hDevice, 0xffffffff, 0xffffffff);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s： acSetAcceptanceFilter调用失败。"), m_szPortName);
			return FALSE;
		}
		ret = acSetListenOnlyMode(m_hDevice, 0);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s： acSetListenOnlyMode调用失败。"), m_szPortName);
			return FALSE;
		}
		ret = acSetSelfReception(m_hDevice, FALSE);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s： acSetSelfReception调用失败。"), m_szPortName);
			return FALSE;
		}
		ret = acSetAcceptanceFilterMode(m_hDevice, PELICAN_DUAL_FILTER);
		if(ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s： acSetAcceptanceFilterMode调用失败。"), m_szPortName);
			return FALSE;
		}
		ret = acEnterWorkMode(m_hDevice);
		if (ret < 0)
		{
			acCanClose(m_hDevice);
			m_hDevice = INVALID_HANDLE_VALUE;
			g_Log.WriteLog(LOG_ERR, _T("%s： acEnterWorkMode调用失败。"), m_szPortName);
			return FALSE;
		}
		g_Log.WriteLog(LOG_INF, _T("打开Can端口%s成功。"), m_szPortName);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("打开Can端口%s时出现异常。"), m_szPortName);
		return FALSE;
	}
}

//关闭Can端口
BOOL CCanManager::CloseCanPort()
{
	try
	{
		acCanClose(m_hDevice);
		m_hDevice = INVALID_HANDLE_VALUE;
		g_Log.WriteLog(LOG_INF, _T("已关闭Can端口%s。"), m_szPortName);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("关闭Can端口%s时出现异常。"), m_szPortName);
		return FALSE;
	}
}

//将任务压入队列
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
			//g_Log.WriteLog(LOG_INF,"任务压入队列成功。");
			return TRUE;
		}
		else
		{
			g_Log.WriteLog(LOG_ERR,"试图将NULL任务压入队列时失败。");
			return FALSE;
		}		
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR,"将任务压入队列时出现异常。");
		return FALSE;
	}
}


