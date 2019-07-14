/*
 * General.cpp
 *
 *  Created on: 2019��3��4��
 *      Author: YWJ
 */
#include "General.h"


//ȫ��������ָ��
CMornSDKDlg* g_pMainDlg = NULL;
//ȫ��Can������1

//��ԴA CAN1
CCanManager* g_pCanManager1 = NULL;
//��ԴB CAN2
CCanManager* g_pCanManager2 = NULL;
//����Դ CAN3
CCanManager* g_pCanManager3 = NULL;
//���  CAN4
CCanManager* g_pCanManager4 = NULL;

//���ù��������
CIniReader g_Config;
//���������ļ�
CIniReader g_CfgTask;
//��־����
CMyLog g_Log;

//������IP���˿ں�
CString g_strServerIP = "127.0.0.1";
int g_iRemotePort = 6666;
//���ض˿ڣ���ΪUDP������ʱ��
int g_iLocalPort = 5555;
//�������ݽ�����ʱ(ms)
int g_iNetTimeOut = 30;

//ʱ�������A IP���˿ں�
CString g_sxAServerIP = "127.0.0.1";
int g_isxARemotePort = 1111;
//���ض˿ڣ���ΪUDP������ʱ��
int g_isxALocalPort = 2222;
//�������ݽ�����ʱ(ms)
int g_isxANetTimeOut = 30;

//ʱ�������B IP���˿ں�
CString g_sxBServerIP = "127.0.0.1";
int g_isxBRemotePort = 1111;
//���ض˿ڣ���ΪUDP������ʱ��
int g_isxBLocalPort = 2222;
//�������ݽ�����ʱ(ms)
int g_isxBNetTimeOut = 30;

//��ʪ�� IP���˿ں�
CString g_WsdServerIP = "192.168.3.53";
int g_WsdPort = 502;
//�������ݽ�����ʱ(ms)
int g_WsdNetTimeOut = 30;


//Dll����·��
CString g_strExePath = _T("./");
//���������
CSerialPort* g_pSP = NULL;

//���������
cnComm* g_pCnCommToUp = NULL;
cnComm* g_pCnCommToDown = NULL;
//���ں�
int g_iComPortToUp = 2;
int g_iComPortToDown = 3;
//�������б�ʶ
BOOL g_bAppRun = TRUE;
//
//YWJ----------------
//IO������
CMIC3780* g_pIoCard = NULL;
//YWJ----------------

//UDP�ϴ�״̬
BYTE g_stateToUp[294] = {0};
//�������ݴ洢ȫ�ֱ���
CString g_localDataLog[170];

BYTE g_batState[11]={0};
BOOL g_isbatDisChargeState[11] = {FALSE};

//������ȫ�ֱ���
int g_chargeEnableFlag = -1;
//��ص�ѹȫ�ֱ���
int g_batVoltage[11] = {0};
//������flag
int g_hotBoot = 0;
int g_batoutput = 0;
//ĸ������ȫ������
BOOL g_muxianOK[21] = {FALSE};
//Ԥ������ȫ������
BOOL g_yuchongOK[11] = {FALSE};

//��ǰ������ȫ�ֱ���
extern CString g_nowTaskNum = "";

//��ǰ����Ԥ�ż��ʱ��
extern int g_delay = 0;
//���״̬��
extern int g_BatRelayState[11] = {0};
//�ж��ǹ����յ�ǿ��ָ��
BOOL g_isQgCmd_Aoutput = FALSE;
BOOL g_isQgCmd_Astop = FALSE;
BOOL g_isQgCmd_Boutput = FALSE;
BOOL g_isQgCmd_Bstop = FALSE;

BOOL g_isQgCmd_AStop = FALSE;
BOOL g_isQgCmd_BStop = FALSE;

int g_isQgAllFlag = 0;
//��������
extern int counter = 0;
//UDP�����¼
int g_ruoGuangValueA = 0;
int g_ruoGuangValueB = 0;
//�����·���־λ
int g_taskDown = 0;
//ǿ����Ʊ�־λ
int g_qgCtrl = 0;


//�汾��
CString g_Version = _T("0.1.4.7");

//�ٽ���
CCriticalSection csQueueTask;

//queue<CTask*> TaskQueue;


//�ַ�����ʮ��������ʽ��ʾ
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


//! ʮ�������ַ���->����������-------------------------------------------------------
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

//! ����������->ʮ�������ַ���ʾ--------------------------------------------
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


//! ʮ�������ַ�->��������������--------------------------------------------
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

//���ļ�
CString ReadFile(CString filePath)
{
	//-----------------------------------------------
	// CStdioFile���ļ�ʾ��
	// ʵ�ֹ���:���Ѵ����ļ������ж�ȡ�ļ�����
	// �ж��ļ��Ƿ����
	bool bFileExist = PathFileExists(filePath) 
		&& (!PathIsDirectory(filePath));
	if (bFileExist == false)
	{
		return "";
	}
	// ���ļ�
	CStdioFile file;
	BOOL ret = file.Open(filePath, CFile::modeRead | CFile::shareDenyNone);
	if (!ret)
	{
		CString str;
		str.Format("��%s�ļ�ʧ��(FileRead)",filePath);
		AfxMessageBox(str,MB_OK);
		g_Log.WriteLog(LOG_ERR, str);
		return "";
	}
	file.SeekToBegin();

	// ѭ����ȡ�ļ�
	CString cstrLine;
	CString text;
	while (file.ReadString(cstrLine))
	{
		//AfxMessageBox(cstrLine);
		text += cstrLine + "\r\n";
	}
	// �ر��ļ�
	file.Close();
	return text;
}

//д�ļ�
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
		// ���ļ�
		CStdioFile file;
		BOOL ret = file.Open(filePath, 
			CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite);
		if (!ret)
		{
			str.Format("��%s�ļ�ʧ��(FileWrite)",filePath);
			//AfxMessageBox(str,MB_OK);
			g_Log.WriteLog(LOG_ERR, str);
			return -1;
		}
		file.SeekToEnd();
		//  д�ļ�
		file.WriteString(content);
		// �ر��ļ�
		file.Close();
		return 0;
	}
	catch (CException* e)
	{
		CString str;
		str.Format("��%s�ļ�ʧ��(FileWrite)",filePath);
		g_Log.WriteLog(LOG_ERR, str);
		return -1;
	}
}


BOOL g_bCanManagerThreadReadFlag = TRUE;
BOOL g_bBatThreadReadFlag = TRUE;




