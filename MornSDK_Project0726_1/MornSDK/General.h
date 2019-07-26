/*
 * General.h
 *
 *  Created on: 2019��3��4��
 *      Author: YWJ
 */

#pragma once

#define _DEBUG_
//#define _SIMULATION_
//#define _TEST_

#define uint unsigned int
#define uchar unsigned char
#define ulong unsigned long
#define ushort unsigned short

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <fcntl.h>
#include <sys/types.h>
#include <math.h>

#include "stdafx.h"
#include "INI.h"
#include "MyLog.h"
#include "cnComm.h"
#include "SerialPort/SerialPort.h"
#include "AdvCan/AdvCan.h"
#include "AdvCan/AdvCanIO.h"
#include "Task.h"
#include "UdpHelper.h"
#include "MIC3780.h"
#include "OphirLMMeasurement.h"
#include "BtnST.h"
#include "grid/GridCtrl.h"
#include "grid/GridCellCheck.h"
#include "grid/GridCellCombo.h"


using namespace std;
using namespace itas109;

//IO��
#pragma comment(lib,"./MIC3780/lib/Adsapi32.lib")
//#include "pthread/include/pthread.h"
//#pragma comment(lib,"./pthread/lib/x86/pthreadVC2.lib")


//typedef queue<CTask*>  TaskQUEUE;

#include "General.h"


//ȫ��������ָ��
class CMornSDKDlg;
extern CMornSDKDlg* g_pMainDlg;
//ȫ��Can������1
class CCanManager;
extern CCanManager* g_pCanManager1;
extern CCanManager* g_pCanManager2;
extern CCanManager* g_pCanManager3;
extern CCanManager* g_pCanManager4;
//���ù��������
extern CIniReader g_Config;
//���������ļ�
extern CIniReader g_CfgTask;
//��־����
extern CMyLog g_Log;
//���������
extern cnComm* g_pCnCommToUp;
extern cnComm* g_pCnCommToDown;
//���ں�
extern int g_iComPortToUp;
extern int g_iComPortToDown;

//���������
extern CSerialPort* g_pSP;
//YWJ----------------
//IO������
extern CMIC3780* g_pIoCard;
//YWJ----------------
//����Դ���ò�����Ϣ
extern int SeedSource_Parameter[8];
//����Դ���ò���
extern int PowerSupply_Parameter[20];

extern vector<vector<int>> g_seedsourceA;
extern vector<vector<int>> g_seedsourceB;
extern vector<vector<int>> g_powersupplyA;
extern vector<vector<int>> g_powersupplyB;
extern vector<vector<int>> g_soc;

//UDP�ϴ�״̬
extern BYTE g_stateToUp[294];
//�������ݴ洢ȫ�ֱ���
extern CString g_localDataLog[170];
//UDP�����¼
extern int g_ruoGuangValueA;
extern int g_ruoGuangValueB;

//�����·���־λ
extern int g_taskDown;

//ǿ����Ʊ�־λ
extern int g_qgCtrl;

extern BYTE g_batState[11];
extern BOOL g_isbatDisChargeState[11];

//������ȫ�ֱ���
extern int g_chargeEnableFlag;
//��ص�ѹȫ�ֱ���
extern int g_batVoltage[11];
//������flag
extern int g_hotBoot;
extern int g_batoutput;
//ĸ������ȫ������
extern BOOL g_muxianOK[21];

//Ԥ������ȫ������
extern BOOL g_yuchongOK[11];

//��ǰ������ȫ�ֱ���
extern CString g_nowTaskNum;

//��ǰ����Ԥ�ż��ʱ��
extern int g_delay;
//���״̬�� 0:��״̬; 1:Ԥ��; 2:�ŵ�; 3:���;
extern int g_BatRelayState[11];

//�ж��ǹ����յ�ǿ��ָ��
extern BOOL g_isQgCmd_Aoutput;
extern BOOL g_isQgCmd_Astop;
extern BOOL g_isQgCmd_Boutput;
extern BOOL g_isQgCmd_Bstop;

extern BOOL g_isQgCmd_AStop;
extern BOOL g_isQgCmd_BStop;

extern int g_isQgAllFlag;

//����10���жϵ�طŵ�״̬
extern int counter;
//
//Dll����·��
extern CString g_strExePath;

//�ڲ��汾��
extern CString g_Version;

//���� IP���˿ں�
extern CString g_strServerIP;
extern int g_iRemotePort;
//���ض˿ڣ���ΪUDP������ʱ��
extern int g_iLocalPort;
//���ݽ��ճ�ʱ
extern int g_iNetTimeOut;

//ʱ����ƿ�A IP���˿ں�
extern CString g_sxAServerIP;
extern int g_isxARemotePort;
//���ض˿ڣ���ΪUDP������ʱ��
extern int g_isxALocalPort;
//���ݽ��ճ�ʱ
extern int g_isxANetTimeOut;

//ʱ����ƿ�B IP���˿ں�
extern CString g_sxBServerIP;
extern int g_isxBRemotePort;
//���ض˿ڣ���ΪUDP������ʱ��
extern int g_isxBLocalPort;
//���ݽ��ճ�ʱ
extern int g_isxBNetTimeOut;

//��ʪ�� IP���˿ں�
extern CString g_WsdServerIP;
extern int g_WsdPort;
//�������ݽ�����ʱ(ms)
extern int g_WsdNetTimeOut;

//�ٽ���
extern CCriticalSection csQueueTask;

//�߳�����flag
extern BOOL g_bCanManagerThreadReadFlag;
extern BOOL g_bBatThreadReadFlag;
//�������б�ʶ���ݲ��ã�
extern BOOL g_bAppRun;

//queue
//extern TaskQUEUE TaskQueue;

//�ַ�����ʮ��������ʽ��ʾ
void StrToHex(const char *str, char * rc);
CString strToHex(uchar* data, int len);
//! ʮ�������ַ�->��������������
int HexStrToBinArray(LPCTSTR strChar, BYTE* pHex);
//! ����������->ʮ�������ַ���ʾ
void BinArrayToHexStr(BYTE* pHex, LPTSTR strHex, int iLen);
//! ʮ�������ַ���->����������
BYTE HexStrToBin(LPCTSTR strHexIn);

//checkSum
uint checkSum16(uchar* data, int len);
//���ļ�
CString ReadFile(CString filePath);
//д�ļ�
int WriteFile(CString filePath, CString content);
//��ȡ�ļ���С
long GetFileSize(const char* file);





