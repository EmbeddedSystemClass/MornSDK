// MIC3780.cpp : 实现文件
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

//打开IO卡
LRESULT CMIC3780::OpenIOCard(ULONG  lDevNum)
{
	LRESULT dwErrCde = DrvErrorCode;
	try
	{
		dwErrCde = DRV_DeviceOpen(lDevNum, &m_hDev);   
		if (dwErrCde != SUCCESS)
		{
			g_Log.WriteLog(LOG_ERR, "打开MIC3780 IO卡失败");
			return dwErrCde;
		}
		g_Log.WriteLog(LOG_INF, "打开MIC3780 IO卡成功");
		return SUCCESS;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, "打开MIC3780 IO卡的过程中出现异常");
		return dwErrCde;
	}
}

//关闭IO卡
void CMIC3780::CloseIOCard()
{
	LRESULT dwErrCde = DrvErrorCode;
	try
	{
		DRV_DeviceClose((LONG far*)&m_hDev);
		g_Log.WriteLog(LOG_INF, "关闭MIC3780 IO卡成功");
		m_hDev = 0;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, "关闭MIC3780 IO卡的过程中出现异常");
	}
}

//对通道输出字(chl：通道序号，wd：输出值）
BOOL CMIC3780::WriteWord(int chl, ushort wd)
{
	//LRESULT dwErrCde = DrvErrorCode;
	//static      PT_DioWritePortByte ptDioWritePortByte; // DioWritePortByte table
	try
	{
		//通道
		m_ptDioWriteWord.port  = chl;
		//掩码
		m_ptDioWriteWord.mask  = 0xFF;
		//输出值
		m_ptDioWriteWord.state = wd;
		if((m_ErrCde = DRV_DioWritePortByte(m_hDev,
			(LPT_DioWritePortByte)&m_ptDioWriteWord)) != 0)
		{
			DRV_GetErrorMessage(m_ErrCde,(LPSTR)m_szErrMsg);
			//g_Log.WriteLog(LOG_ERR, "MIC3780 IO卡输出数值失败");
			//MessageBox((LPCSTR)m_szErrMsg,"Driver Message",MB_OK);
			return FALSE;
		}
		else
		{
			//g_Log.WriteLog(LOG_ERR, "MIC3780 IO卡输出数值成功");
			return TRUE;
		}

		/*if((dwErrCde = DRV_DioWritePortWord(m_hDev, (LPT_DioWritePortWord)&m_ptDioWriteWord)) != 0)
		{
			g_Log.WriteLog(LOG_ERR, "MIC3780 IO卡输出数值失败");
			return FALSE;
		} 
		else
		{
			g_Log.WriteLog(LOG_ERR, "MIC3780 IO卡输出数值成功");
			return TRUE;
		}*/
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, "MIC3780 IO卡输出数值过程中出现异常");
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

// CMIC3780 成员函数
