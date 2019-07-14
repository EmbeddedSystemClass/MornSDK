#pragma once

#include "General.h"
#include "Task.h"
// CCanManager 命令目标

class CCanManager : public CObject
{
public:
	CCanManager();
	virtual ~CCanManager();

	//打开Can端口
	BOOL OpenCanPort(CString canPortName, BOOL sync=FALSE);
	//关闭Can端口
	BOOL CloseCanPort();
	//设置波特率
	void SetBaud(uint nBaud);

	//输出电流设置
	int Output_CurrentSupply(BYTE id, int current1, int current2, int current3, int current4, int timeout, void* pCallbackFunc);
	//种子源输出控制
	int SendFrame_SeedSource_Output(BYTE id, int timeout, void* pCallbackFunc);
	//种子源停止输出
	int SendFrame_SeedSource_StopOutput(BYTE id, int timeout, void* pCallbackFunc);
	//种子源复位指令
	int Reset_SeedSource(BYTE id, int timeout, void* pCallbackFunc);
	//激光器状态查询
	int GetStatus_Laser(BYTE id, int timeout, void* pCallbackFunc);
	//电源状态查询
	int GetStatus_Battery(BYTE id, int timeout, void* pCallbackFunc);

	//驱动电源加载参数设置
	int SetParameter_PowerSupply(BYTE id, int current1, int current2, int current3, int current4, int delay, int timeout, void* pCallbackFunc);
	//驱动电源查询参数设置
	int QuerySetParameter_PowerSupply(BYTE id, int current1, int current2, int current3, int current4, BYTE delay, int timeout,  void* pCallbackFunc);
	//驱动电源输出控制
	int Output_PowerSupply(BYTE id, int timeout, void* pCallbackFunc);
	//驱动电源停止输出控制
	int OutputStop_PowerSupply(BYTE id, int timeout, void* pCallbackFunc);
	//驱动电源复位
	int Reset_PowerSupply(BYTE id, int timeout, void* pCallbackFunc);
	//驱动电源状态查询
	int GetStatus_PowerSupply(BYTE id, int timeout, void* pCallbackFunc);


	//充电控制
	int ChargeControl(DWORD id, int switchCtrl, int changeCtrl, int strong,int stongVoltage, void* pCallbackFunc);
	//电池充电参数设置
	int BatteryChargeParameter_Supply(DWORD id, int chargeCtrl, int chargeMode, int current, int power, int voltage, int chargeStateFlag, void* pCallbackFunc);
	//电池停止充电
	int StopChargeControl(DWORD id, int switchCtrl, int changeCtrl, int strong,int stongVoltage, void* pCallbackFunc);
	//电池放电参数设置
	int DisChargeControl(DWORD id, int mode, void* pCallbackFunc);
	//电池停止放电
	int StopDisChargeControl(DWORD id, void* pCallbackFunc);
	//将任务压入队列
	BOOL PushTaskInQueue(CTask* pTask);


public:
	//Can设备句柄
	HANDLE m_hDevice;
	//端口名称
	CString m_szPortName;
	//波特率
	unsigned int m_iBuad;
	//当前task
	CTask* m_pTaskCur;
	//队列线程
	CWinThread* m_pThreadQueue;
	//任务队列
	queue<CTask*> queueTasks;

	CCriticalSection m_csQueueTask;

	CWinThread* m_ThreadId;
};


