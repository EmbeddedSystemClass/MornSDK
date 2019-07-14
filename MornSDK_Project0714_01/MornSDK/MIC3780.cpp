// MIC3780.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "General.h"
#include "MornSDK.h"
#include "MIC3780.h"


// CMIC3780
//YWJ----------------
CMIC3780::CMIC3780()
{
	m_hDev = 0;
}

CMIC3780::~CMIC3780()
{
	if (m_hDev != 0)
	{
		CloseIOCard();
	}
}

//��IO��
LRESULT CMIC3780::OpenIOCard(ULONG  lDevNum)
{
	LRESULT dwErrCde = DrvErrorCode;
	try
	{
		dwErrCde = DRV_DeviceOpen(lDevNum, &m_hDev);   
		if (dwErrCde != SUCCESS)
		{
			g_Log.WriteLog(LOG_ERR, "��MIC3780 IO��ʧ��");
			return dwErrCde;
		}
		g_Log.WriteLog(LOG_INF, "��MIC3780 IO���ɹ�");
		return SUCCESS;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, "��MIC3780 IO���Ĺ����г����쳣");
		return dwErrCde;
	}
}

//�ر�IO��
void CMIC3780::CloseIOCard()
{
	LRESULT dwErrCde = DrvErrorCode;
	try
	{
		DRV_DeviceClose((LONG far*)&m_hDev);
		g_Log.WriteLog(LOG_INF, "�ر�MIC3780 IO���ɹ�");
		m_hDev = 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, "�ر�MIC3780 IO���Ĺ����г����쳣");
	}
}

//��ͨ�������(chl��ͨ����ţ�wd�����ֵ��
BOOL CMIC3780::WriteWord(int chl, ushort wd)
{
	//LRESULT dwErrCde = DrvErrorCode;
	//static      PT_DioWritePortByte ptDioWritePortByte; // DioWritePortByte table
	try
	{
		//ͨ��
		m_ptDioWriteWord.port  = chl;
		//����
		m_ptDioWriteWord.mask  = 0xFF;
		//���ֵ
		m_ptDioWriteWord.state = wd;
		if((m_ErrCde = DRV_DioWritePortByte(m_hDev,
			(LPT_DioWritePortByte)&m_ptDioWriteWord)) != 0)
		{
			DRV_GetErrorMessage(m_ErrCde,(LPSTR)m_szErrMsg);
			//g_Log.WriteLog(LOG_ERR, "MIC3780 IO�������ֵʧ��");
			//MessageBox((LPCSTR)m_szErrMsg,"Driver Message",MB_OK);
			return FALSE;
		}
		else
		{
			//g_Log.WriteLog(LOG_ERR, "MIC3780 IO�������ֵ�ɹ�");
			return TRUE;
		}

		/*if((dwErrCde = DRV_DioWritePortWord(m_hDev, (LPT_DioWritePortWord)&m_ptDioWriteWord)) != 0)
		{
			g_Log.WriteLog(LOG_ERR, "MIC3780 IO�������ֵʧ��");
			return FALSE;
		} 
		else
		{
			g_Log.WriteLog(LOG_ERR, "MIC3780 IO�������ֵ�ɹ�");
			return TRUE;
		}*/
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, "MIC3780 IO�������ֵ�����г����쳣");
		return FALSE;
	}
}

USHORT CMIC3780::OnReadback()
{
	//static      PT_DioGetCurrentDOByte ptDioGetCurrentDOByte;
	LRESULT m_ErrCde;          //Return Eror Code
	char m_szErrMsg[80];       //Use for MessageBox function
	USHORT chValue = 0;
	m_ptDioGetCurrentDOByte.port  = 0;
	m_ptDioGetCurrentDOByte.value = (USHORT far *)&chValue;
	if((m_ErrCde = DRV_DioGetCurrentDOByte(m_hDev,
		(LPT_DioGetCurrentDOByte)&m_ptDioGetCurrentDOByte)) != 0)
	{
		DRV_GetErrorMessage(m_ErrCde,(LPSTR)m_szErrMsg);
		//AfxMessageBox((LPCSTR)m_szErrMsg,MB_OK);
	}
	CString szTemp;
	szTemp.Format( "%02X", chValue );
	//AfxMessageBox(szTemp, MB_OK);
	return chValue;
}

// CMIC3780 ��Ա����
