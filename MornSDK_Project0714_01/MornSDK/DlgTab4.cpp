// DlgTab4.cpp : 实现文件
//

#include "stdafx.h"
#include "MornSDK.h"
#include "DlgTab4.h"
#include "afxdialogex.h"
#include "grid/GridCtrl.h"
#include "grid/GridCellCheck.h"
#include "General.h"
#include "MornSDKDlg.h"

HANDLE g_hTestDevice;
/*****************************************************************************
 DESCRIPTION: Send msg info 
*****************************************************************************/
int		g_SendFlag;
DWORD	g_dwCobId;
int		g_nLength;
BYTE	g_byData[8];

int count = 0;

/*****************************************************************************
 DESCRIPTION: Send and rceive status 
*****************************************************************************/
int     g_nSendNum4;
int     g_nSendTimeOut4;
int     g_nRcvNum4;
int     g_nRcvTimeOut4;
int     g_nBusOff;
int     g_nPassive;
int     g_nOverRun;
int     g_nBOverRun;

/*****************************************************************************
 DESCRIPTION: Parameter of data transfer 
*****************************************************************************/
int		g_nInterval;
int		g_nBaud;
int     g_nFilterMode;
DWORD	g_dwAcceptCode;
DWORD   g_dwAcceptMask;
HANDLE	g_hDeviceDirect;
HANDLE	g_hDeviceThread;

/*****************************************************************************
 DESCRIPTION: Error status 
*****************************************************************************/
#define STATUS_OK			        0
#define STATUS_BUS_ERROR            1
#define STATUS_BUS_OFF              2

/*****************************************************************************
 DESCRIPTION: Run flag of Thread 
*****************************************************************************/
BOOL	g_bExitReadThreadFlag;
BOOL	g_bExitWriteThreadFlag;

BOOL	g_bExitReadDirectFlag;

//UINT ThreadSendAndReceive(LPVOID param)
//{
//	//SetTimer(0, 2000, NULL);
//	
//	return 0;
//}


// CDlgTab4 对话框

IMPLEMENT_DYNAMIC(CDlgTab4, CDialogEx)

CDlgTab4::CDlgTab4(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTab4::IDD, pParent)
{

}

CDlgTab4::~CDlgTab4()
{
}

void CDlgTab4::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_GridControl(pDX, IDC_GirdSeedSource, m_pReceive_SeedSource);
	//DDX_GridControl(pDX, IDC_GirdLightA, m_pReceive_LightA);
	//DDX_GridControl(pDX, IDC_GirdLightB, m_pReceive_LightB);
	DDX_GridControl(pDX, IDC_GirdPowerB, m_pReceive_Power);
	DDX_GridControl(pDX, IDC_GirdStatus, m_pReceive_Status);

	DDX_Control(pDX, IDC_EDIT1, m_pEditCount);
}


BEGIN_MESSAGE_MAP(CDlgTab4, CDialogEx)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DISPYAY, display2)
END_MESSAGE_MAP()


// CDlgTab4 消息处理程序



BOOL CDlgTab4::InitPage()
{
	GridInit();

	//try
	//{
	//	int ret;
	//	//若已打开则先关闭
	//	if (g_hTestDevice != INVALID_HANDLE_VALUE && g_hTestDevice != NULL)
	//	{
	//		acCanClose(g_hTestDevice);
	//	}
	//	//打开
	//	g_hTestDevice = acCanOpen((LPTSTR)(LPCTSTR)"CAN6", TRUE);
	//	if (g_hTestDevice == INVALID_HANDLE_VALUE || g_hTestDevice == NULL)
	//	{
	//		acCanClose(g_hTestDevice);
	//		g_hTestDevice = INVALID_HANDLE_VALUE;
	//		//g_Log.WriteLog(LOG_ERR, _T("打开Can端口%s失败。"), (LPTSTR)(LPCTSTR)canPortName);
	//		return FALSE;
	//	}
	//	//保存端口名
	//	//m_szPortName = canPortName;

	//	ret = acEnterResetMode(g_hTestDevice);
	//	if (ret < 0)
	//	{
	//		acCanClose(g_hTestDevice);
	//		g_hTestDevice = INVALID_HANDLE_VALUE;
	//		//g_Log.WriteLog(LOG_ERR, _T("%s： acEnterResetMode调用失败。"), m_szPortName);
	//		return FALSE;
	//	}
	//	//set param
	//	ret = acSetBaud(g_hTestDevice, 250);
	//	if (ret < 0)
	//	{
	//		acCanClose(g_hTestDevice);
	//		g_hTestDevice = INVALID_HANDLE_VALUE;
	//		//g_Log.WriteLog(LOG_ERR, _T("%s： acSetBaud调用失败。"), m_szPortName);
	//		return FALSE;
	//	}
	//	ret = acSetAcceptanceFilter(g_hTestDevice, 0xffffffff, 0xffffffff);
	//	if (ret < 0)
	//	{
	//		acCanClose(g_hTestDevice);
	//		g_hTestDevice = INVALID_HANDLE_VALUE;
	//		//g_Log.WriteLog(LOG_ERR, _T("%s： acSetAcceptanceFilter调用失败。"), m_szPortName);
	//		return FALSE;
	//	}
	//	ret = acSetListenOnlyMode(g_hTestDevice, 0);
	//	if (ret < 0)
	//	{
	//		acCanClose(g_hTestDevice);
	//		g_hTestDevice = INVALID_HANDLE_VALUE;
	//		//g_Log.WriteLog(LOG_ERR, _T("%s： acSetListenOnlyMode调用失败。"), m_szPortName);
	//		return FALSE;
	//	}
	//	ret = acSetSelfReception(g_hTestDevice, FALSE);
	//	if (ret < 0)
	//	{
	//		acCanClose(g_hTestDevice);
	//		g_hTestDevice = INVALID_HANDLE_VALUE;
	//		//g_Log.WriteLog(LOG_ERR, _T("%s： acSetSelfReception调用失败。"), m_szPortName);
	//		return FALSE;
	//	}
	//	ret = acSetAcceptanceFilterMode(g_hTestDevice, PELICAN_DUAL_FILTER);
	//	if(ret < 0)
	//	{
	//		acCanClose(g_hTestDevice);
	//		g_hTestDevice = INVALID_HANDLE_VALUE;
	//		//g_Log.WriteLog(LOG_ERR, _T("%s： acSetAcceptanceFilterMode调用失败。"), m_szPortName);
	//		return FALSE;
	//	}
	//	ret = acEnterWorkMode(g_hTestDevice);
	//	if (ret < 0)
	//	{
	//		acCanClose(g_hTestDevice);
	//		g_hTestDevice = INVALID_HANDLE_VALUE;
	//		//g_Log.WriteLog(LOG_ERR, _T("%s： acEnterWorkMode调用失败。"), m_szPortName);
	//		return FALSE;
	//	}
	//	//g_Log.WriteLog(LOG_INF, _T("打开Can端口%s成功。"), m_szPortName);
	//	//return TRUE;
	//}
	//catch (CException* e)
	//{
	//	//g_Log.WriteLog(LOG_ERR, _T("打开Can端口%s时出现异常。"), m_szPortName);
	//	return FALSE;
	//}




	//DWORD ThreadIDSendAndReceive;
	//hThreadSendAndReceive = AfxBeginThread(ThreadSendAndReceive,&ThreadIDSendAndReceive);
	//m_pEditCount.SetWindowText("aa");


	//SetTimer(0, 1000, NULL);
	return TRUE;
}

//
//void CDlgTab4::OnTimer(UINT nIDEvent)
//{
//	int		count = 0;
//	OVERLAPPED ov;
//	DWORD dwWritten;
//	char DataBuf[10];
//	DWORD	ReceiveFlag = 0;
//	DWORD   CobId = 0;
//	DWORD	Length = 0;
//	BYTE	Data[8];
//	DWORD	dwRead=0;
//	CString strTemp, str;
//	int     ret;
//	BYTE	byBuf[100];
////	g_bExitReadDirectFlag = TRUE;
//	canmsg_t tx[1];
//	{
//		ZeroMemory(tx[0].data, 8);
//		tx[0].flags	    = (BYTE)0;
//		tx[0].id	    = (BYTE)33;
//		tx[0].length    = (BYTE)8;
//		tx[0].data[0]	= (BYTE)254;
//		tx[0].data[1]	= (BYTE)7;
//		tx[0].data[2]	= (BYTE)0;
//		tx[0].data[3]	= (BYTE)0;
//		tx[0].data[4]	= (BYTE)0;
//		tx[0].data[5]	= (BYTE)0;
//		tx[0].data[6]	= (BYTE)0;
//		tx[0].data[7]	= (BYTE)0;
//	}
//	/*if (((CCanTestDlg *)(theApp.m_pMainWnd))->m_bRtr)
//	{
//	ZeroMemory(tx[0].data,  8);
//	tx[0].length	= 0;	
//	}	*/
//	memset(&ov, 0, sizeof(OVERLAPPED));
//	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//
//	ret = acCanWrite(g_hTestDevice, &tx[0], 1, &dwWritten, &ov);
//	if(ret == SUCCESS)
//	{
//		g_nSendNum4 += dwWritten;
//	}
//	else if(ret == TIME_OUT)
//	{
//		g_nSendTimeOut4 ++;
//	}
//
//	CloseHandle(ov.hEvent);
//
//	ZeroMemory(byBuf, 100);
//	canmsg_t *pBuf = (canmsg_t *)byBuf;
//	memset(&ov, 0, sizeof(OVERLAPPED));
//	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//	ret = acCanRead(g_hTestDevice, pBuf, 1, &dwRead, &ov);
//	CloseHandle(ov.hEvent);
//	if(ret == SUCCESS || ret==TIME_OUT)
//	{
//		if(ret == TIME_OUT)
//		{
//			//if(g_bExitReadDirectFlag)
//			if(TRUE)
//				g_nRcvTimeOut4 ++;
//		}
//
//		if(pBuf->id != ERRORID)
//			g_nRcvNum4 += dwRead;
//	}
//	else{
//		//continue;
//	}
//	if(dwRead > 0 )
//	{
//		ReceiveFlag	= pBuf->flags;
//		CobId		= pBuf->id;
//		Length		= pBuf->length;
//		memcpy(Data, pBuf->data, 8);
//
//		strTemp.Format("No.%-5d ID:%-3x Len%d: %02X %02X %02X %02X %02X %02X %02X %02X",g_nRcvNum4, CobId, Length, Data[0], Data[1]\
//			,Data[2], Data[3],Data[4], Data[5],Data[6], Data[7] );
//
//		GetDlgItem(IDC_EDIT1)->SetWindowText(strTemp);
//
//		//if (((CCanTestDlg *)(theApp.m_pMainWnd))->m_listView.GetCount() > 10000)
//			//((CCanTestDlg *)(theApp.m_pMainWnd))->m_listView.DeleteString(((CCanTestDlg *)(theApp.m_pMainWnd))->m_listView.GetCount()-1);
//		//(CEdit*)GetDlgItem(IDC_EDIT_ThreadReceive)->SetWindowText(strTemp);
//		//m_pEditThreadReceive.SetWindowText(strTemp);
//		//((CTestSendAndReceiveDlg*)(theApp.m_pMainWnd))->GetDlgItem(IDC_EDIT_DirectReceive)->SetWindowText(strTemp);
//		//((CButtonST*)(g_pMainDlg->m_Page4.GetDlgItem(IDC_LED_resetCURRENT)))->SetIcon(IDI_ICON_GREEN);
//		//No.1     ID:21  Len8: 21 08 00 00 00 00 00 2B SFF
//		//((CGridCtrl*)(g_pMainDlg->m_Page4.m_pReceive_Power).SetItemText(0, 0, 1);
//		if(Data[0] == (BYTE)21 && Data[1] == (BYTE)8)
//		{
//			//(CGridCtrl*)(g_pMainDlg->m_Page4).m_pReceive_StatusSetItemText(1, 1, Data[3]+Data[4]);
//			//(CGridCtrl)(g_pMainDlg->m_Page4).m_pReceive_Status.SetItemText(1,1,Data[3]);
//			CString str;
//			str.Format("%d", Data[3]);
//			m_pReceive_Status.SetItemText(1,1,str);
//		}
//
//		if(ReceiveFlag & MSG_EXT)
//			strTemp += " EFF";
//		else
//			strTemp += " SFF";
//		if(ReceiveFlag & MSG_RTR)
//			strTemp += "|RTR";	
//		if(ReceiveFlag & MSG_SELF)
//			strTemp += "|SELF";
//
//		if(ReceiveFlag & MSG_BOVR)
//		{
//			g_nBOverRun ++;
//			strTemp  += "Buffer overrun error";
//		}
//		if(ReceiveFlag & MSG_OVR)
//		{
//			g_nOverRun ++;
//			strTemp  += "Fifo overrun error";
//		}
//		if(ReceiveFlag & MSG_BUSOFF)
//		{
//			g_nBusOff ++;
//			strTemp  += "Bus off error";
//		}
//		if(ReceiveFlag & MSG_PASSIVE)
//		{
//			strTemp  += "Passive error";
//			g_nPassive ++;
//		}
//
//		/*if(((CCanTestDlg *)(theApp.m_pMainWnd))->m_cbAssic.GetCheck())
//		{	
//		strTemp += " ";
//		for(int m=0; m<8; m++)
//		{
//		str.Format(" %-c", Data[m]);
//		strTemp += str;
//		}
//		}*/
//		/*if(CobId != ERRORID)
//		((CCanTestDlg *)(theApp.m_pMainWnd))->m_listView.InsertString(0, strTemp);*/
//		if(CobId != ERRORID)
//			//((CTestSendAndReceiveDlg*)(theApp.m_pMainWnd))->GetDlgItem(IDC_EDIT_DirectReceive)->SetWindowText(strTemp);
//			;
//	}
//	CString a;
//	a.Format("%d", count);
//	m_pEditCount.SetWindowTextA(a);
//	count++;
//	CDialog::OnTimer(nIDEvent);
//}

void CDlgTab4::textReset(BYTE id)
{
	int		count = 0;
	OVERLAPPED ov;
	DWORD dwWritten;
//	char DataBuf[10];
	DWORD	ReceiveFlag = 0;
	DWORD   CobId = 0;
	DWORD	Length = 0;
	BYTE	Data[8];
	DWORD	dwRead=0;
	CString strTemp, str;
	int     ret;
	BYTE	byBuf[100];
//	g_bExitReadDirectFlag = TRUE;
	canmsg_t tx[1];
	{
		ZeroMemory(tx[0].data, 8);
		tx[0].flags	    = (BYTE)0;
		tx[0].id	    = (BYTE)33;
		tx[0].length    = (BYTE)8;
		tx[0].data[0]	= (BYTE)254;
		tx[0].data[1]	= (BYTE)7;
		tx[0].data[2]	= (BYTE)0;
		tx[0].data[3]	= (BYTE)0;
		tx[0].data[4]	= (BYTE)0;
		tx[0].data[5]	= (BYTE)0;
		tx[0].data[6]	= (BYTE)0;
		tx[0].data[7]	= (BYTE)0;
	}
	/*if (((CCanTestDlg *)(theApp.m_pMainWnd))->m_bRtr)
	{
	ZeroMemory(tx[0].data,  8);
	tx[0].length	= 0;	
	}	*/
	memset(&ov, 0, sizeof(OVERLAPPED));
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	ret = acCanWrite(g_hTestDevice, &tx[0], 1, &dwWritten, &ov);
	if(ret == SUCCESS)
	{
		g_nSendNum4 += dwWritten;
	}
	else if(ret == TIME_OUT)
	{
		g_nSendTimeOut4 ++;
	}

	CloseHandle(ov.hEvent);

	ZeroMemory(byBuf, 100);
	canmsg_t *pBuf = (canmsg_t *)byBuf;
	memset(&ov, 0, sizeof(OVERLAPPED));
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ret = acCanRead(g_hTestDevice, pBuf, 1, &dwRead, &ov);
	CloseHandle(ov.hEvent);
	if(ret == SUCCESS || ret==TIME_OUT)
	{
		if(ret == TIME_OUT)
		{
			//if(g_bExitReadDirectFlag)
			if(TRUE)
				g_nRcvTimeOut4 ++;
		}

		if(pBuf->id != ERRORID)
			g_nRcvNum4 += dwRead;
	}
	else{
		//continue;
	}
	if(dwRead > 0 )
	{
		ReceiveFlag	= pBuf->flags;
		CobId		= pBuf->id;
		Length		= pBuf->length;
		memcpy(Data, pBuf->data, 8);

		strTemp.Format("No.%-5d ID:%-3x Len%d: %02X %02X %02X %02X %02X %02X %02X %02X",g_nRcvNum4, CobId, Length, Data[0], Data[1]\
			,Data[2], Data[3],Data[4], Data[5],Data[6], Data[7] );

		::PostMessageA(g_pMainDlg->m_Page4, WM_DISPYAY, CobId, (LPARAM)Data);

		//if (((CCanTestDlg *)(theApp.m_pMainWnd))->m_listView.GetCount() > 10000)
			//((CCanTestDlg *)(theApp.m_pMainWnd))->m_listView.DeleteString(((CCanTestDlg *)(theApp.m_pMainWnd))->m_listView.GetCount()-1);
		//(CEdit*)GetDlgItem(IDC_EDIT_ThreadReceive)->SetWindowText(strTemp);
		//m_pEditThreadReceive.SetWindowText(strTemp);
		//((CTestSendAndReceiveDlg*)(theApp.m_pMainWnd))->GetDlgItem(IDC_EDIT_DirectReceive)->SetWindowText(strTemp);
		//((CButtonST*)(g_pMainDlg->m_Page4.GetDlgItem(IDC_LED_resetCURRENT)))->SetIcon(IDI_ICON_GREEN);
		//No.1     ID:21  Len8: 21 08 00 00 00 00 00 2B SFF
		//((CGridCtrl*)(g_pMainDlg->m_Page4.m_pReceive_Power).SetItemText(0, 0, 1);
		if(Data[0] == (BYTE)33 && Data[1] == (BYTE)8)
		{
			//(CGridCtrl*)(g_pMainDlg->m_Page4).m_pReceive_StatusSetItemText(1, 1, Data[3]+Data[4]);
			//(CGridCtrl)(g_pMainDlg->m_Page4).m_pReceive_Status.SetItemText(1,1,Data[3]);
			CString str;
			str.Format("%d", Data[3]);

			m_pReceive_Status.SetItemText(1,1,str);
			m_pReceive_Status.SetItemText(3,3,"1");
			m_pReceive_Status.SetItemText(4,4,"2");
			m_pReceive_Status.SetItemText(5,5,"3");
		}

		if(ReceiveFlag & MSG_EXT)
			strTemp += " EFF";
		else
			strTemp += " SFF";
		if(ReceiveFlag & MSG_RTR)
			strTemp += "|RTR";	
		if(ReceiveFlag & MSG_SELF)
			strTemp += "|SELF";

		if(ReceiveFlag & MSG_BOVR)
		{
			g_nBOverRun ++;
			strTemp  += "Buffer overrun error";
		}
		if(ReceiveFlag & MSG_OVR)
		{
			g_nOverRun ++;
			strTemp  += "Fifo overrun error";
		}
		if(ReceiveFlag & MSG_BUSOFF)
		{
			g_nBusOff ++;
			strTemp  += "Bus off error";
		}
		if(ReceiveFlag & MSG_PASSIVE)
		{
			strTemp  += "Passive error";
			g_nPassive ++;
		}

		/*if(((CCanTestDlg *)(theApp.m_pMainWnd))->m_cbAssic.GetCheck())
		{	
		strTemp += " ";
		for(int m=0; m<8; m++)
		{
		str.Format(" %-c", Data[m]);
		strTemp += str;
		}
		}*/
		/*if(CobId != ERRORID)
		((CCanTestDlg *)(theApp.m_pMainWnd))->m_listView.InsertString(0, strTemp);*/
		//if(CobId != ERRORID)
			//((CTestSendAndReceiveDlg*)(theApp.m_pMainWnd))->GetDlgItem(IDC_EDIT_DirectReceive)->SetWindowText(strTemp);
			//;
	}
}

void CDlgTab4::GridInit()
{
	////A激光状态
	//m_pReceive_LightA.SetColumnCount(8);    // 设定总共的列数
	//m_pReceive_LightA.SetColumnWidth(0, 40);
	//m_pReceive_LightA.SetColumnWidth(1, 40);
	//m_pReceive_LightA.SetColumnWidth(2, 40);
	//m_pReceive_LightA.SetColumnWidth(3, 60);
	//m_pReceive_LightA.SetColumnWidth(4, 40);
	//m_pReceive_LightA.SetColumnWidth(5, 40);
	//m_pReceive_LightA.SetColumnWidth(6, 40);
	//m_pReceive_LightA.SetColumnWidth(7, 40);
	//m_pReceive_LightA.SetRowCount(2);       // 设定总共的行数S
	//m_pReceive_LightA.SetRowHeight(0,30);
	//m_pReceive_LightA.SetFixedRowCount(1);  // 设定固定行数
	//m_pReceive_LightA.SetItemText(0,0,"Pd1"); // SetItemText 用来设定某行某列的具体文本数值
	//m_pReceive_LightA.SetItemText(0,1,"Pd2");
	//m_pReceive_LightA.SetItemText(0,2,"Pd3");
	//m_pReceive_LightA.SetItemText(0,3,"光源状态");
	//m_pReceive_LightA.SetItemText(0,4,"T1");
	//m_pReceive_LightA.SetItemText(0,5,"T2");
	//m_pReceive_LightA.SetItemText(0,6,"T3");
	//m_pReceive_LightA.SetItemText(0,7,"温度");
	////m_pLight1.AutoSize();
	//m_pReceive_LightA.ShowScrollBar(SB_BOTH, FALSE);

	////B激光状态
	//m_pReceive_LightB.SetColumnCount(8);    // 设定总共的列数
	//m_pReceive_LightB.SetColumnWidth(0, 40);
	//m_pReceive_LightB.SetColumnWidth(1, 40);
	//m_pReceive_LightB.SetColumnWidth(2, 40);
	//m_pReceive_LightB.SetColumnWidth(3, 60);
	//m_pReceive_LightB.SetColumnWidth(4, 40);
	//m_pReceive_LightB.SetColumnWidth(5, 40);
	//m_pReceive_LightB.SetColumnWidth(6, 40);
	//m_pReceive_LightB.SetColumnWidth(7, 40);
	//m_pReceive_LightB.SetRowCount(2);       // 设定总共的行数S
	//m_pReceive_LightB.SetRowHeight(0,30);
	//m_pReceive_LightB.SetFixedRowCount(1);  // 设定固定行数
	//m_pReceive_LightB.SetItemText(0,0,"Pd1"); // SetItemText 用来设定某行某列的具体文本数值
	//m_pReceive_LightB.SetItemText(0,1,"Pd2");
	//m_pReceive_LightB.SetItemText(0,2,"Pd3");
	//m_pReceive_LightB.SetItemText(0,3,"光源状态");
	//m_pReceive_LightB.SetItemText(0,4,"T1");
	//m_pReceive_LightB.SetItemText(0,5,"T2");
	//m_pReceive_LightB.SetItemText(0,6,"T3");
	//m_pReceive_LightB.SetItemText(0,7,"温度");
	//m_pReceive_LightB.ShowScrollBar(SB_BOTH, FALSE);


	////电源状态
	//m_pReceive_SeedSource.SetColumnCount(8);    // 设定总共的列数
	//for (int i = 0; i < 8; i++)
	//{
	//	m_pReceive_SeedSource.SetColumnWidth(i, 80);
	//}
	//m_pReceive_SeedSource.SetRowCount(3);       // 设定总共的行数S
	//m_pReceive_SeedSource.SetRowHeight(0,30);
	//m_pReceive_SeedSource.SetFixedRowCount(1);  // 设定固定行数
	//
	//m_pReceive_SeedSource.SetItemText(0,0,"震荡级电流"); // SetItemText 用来设定某行某列的具体文本数值
	//m_pReceive_SeedSource.SetItemText(0,1,"震荡级状态");
	//m_pReceive_SeedSource.SetItemText(0,2,"放大级1电流");
	//m_pReceive_SeedSource.SetItemText(0,3,"放大级1状态");
	//m_pReceive_SeedSource.SetItemText(0,4,"放大级2电流"); 
	//m_pReceive_SeedSource.SetItemText(0,5,"放大级2状态");
	//m_pReceive_SeedSource.SetItemText(0,6,"放大级3电流");
	//m_pReceive_SeedSource.SetItemText(0,7,"放大级3状态");

	////m_pBat1.SetItemText(0,8,"B震荡电流"); // SetItemText 用来设定某行某列的具体文本数值
	////m_pBat1.SetItemText(0,9,"B震荡状态");
	////m_pBat1.SetItemText(0,10,"B放1电流");
	////m_pBat1.SetItemText(0,11,"B放1状态");
	////m_pBat1.SetItemText(0,12,"B放2电流"); 
	////m_pBat1.SetItemText(0,13,"B放2状态");
	////m_pBat1.SetItemText(0,14,"B放3电流");
	////m_pBat1.SetItemText(0,15,"B放3状态");
	//m_pReceive_SeedSource.ShowScrollBar(SB_BOTH, FALSE);

	////驱动电源设置
	m_pReceive_Power.SetColumnCount(21);   // 设定总共的列数
	m_pReceive_Power.SetRowCount(6);       //设定总共的行数S
	for (int i = 0; i < 21; i++)
	{
		m_pReceive_Power.SetColumnWidth(i, 34);
	}
	m_pReceive_Power.SetColumnWidth(12, 35);
	m_pReceive_Power.SetColumnWidth(14, 35);
	m_pReceive_Power.SetRowHeight(0,30);
	/*for (int i = 0; i < 21; i++)
	{
		m_pReceive_Power.SetCellType(0, i, RUNTIME_CLASS(CGridCellCheck));
	}*/
	m_pReceive_Power.SetItemText(0,0,"id"); // SetItemText 用来设定某行某列的具体文本数值
	m_pReceive_Power.SetItemText(1,0,"1");
	m_pReceive_Power.SetItemText(2,0,"2");
	m_pReceive_Power.SetItemText(3,0,"3");
	m_pReceive_Power.SetItemText(4,0,"4");
	m_pReceive_Power.SetItemText(5,0,"delay");
	for(int i = 1; i < 11 ; i++)
	{
		CString str ;
		str.Format("%d", i - 1);
		m_pReceive_Power.SetItemText(0,i,"0x2" + str);
	}
	m_pReceive_Power.SetItemText(0,11,"0x2a");
	m_pReceive_Power.SetItemText(0,12,"0x2b");
	m_pReceive_Power.SetItemText(0,13,"0x2c");
	m_pReceive_Power.SetItemText(0,14,"0x2d");
	m_pReceive_Power.SetItemText(0,15,"0x2e");
	m_pReceive_Power.SetItemText(0,16,"0x2f");
	m_pReceive_Power.SetItemText(0,17,"0x30");
	m_pReceive_Power.SetItemText(0,18,"0x31");
	m_pReceive_Power.SetItemText(0,19,"0x32");
	m_pReceive_Power.SetItemText(0,20,"0x33");
	/*for (int i = 1; i < 21; i++)
	{
		m_pReceive_Power.SetItemState(0,i,GVIS_READONLY);/// 来设置单元格是否只读
	}
	for (int i = 0; i < 6; i++)
	{
		m_pReceive_Power.SetItemState(i,0,GVIS_READONLY);/// 来设置单元格是否只读
	}*/
	
	m_pReceive_Power.SetFixedRowCount(1);		//设定固定行数
	m_pReceive_Power.SetFixedColumnCount(1);    //设定固定行数
	m_pReceive_Power.ShowScrollBar(SB_BOTH, FALSE);



	//驱动电源状态查询
	m_pReceive_Status.SetColumnCount(21);    // 设定总共的列数
	m_pReceive_Status.SetRowCount(15);       // 设定总共的行数S
	m_pReceive_Status.SetColumnWidth(0, 80);
	for (int i = 1; i < 21; i++)
	{
		m_pReceive_Status.SetColumnWidth(i, 37);
	}
	for (int i = 0; i < 15; i++)
	{
		m_pReceive_Status.SetRowHeight(i, 20);
	}
	m_pReceive_Status.SetItemText(0,0,"id"); // SetItemText 用来设定某行某列的具体文本数值
	m_pReceive_Status.SetItemText(1,0,"输出电流");
	m_pReceive_Status.SetItemText(2,0,"状态字1Bit7");
	m_pReceive_Status.SetItemText(3,0,"状态字1Bit6");
	m_pReceive_Status.SetItemText(4,0,"状态字1Bit5");
	m_pReceive_Status.SetItemText(5,0,"状态字1Bit4");
	m_pReceive_Status.SetItemText(6,0,"状态字1Bit3");
	m_pReceive_Status.SetItemText(7,0,"状态字1Bit2");
	m_pReceive_Status.SetItemText(8,0,"状态字1Bit1");
	m_pReceive_Status.SetItemText(9,0,"状态字1Bit0");
	m_pReceive_Status.SetItemText(10,0,"状态字2Bit7");
	m_pReceive_Status.SetItemText(11,0,"状态字2Bit6");
	m_pReceive_Status.SetItemText(12,0,"状态字2Bit5");
	m_pReceive_Status.SetItemText(13,0,"状态字2Bit4");
	m_pReceive_Status.SetItemText(14,0,"状态字2Bit3");
	m_pReceive_Status.SetItemText(15,0,"状态字2Bit2");
	m_pReceive_Status.SetItemText(16,0,"状态字2Bit1");

	for(int i = 1; i < 11 ; i++)
	{
		CString str ;
		str.Format("%d", i - 1);
		m_pReceive_Status.SetItemText(0,i,"0x2" + str);
	}
	m_pReceive_Status.SetItemText(0,11,"0x2a");
	m_pReceive_Status.SetItemText(0,12,"0x2b");
	m_pReceive_Status.SetItemText(0,13,"0x2c");
	m_pReceive_Status.SetItemText(0,14,"0x2d");
	m_pReceive_Status.SetItemText(0,15,"0x2e");
	m_pReceive_Status.SetItemText(0,16,"0x2f");
	m_pReceive_Status.SetItemText(0,17,"0x30");
	m_pReceive_Status.SetItemText(0,18,"0x31");
	m_pReceive_Status.SetItemText(0,19,"0x32");
	m_pReceive_Status.SetItemText(0,20,"0x33");

	m_pReceive_Status.SetFixedRowCount(1);  // 设定固定行数
	m_pReceive_Status.SetFixedColumnCount(1);  // 设定固定行数
	m_pReceive_Status.ShowScrollBar(SB_BOTH, FALSE);

	//m_pReceive_Status.SetItemText(1,1,"√");
	//m_pReceive_Status.SetItemText(1,2,"×");
}

LRESULT CDlgTab4::display2(WPARAM wPARAM, LPARAM lPARAM)
{
	BYTE *data = (BYTE*)lPARAM;
	m_pReceive_Status.SetItemText(2,2,"aaa");
	m_pReceive_Status.SetItemText(2,3,"aaa");
	m_pReceive_Status.SetItemText(2,4,"aaa");
	m_pReceive_Status.SetItemText(2,5,"aaa");
	m_pReceive_Status.SetItemText(2,6,"aaa");

	return 0;
}
