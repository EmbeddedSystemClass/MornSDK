/*
 * General.h
 *
 *  Created on: 2019年3月4日
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

//IO卡
#pragma comment(lib,"./MIC3780/lib/Adsapi32.lib")
//#include "pthread/include/pthread.h"
//#pragma comment(lib,"./pthread/lib/x86/pthreadVC2.lib")


//typedef queue<CTask*>  TaskQUEUE;

#include "General.h"


//全局主窗口指针
class CMornSDKDlg;
extern CMornSDKDlg* g_pMainDlg;
//全局Can管理者1
class CCanManager;
extern CCanManager* g_pCanManager1;
extern CCanManager* g_pCanManager2;
extern CCanManager* g_pCanManager3;
extern CCanManager* g_pCanManager4;
//配置管理类对象
extern CIniReader g_Config;
//试验配置文件
extern CIniReader g_CfgTask;
//日志对象
extern CMyLog g_Log;
//串口类对象
extern cnComm* g_pCnCommToUp;
extern cnComm* g_pCnCommToDown;
//串口号
extern int g_iComPortToUp;
extern int g_iComPortToDown;

//串口类对象
extern CSerialPort* g_pSP;
//YWJ----------------
//IO卡对象
extern CMIC3780* g_pIoCard;
//YWJ----------------
//种子源配置参数信息
extern int SeedSource_Parameter[8];
//驱动源配置参数
extern int PowerSupply_Parameter[20];

extern vector<vector<int>> g_seedsourceA;
extern vector<vector<int>> g_seedsourceB;
extern vector<vector<int>> g_powersupplyA;
extern vector<vector<int>> g_powersupplyB;
extern vector<vector<int>> g_soc;

//UDP上传状态
extern BYTE g_stateToUp[294];
//本地数据存储全局变量
extern CString g_localDataLog[170];
//UDP弱光记录
extern int g_ruoGuangValueA;
extern int g_ruoGuangValueB;

//任务下方标志位
extern int g_taskDown;

//强光控制标志位
extern int g_qgCtrl;

extern BYTE g_batState[11];
extern BOOL g_isbatDisChargeState[11];

//充电许可全局变量
extern int g_chargeEnableFlag;
//电池电压全局变量
extern int g_batVoltage[11];
//热启动flag
extern int g_hotBoot;
extern int g_batoutput;
//母线正常全局数组
extern BOOL g_muxianOK[21];

//预充正常全局数组
extern BOOL g_yuchongOK[11];

//当前任务编号全局变量
extern CString g_nowTaskNum;

//当前任务预放间隔时间
extern int g_delay;
//电池状态机 0:无状态; 1:预充; 2:放电; 3:充电;
extern int g_BatRelayState[11];

//判断是够接收到强光指令
extern BOOL g_isQgCmd_Aoutput;
extern BOOL g_isQgCmd_Astop;
extern BOOL g_isQgCmd_Boutput;
extern BOOL g_isQgCmd_Bstop;

extern BOOL g_isQgCmd_AStop;
extern BOOL g_isQgCmd_BStop;

extern int g_isQgAllFlag;

//计数10秒判断电池放电状态
extern int counter;
//
//Dll所在路径
extern CString g_strExePath;

//内部版本号
extern CString g_Version;

//对上 IP及端口号
extern CString g_strServerIP;
extern int g_iRemotePort;
//本地端口（作为UDP服务器时）
extern int g_iLocalPort;
//数据接收超时
extern int g_iNetTimeOut;

//时序控制卡A IP及端口号
extern CString g_sxAServerIP;
extern int g_isxARemotePort;
//本地端口（作为UDP服务器时）
extern int g_isxALocalPort;
//数据接收超时
extern int g_isxANetTimeOut;

//时序控制卡B IP及端口号
extern CString g_sxBServerIP;
extern int g_isxBRemotePort;
//本地端口（作为UDP服务器时）
extern int g_isxBLocalPort;
//数据接收超时
extern int g_isxBNetTimeOut;

//温湿度 IP及端口号
extern CString g_WsdServerIP;
extern int g_WsdPort;
//网络数据接收延时(ms)
extern int g_WsdNetTimeOut;

//临界区
extern CCriticalSection csQueueTask;

//线程运行flag
extern BOOL g_bCanManagerThreadReadFlag;
extern BOOL g_bBatThreadReadFlag;
//程序运行标识（暂不用）
extern BOOL g_bAppRun;

//queue
//extern TaskQUEUE TaskQueue;

//字符串以十六进制形式表示
void StrToHex(const char *str, char * rc);
CString strToHex(uchar* data, int len);
//! 十六进制字符->二进制数据数组
int HexStrToBinArray(LPCTSTR strChar, BYTE* pHex);
//! 二进制数组->十六进制字符表示
void BinArrayToHexStr(BYTE* pHex, LPTSTR strHex, int iLen);
//! 十六进制字符串->二进制数据
BYTE HexStrToBin(LPCTSTR strHexIn);

//checkSum
uint checkSum16(uchar* data, int len);
//读文件
CString ReadFile(CString filePath);
//写文件
int WriteFile(CString filePath, CString content);
//获取文件大小
long GetFileSize(const char* file);





