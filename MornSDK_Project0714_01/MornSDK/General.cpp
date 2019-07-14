/*
 * General.cpp
 *
 *  Created on: 2019年3月4日
 *      Author: YWJ
 */
#include "General.h"


//全局主窗口指针
CMornSDKDlg* g_pMainDlg = NULL;
//全局Can管理者1

//电源A CAN1
CCanManager* g_pCanManager1 = NULL;
//电源B CAN2
CCanManager* g_pCanManager2 = NULL;
//种子源 CAN3
CCanManager* g_pCanManager3 = NULL;
//电池  CAN4
CCanManager* g_pCanManager4 = NULL;

//配置管理类对象
CIniReader g_Config;
//试验配置文件
CIniReader g_CfgTask;
//日志对象
CMyLog g_Log;

//服务器IP及端口号
CString g_strServerIP = "127.0.0.1";
int g_iRemotePort = 6666;
//本地端口（作为UDP服务器时）
int g_iLocalPort = 5555;
//网络数据接收延时(ms)
int g_iNetTimeOut = 30;

//时序控制器A IP及端口号
CString g_sxAServerIP = "127.0.0.1";
int g_isxARemotePort = 1111;
//本地端口（作为UDP服务器时）
int g_isxALocalPort = 2222;
//网络数据接收延时(ms)
int g_isxANetTimeOut = 30;

//时序控制器B IP及端口号
CString g_sxBServerIP = "127.0.0.1";
int g_isxBRemotePort = 1111;
//本地端口（作为UDP服务器时）
int g_isxBLocalPort = 2222;
//网络数据接收延时(ms)
int g_isxBNetTimeOut = 30;

//温湿度 IP及端口号
CString g_WsdServerIP = "192.168.3.53";
int g_WsdPort = 502;
//网络数据接收延时(ms)
int g_WsdNetTimeOut = 30;


//Dll所在路径
CString g_strExePath = _T("./");
//串口类对象
CSerialPort* g_pSP = NULL;

//串口类对象
cnComm* g_pCnCommToUp = NULL;
cnComm* g_pCnCommToDown = NULL;
//串口号
int g_iComPortToUp = 2;
int g_iComPortToDown = 3;
//程序运行标识
BOOL g_bAppRun = TRUE;
//
//YWJ----------------
//IO卡对象
CMIC3780* g_pIoCard = NULL;
//YWJ----------------

//UDP上传状态
BYTE g_stateToUp[294] = {0};
//本地数据存储全局变量
CString g_localDataLog[170];

BYTE g_batState[11]={0};
BOOL g_isbatDisChargeState[11] = {FALSE};

//充电许可全局变量
int g_chargeEnableFlag = -1;
//电池电压全局变量
int g_batVoltage[11] = {0};
//热启动flag
int g_hotBoot = 0;
int g_batoutput = 0;
//母线正常全局数组
BOOL g_muxianOK[21] = {FALSE};
//预充正常全局数组
BOOL g_yuchongOK[11] = {FALSE};

//当前任务编号全局变量
extern CString g_nowTaskNum = "";

//当前任务预放间隔时间
extern int g_delay = 0;
//电池状态机
extern int g_BatRelayState[11] = {0};
//判断是够接收到强光指令
BOOL g_isQgCmd_Aoutput = FALSE;
BOOL g_isQgCmd_Astop = FALSE;
BOOL g_isQgCmd_Boutput = FALSE;
BOOL g_isQgCmd_Bstop = FALSE;

BOOL g_isQgCmd_AStop = FALSE;
BOOL g_isQgCmd_BStop = FALSE;

int g_isQgAllFlag = 0;
//计数三秒
extern int counter = 0;
//UDP弱光记录
int g_ruoGuangValueA = 0;
int g_ruoGuangValueB = 0;
//任务下方标志位
int g_taskDown = 0;
//强光控制标志位
int g_qgCtrl = 0;


//版本号
CString g_Version = _T("0.1.4.7");

//临界区
CCriticalSection csQueueTask;

//queue<CTask*> TaskQueue;


//字符串以十六进制形式表示
void StrToHex(const char *str, char * rc)
{
	int len = strlen(str);
	for (int i=0; i < len; i++)
	{
		char tmp[6] = {0};
		sprintf(tmp, "0X%02X ", (unsigned char)str[i]);
		strcat(rc,tmp);
	}
}

CString strToHex(uchar* data, int len)
{
	CString str, tem;
	for(int i= 0; i < len; i++)
	{
		tem.Format("%02x ", data[i]);
		str+= tem;
	}
	return str;
}


//! 十六进制字符串->二进制数据-------------------------------------------------------
BYTE HexStrToBin(LPCTSTR strHexIn)
{
	CString strHex = strHexIn;
	int iLength = strHex.GetLength();
	int iRes =-1; 
	BYTE iReturn=0;
	for (int i=0; i<iLength; i++)
	{
		switch(strHex[i])
		{
		case _T('0'):	
		case _T('1'):
		case _T('2'):
		case _T('3'):
		case _T('4'):
		case _T('5'):
		case _T('6'):
		case _T('7'):
		case _T('8'):
		case _T('9'):
			iRes = strHex[i] - _T('0');
			iRes = (int)pow(16.0,(iLength-1)-i) * iRes;
			break;
		case 'A':
		case 'a':
			iRes = (int)pow(16.0,(iLength-1)-i) * 10;
			break;
		case 'B':
		case 'b':
			iRes = (int)pow(16.0,(iLength-1)-i) * 11;
			break;
		case 'C':
		case 'c':
			iRes = (int)pow(16.0,(iLength-1)-i) * 12;
			break;
		case 'D':
		case 'd':
			iRes = (int)pow(16.0,(iLength-1)-i) * 13;
			break;
		case 'E':
		case 'e':
			iRes = (int)pow(16.0,(iLength-1)-i) * 14;
			break;
		case 'F':
		case 'f':
			iRes = (int)pow(16.0,(iLength-1)-i) * 15;
			break;
		default:
			return -1;
		}
		if (iRes != -1)
			iReturn += iRes;
		iRes = -1;
	}
	return iReturn;
}

uint checkSum16(uchar* data, int len)
{
	uint sum = 0;
	for(int i = 0; i < len; i++)
	{
		sum += data[i];
	}
	return sum;
}

//! 二进制数组->十六进制字符表示--------------------------------------------
void BinArrayToHexStr(BYTE* pHex, LPTSTR strHex, int iLen)
{
	char strTmp[4];
	for (int i=0; i < iLen; i++)
	{
		sprintf(strTmp, "%02X ", pHex[i]);
		strTmp[3] = '\0';
		strcat(strHex, strTmp);
	}
}


//! 十六进制字符->二进制数据数组--------------------------------------------
int HexStrToBinArray(LPCTSTR strChar, BYTE* pHex)
{
	int j=0;
	CString str = strChar;
	CString strtmp;
	for (int i=0; i<str.GetLength(); )
	{
		if (_T(' ')==str[i])
		{
			i++;
			continue;
		}
		strtmp = str.Mid(i,2);
		if (HexStrToBin(strtmp) == -1)
		{
			return -1;
		}
		pHex[j] = HexStrToBin(strtmp);
		i+=2;
		j++;
	}
	return j;
}

//读文件
CString ReadFile(CString filePath)
{
	//-----------------------------------------------
	// CStdioFile读文件示例
	// 实现功能:打开已存在文件并逐行读取文件内容
	// 判断文件是否存在
	bool bFileExist = PathFileExists(filePath) 
		&& (!PathIsDirectory(filePath));
	if (bFileExist == false)
	{
		return "";
	}
	// 打开文件
	CStdioFile file;
	BOOL ret = file.Open(filePath, CFile::modeRead | CFile::shareDenyNone);
	if (!ret)
	{
		CString str;
		str.Format("打开%s文件失败(FileRead)",filePath);
		AfxMessageBox(str,MB_OK);
		g_Log.WriteLog(LOG_ERR, str);
		return "";
	}
	file.SeekToBegin();

	// 循环读取文件
	CString cstrLine;
	CString text;
	while (file.ReadString(cstrLine))
	{
		//AfxMessageBox(cstrLine);
		text += cstrLine + "\r\n";
	}
	// 关闭文件
	file.Close();
	return text;
}

//写文件
int WriteFile(CString filePath, CString content)
{
	try
	{
		if (PathFileExists(filePath) && !PathIsDirectory(filePath))
		{
			DWORD dwAttrs = GetFileAttributes(filePath);
			if (dwAttrs != INVALID_FILE_ATTRIBUTES
				&& (dwAttrs & FILE_ATTRIBUTE_READONLY))
			{
				dwAttrs &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(filePath, dwAttrs);
			}
		}
		CString str;
		// 打开文件
		CStdioFile file;
		BOOL ret = file.Open(filePath, 
			CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite);
		if (!ret)
		{
			str.Format("打开%s文件失败(FileWrite)",filePath);
			//AfxMessageBox(str,MB_OK);
			g_Log.WriteLog(LOG_ERR, str);
			return -1;
		}
		file.SeekToEnd();
		//  写文件
		file.WriteString(content);
		// 关闭文件
		file.Close();
		return 0;
	}
	catch (CException* e)
	{
		CString str;
		str.Format("打开%s文件失败(FileWrite)",filePath);
		g_Log.WriteLog(LOG_ERR, str);
		return -1;
	}
}


BOOL g_bCanManagerThreadReadFlag = TRUE;
BOOL g_bBatThreadReadFlag = TRUE;




