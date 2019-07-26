#pragma once

#include "General.h"
#include "Task.h"
// CCanManager ����Ŀ��

class CCanManager : public CObject
{
public:
	CCanManager();
	virtual ~CCanManager();

	//��Can�˿�
	BOOL OpenCanPort(CString canPortName, BOOL sync=FALSE);
	//�ر�Can�˿�
	BOOL CloseCanPort();
	//���ò�����
	void SetBaud(uint nBaud);

	//�����������
	int Output_CurrentSupply(BYTE id, int current1, int current2, int current3, int current4, int timeout, void* pCallbackFunc);
	//����Դ�������
	int SendFrame_SeedSource_Output(BYTE id, int timeout, void* pCallbackFunc);
	//����Դֹͣ���
	int SendFrame_SeedSource_StopOutput(BYTE id, int timeout, void* pCallbackFunc);
	//����Դ��λָ��
	int Reset_SeedSource(BYTE id, int timeout, void* pCallbackFunc);
	//������״̬��ѯ
	int GetStatus_Laser(BYTE id, int timeout, void* pCallbackFunc);
	//��Դ״̬��ѯ
	int GetStatus_Battery(BYTE id, int timeout, void* pCallbackFunc);

	//������Դ���ز�������
	int SetParameter_PowerSupply(BYTE id, int current1, int current2, int current3, int current4, int delay, int timeout, void* pCallbackFunc);
	//������Դ��ѯ��������
	int QuerySetParameter_PowerSupply(BYTE id, int current1, int current2, int current3, int current4, BYTE delay, int timeout,  void* pCallbackFunc);
	//������Դ�������
	int Output_PowerSupply(BYTE id, int timeout, void* pCallbackFunc);
	//������Դֹͣ�������
	int OutputStop_PowerSupply(BYTE id, int timeout, void* pCallbackFunc);
	//������Դ��λ
	int Reset_PowerSupply(BYTE id, int timeout, void* pCallbackFunc);
	//������Դ״̬��ѯ
	int GetStatus_PowerSupply(BYTE id, int timeout, void* pCallbackFunc);


	//������
	int ChargeControl(DWORD id, int switchCtrl, int changeCtrl, int strong,int stongVoltage, void* pCallbackFunc);
	//��س���������
	int BatteryChargeParameter_Supply(DWORD id, int chargeCtrl, int chargeMode, int current, int power, int voltage, int chargeStateFlag, void* pCallbackFunc);
	//���ֹͣ���
	int StopChargeControl(DWORD id, int switchCtrl, int changeCtrl, int strong,int stongVoltage, void* pCallbackFunc);
	//��طŵ��������
	int DisChargeControl(DWORD id, int mode, void* pCallbackFunc);
	//���ֹͣ�ŵ�
	int StopDisChargeControl(DWORD id, void* pCallbackFunc);
	//������ѹ�����
	BOOL PushTaskInQueue(CTask* pTask);


public:
	//Can�豸���
	HANDLE m_hDevice;
	//�˿�����
	CString m_szPortName;
	//������
	unsigned int m_iBuad;
	//��ǰtask
	CTask* m_pTaskCur;
	//�����߳�
	CWinThread* m_pThreadQueue;
	//�������
	queue<CTask*> queueTasks;

	CCriticalSection m_csQueueTask;

	CWinThread* m_ThreadId;
};


