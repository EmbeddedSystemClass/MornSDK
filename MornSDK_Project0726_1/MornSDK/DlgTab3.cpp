// DlgTab3.cpp : 实现文件
//

#include "stdafx.h"
#include "MornSDK.h"
#include "DlgTab3.h"
#include "afxdialogex.h"
#include "MornSDKDlg.h"
#include <iomanip>
#include <Windows.h>
#include <math.h>


// CDlgTab3 对话框
CString OphirLMStates[10] = {"ok", "overrange", "wayOverrange", "missing", 
	"energyReset", "energyWaiting", "energySumming", "energyTimeout", "energyPeakOver", "energyOver"};
int hOphirLMCount = 0;	//功率计1S计数
void DataReadyCallback(long device,long channel)
{
	std::vector<double> values;
	std::vector<double> timestamps;
	std::vector<OphirLMMeasurement::Status> statuses;
	g_pMainDlg->m_Page3.OphirLM.GetData(device, channel, values, timestamps, statuses);
	hOphirLMCount++;
	if(hOphirLMCount == 20)	//频率50hz，20次为1s
	{
		for (size_t i = 0; i < values.size(); ++i)
		{
			CString s;
			CString value;
			if(values[i] >= pow((double)(10), -6))								//uW
				value.Format("%.1f%s", values[i] * pow((double)(10), 6), "uW"); 
			else if(values[i] >= pow((double)(10), -9) && values[i] < pow((double)(10), -6))
				value.Format("%.1f%s", values[i] * pow((double)(10), 9), "nW"); //nW
			else
				value.Format("%.1f%s", values[i] * pow((double)(10), 12), "pW"); //nW
			//s.Format("%d %.1f %.10f %d", i, timestamps[i], values[i], statuses[i]);
			s.Format("%d %s %s", i, value, OphirLMStates[i]);
			((CEdit*)(g_pMainDlg->m_Page3.GetDlgItem(IDC_EDIT3)))->SetWindowText(s);
		}
		hOphirLMCount = 0;	//清楚计数，重新开始计数
	}
}

IMPLEMENT_DYNAMIC(CDlgTab3, CDialogEx)

CDlgTab3::CDlgTab3(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTab3::IDD, pParent)
{
	hOphirLMDevice = 0, hOphirLMchannel = 0;	//初始化功率计句柄和通道
}

CDlgTab3::~CDlgTab3()
{
	aSocket.Close();
}

void CDlgTab3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

//初始化页面函数
void CDlgTab3::InitPage()
{
	InitialOphirLM();	//初始化功率计
	IntialCSocket();	//初始化tcp
	SetTimer(0, 1000, NULL);
}

BEGIN_MESSAGE_MAP(CDlgTab3, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_COMSEND, &CDlgTab3::OnBnClickedBtnComsend)
	ON_BN_CLICKED(IDC_BTN_UDPSEND, &CDlgTab3::OnBnClickedBtnUdpsend)
	ON_BN_CLICKED(IDC_BUTTON_SETA, &CDlgTab3::OnBnClickedButtonSetA)
	ON_BN_CLICKED(IDC_BUTTON_SETB, &CDlgTab3::OnBnClickedButtonSetB)
	ON_BN_CLICKED(IDC_BUTTON_RESETA, &CDlgTab3::OnBnClickedButtonResetA)
	ON_BN_CLICKED(IDC_BUTTON_RESETB, &CDlgTab3::OnBnClickedButtonResetB)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUTA, &CDlgTab3::OnBnClickedButtonOutputA)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUTB, &CDlgTab3::OnBnClickedButtonOutputB)
	ON_BN_CLICKED(IDC_BUTTON_STOPA, &CDlgTab3::OnBnClickedButtonStopA)
	ON_BN_CLICKED(IDC_BUTTON_STOPB, &CDlgTab3::OnBnClickedButtonStopB)
	ON_BN_CLICKED(IDC_BUTTON_CXA, &CDlgTab3::OnBnClickedButtonCxA)
	ON_BN_CLICKED(IDC_BUTTON_CXB, &CDlgTab3::OnBnClickedButtonCxB)
	ON_BN_CLICKED(IDC_BUTTON_CXZTA, &CDlgTab3::OnBnClickedButtonCxzA)
	ON_BN_CLICKED(IDC_BUTTON_CXZTB, &CDlgTab3::OnBnClickedButtonCxztB)
	ON_BN_CLICKED(IDC_BTN_SETVALUE, &CDlgTab3::OnBnClickedBtnSetvalue)
	ON_BN_CLICKED(IDC_BTN_FEEDBACK, &CDlgTab3::OnBnClickedBtnFeedback)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgTab3::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgTab3 消息处理程序

//发送数据
void CDlgTab3::OnBnClickedBtnComsend()
{
	/*if (g_pCnCommToDown->IsOpen())
	{
	CString strSend;
	GetDlgItem(IDC_EDIT_COMSEND)->GetWindowText(strSend);
	int length = strSend.GetLength();
	char* sendBuff = strSend.GetBuffer(length);
	g_pCnCommToDown->Write(sendBuff);
	}
	else
	{
	MessageBox(_T("串口无法发送数据。"),_T("提示"));
	}*/
	//if (g_pCnCommToUp->IsOpen())
	//{
	//	CString strSend;
	//	GetDlgItem(IDC_EDIT_COMSEND)->GetWindowText(strSend);
	//	int length = strSend.GetLength();
	//	char* sendBuff = strSend.GetBuffer(length);
	//	g_pCnCommToUp->Write(sendBuff);
	//}
	//else
	//{
	//	MessageBox(_T("串口无法发送数据。"),_T("提示"));
	//}
	if (g_pSP->isOpened())
	{
		CString strSend;
		GetDlgItem(IDC_EDIT_COMSEND)->GetWindowText(strSend);
		int length = strSend.GetLength();
		char* sendBuff = strSend.GetBuffer(length);
		g_pSP->writeData(sendBuff,length);
	}
	else
	{
		MessageBox(_T("串口未打开，无法发送数据。"),_T("提示"));
	}
}


void CDlgTab3::OnBnClickedBtnUdpsend()
{
	CString strSend;
	GetDlgItem(IDC_EDIT_UDPSEND)->GetWindowText(strSend);
	g_pMainDlg->udpHelper.SendString(strSend);
	//g_pMainDlg->udpHelper.SendTo((LPCTSTR)strSend, strSend.GetLength(), 5550, "127.0.0.1");
}

//时序控制器A 参数设置
void CDlgTab3::OnBnClickedButtonSetA()
{
	CString str;
	BYTE xx[6] = {0xBB, 0x11, 0x00, 0x00, 0x11, 0xEE};
	((CEdit*)GetDlgItem(IDC_EDIT_YFA))->GetWindowText(str); 
	xx[2] = atoi(str);
	((CEdit*)GetDlgItem(IDC_EDIT_CGA))->GetWindowText(str);
	xx[3] = atoi(str);
	xx[4]=0x11^xx[2]^xx[3];
	g_pMainDlg->udpAsxHelper.Send(xx, 6);
}

//时序控制器B 参数设置
void CDlgTab3::OnBnClickedButtonSetB()
{
	CString str;
	BYTE xx[6] = {0xBB, 0x11, 0x00, 0x00, 0x11, 0xEE}; //时序控制器 参数设置帧
	((CEdit*)GetDlgItem(IDC_EDIT_YFB))->GetWindowText(str); 
	xx[2] = atoi(str);
	((CEdit*)GetDlgItem(IDC_EDIT_CGB))->GetWindowText(str);
	xx[3] = atoi(str);
	xx[4]=0x11^xx[2]^xx[3];
	g_pMainDlg->udpBsxHelper.Send(xx, 6);
}

//时序控制器A 复位
void CDlgTab3::OnBnClickedButtonResetA()
{
	BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE};
	g_pMainDlg->udpAsxHelper.Send(xx, 6);
}

//时序控制器B 复位
void CDlgTab3::OnBnClickedButtonResetB()
{
	BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //故障复位帧
	g_pMainDlg->udpBsxHelper.Send(xx, 6);
}

//时序控制器A 输出
void CDlgTab3::OnBnClickedButtonOutputA()
{
	BYTE xx[6] = {0xBB, 0x13, 0x00, 0x00, 0x13, 0xEE};
	g_pMainDlg->udpAsxHelper.Send(xx, 6);
}

//时序控制器B 输出
void CDlgTab3::OnBnClickedButtonOutputB()
{
	BYTE xx[6] = {0xBB, 0x13, 0x00, 0x00, 0x13, 0xEE};
	g_pMainDlg->udpBsxHelper.Send(xx, 6);
}

//时序控制器A 停止输出
void CDlgTab3::OnBnClickedButtonStopA()
{
	BYTE xx[6] = {0xBB, 0x14, 0x00, 0x00, 0x14, 0xEE};
	g_pMainDlg->udpAsxHelper.Send(xx, 6);
}

//时序控制器B 停止输出
void CDlgTab3::OnBnClickedButtonStopB()
{
	BYTE xx[6] = {0xBB, 0x14, 0x00, 0x00, 0x14, 0xEE};
	g_pMainDlg->udpBsxHelper.Send(xx, 6);
}

//时序控制器A 查询参数帧
void CDlgTab3::OnBnClickedButtonCxA()
{
	BYTE xx[6] = {0xBB, 0x21, 0x00, 0x00, 0x21, 0xEE};
	g_pMainDlg->udpAsxHelper.Send(xx, 6);
}

//时序控制器B 查询参数帧
void CDlgTab3::OnBnClickedButtonCxB()
{
	BYTE xx[6] = {0xBB, 0x21, 0x00, 0x00, 0x21, 0xEE};
	g_pMainDlg->udpBsxHelper.Send(xx, 6);
}

//时序控制器A 查询状态
void CDlgTab3::OnBnClickedButtonCxzA()
{
	BYTE xx[6] = {0xBB, 0x22, 0x00, 0x00, 0x22, 0xEE};
	g_pMainDlg->udpAsxHelper.Send(xx, 6);
}

//时序控制器B 查询状态
void CDlgTab3::OnBnClickedButtonCxztB()
{
	BYTE xx[6] = {0xBB, 0x22, 0x00, 0x00, 0x22, 0xEE};
	g_pMainDlg->udpBsxHelper.Send(xx, 6);
}

//IO控制卡 设置Byte
void CDlgTab3::OnBnClickedBtnSetvalue()
{
	static      PT_DioWritePortByte ptDioWritePortByte; // DioWritePortByte table

	CString valueStr;
	((CEdit*)GetDlgItem(IDC_EDIT_SETVALUE))->GetWindowText(valueStr);
	USHORT chValue = strtol(valueStr, NULL, 16);

	//g_pIoCard->WriteWord(0, chValue);

	ptDioWritePortByte.port  = 0;
	ptDioWritePortByte.mask  = 0xFF;
	ptDioWritePortByte.state = chValue;

	if((g_pIoCard->m_ErrCde = DRV_DioWritePortByte(g_pIoCard->m_hDev,
		(LPT_DioWritePortByte)&ptDioWritePortByte)) != 0)
	{
		DRV_GetErrorMessage(g_pIoCard->m_ErrCde,(LPSTR)g_pIoCard->m_szErrMsg);
		//MessageBox((LPCSTR)g_pIoCard->m_szErrMsg,"Driver Message",MB_OK);
	}
	else
	{
		//MessageBox("set success", "setValue");
	}
}

//IO控制卡 读取状态
void CDlgTab3::OnBnClickedBtnFeedback()
{
	USHORT chValue = 0;
	chValue = g_pIoCard->OnReadback();
	static      PT_DioGetCurrentDOByte ptDioGetCurrentDOByte;
	ptDioGetCurrentDOByte.port  = 0;
	ptDioGetCurrentDOByte.value = (USHORT far *)&chValue;

	if((g_pIoCard->m_ErrCde = DRV_DioGetCurrentDOByte(g_pIoCard->m_hDev,
		(LPT_DioGetCurrentDOByte)&ptDioGetCurrentDOByte)) != 0)
	{
		DRV_GetErrorMessage(g_pIoCard->m_ErrCde,(LPSTR)g_pIoCard->m_szErrMsg);
		//MessageBox((LPCSTR)g_pIoCard->m_szErrMsg,"Driver Message",MB_OK);
	}
	
	CString szTemp;
	szTemp.Format( "%02X", chValue );
	SetDlgItemText( IDC_EDIT_FEEDBACK, szTemp );
}


void CDlgTab3::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case 0:
		{
			//每秒查询时序控制器A状态
			BYTE data[6] = {0xBB, 0x22, 0x00, 0x00, 0x22, 0xEE};
			g_pMainDlg->udpAsxHelper.Send(data, 6);
			//每秒查询时序控制器B状态
			g_pMainDlg->udpBsxHelper.Send(data, 6);


			//TCP每秒查询
			char szRecValue[1024] = {0};
			//指令查询代码
			BYTE xx[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02};	
			//StrToHex(, )
			//aSocket.Send(strText, strText.GetLength()); //发送内容给服务器
			//aSocket.Receive((void *)szRecValue, 1024); //接收服务器发送回来的内容(该方法会阻塞, 在此等待有内容接收到才继续向下执行)
			aSocket.Send(xx, 12); //发送内容给服务器
			BYTE Recv[64] = {0};
			aSocket.Receive((BYTE*)Recv, 64); //接收服务器发送回来的内容(该方法会阻塞, 在此等待有内容接收到才继续向下执行)
			//CString str = ((LPCTSTR)szRecValue);
			CString res;
			for(int i = 0; i < 13; i++)
			{
				CString tmp;
				tmp.Format("%02X", Recv[i]);
				res.SetString(res + tmp + " ");
			}
			((CEdit*)GetDlgItem(IDC_EDIT_WSD))->SetWindowText(res);
			//换算温湿度
			float num = Recv[9]*256 + Recv[10];	
			float tmp1 = num / (float)65536;
			float volt = (tmp1*(20-4) + 4)*50/20;
			float wenduNum = tmp1*50;

			CString strWenShidu;
			strWenShidu.Format("%.1f", wenduNum);
			((CEdit*)GetDlgItem(IDC_EDIT_WENSHIDU))->SetWindowText(strWenShidu);
		}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CDlgTab3::IntialCSocket()
{
	//AfxSocketInit();
	//CSocket aSocket;
	//CString strIP = "192.168.3.53";
	//CString strIP = "127.0.0.1";
	CString strIP = g_WsdServerIP;
	//CString strPort = "502";
	int strPort = g_WsdPort;
	CString strText;
	/*this->GetDlgItem(IDC_EDIT_IP)->GetWindowText(strIP);
	this->GetDlgItem(IDC_EDIT_PORT)->GetWindowText(strPort);
	this->GetDlgItem(IDC_EDIT_TEXT)->GetWindowText(strText);*/
	//初始化 CSocket 对象, 因为客户端不需要绑定任何端口和地址, 所以用默认参数即可
	if(!aSocket.Create())
	{
		char szMsg[1024] = {0};

		sprintf(szMsg, "create faild: %d", aSocket.GetLastError());

		//AfxMessageBox(szMsg);
		return;
	}
	//转换需要连接的端口内容类型
	//int nPort = atoi(strPort);
	//连接指定的地址和端口
	int ret = aSocket.Connect(strIP, strPort);
	//if(aSocket.Connect(strIP, nPort))
	//{
	//	char szRecValue[1024] = {0};
	//	aSocket.Send(strText, strText.GetLength()); //发送内容给服务器
	//	aSocket.Receive((void *)szRecValue, 1024); //接收服务器发送回来的内容(该方法会阻塞, 在此等待有内容接收到才继续向下执行)
	//	AfxMessageBox(szRecValue);
	//}
	if(!ret)
	//else
	{
		char szMsg[1024] = {0};
		sprintf(szMsg, "create faild: %d", aSocket.GetLastError());
		//AfxMessageBox(szMsg);
	}
	//aSocket.Close();
}

void CDlgTab3::InitialOphirLM()
{
	// a basic sample of using the CPlus_Demo	
	try
	{
		// Scan for connected Devices
		OphirLM.ScanUSB(serialNumbers);

		if (serialNumbers.size() > 0)
		{
			//open first device
			OphirLM.OpenUSBDevice(serialNumbers[0].c_str(), hOphirLMDevice);	//打开设备
			long index = 0;
			vector<wstring> optionsOld;
			OphirLM.GetWavelengths(hOphirLMDevice, hOphirLMchannel, index, optionsOld);	//得到波长列表
			vector<wstring> optionsNew;
			OphirLM.AddWavelength(hOphirLMDevice, hOphirLMchannel, 1080);	//添加波长
			OphirLM.GetWavelengths(hOphirLMDevice, hOphirLMchannel, index, optionsNew);	//得到波长列表
			OphirLM.SetWavelength(hOphirLMDevice, hOphirLMchannel, 3); //设置波长
			vector<wstring> Filteroptions;
			long Filterindex = 0;
			OphirLM.GetFilter(hOphirLMDevice,hOphirLMchannel, Filterindex, Filteroptions);//得到滤波列表
			OphirLM.SetFilter(hOphirLMDevice, hOphirLMchannel, 0);	////设置滤波 OUT

			vector<wstring> Rangeoptions;
			long Rangeindex = 0;
			OphirLM.GetRanges(hOphirLMDevice, hOphirLMchannel, Rangeindex, Rangeoptions);//得到范围列表	
			OphirLM.SetRange(hOphirLMDevice, hOphirLMchannel, 3);  //设置范围 300uW
			//OphirLM.SaveSettings(hDevice, channel);
			//OphirLM.SaveSettings(hDevice, channel);
			//std:: wcout << L"Ophir device opened with handle " << hDevice << L".\n\n";

			//start measuring on first device
			//OphirLM.RegisterPlugAndPlay(PlugAndPlayCallback);	
			OphirLM.RegisterDataReady(DataReadyCallback);
			OphirLM.StartStream(hOphirLMDevice, hOphirLMchannel);
			
			//A message loop is necessary to receive events. 
			//An alternative is to call GetData in a loop (with a small delay in the loop) and not use events at all.
		/*	MSG Msg;
			while(GetMessage(&Msg, NULL, 0, 0) > 0){
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}*/

			//OphirLM.StopAllStreams(); //stop measuring
			//OphirLM.CloseAll(); //close device
		}
	}

	catch (const _com_error& e)
	{
		//std ::wcout << L"Error 0x" << std::hex << e.Error() << L" " << e.Description()  << L"\n";
		// 0x00000000 (S_OK) : No Error
		// 0x80070057 (E_INVALIDARG) : Invalid Argument
		// 0x80040201 : Device Already Opened
		// 0x80040300 : Device Failed
		// 0x80040301 : Device Firmware is Incorrect
		// 0x80040302 : Sensor Failed
		// 0x80040303 : Sensor Firmware is Incorrect
		// 0x80040306 : This Sensor is Not Supported
		// 0x80040308 : The Device is no longer Available
		// 0x80040405 : Command Failed
		// 0x80040501 : A channel is in Stream Mode
	}
}


void CDlgTab3::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	

	BYTE xx[6] = {0xBB, 0x11, 0x00, 0x00, 0x11, 0xEE};	//时序控制器参数设置指令
	xx[2] = (BYTE)g_delay;
	xx[3] = (BYTE)10;
	xx[4] = 0x11^xx[2]^xx[3];
	g_pMainDlg->udpAsxHelper.Send(xx, 6);		//时序控制器A设置参数
	g_pMainDlg->udpBsxHelper.Send(xx, 6);		//时序控制器B设置参数

	//BYTE aa[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //故障复位帧
	//g_pMainDlg->udpBsxHelper.Send(aa, 6);

	//Sleep(2000);
	g_pIoCard->WriteWord(0, 0);
	g_pIoCard->WriteWord(0, 3);

	//static      PT_DioWritePortByte ptDioWritePortByte; // DioWritePortByte table
	//ptDioWritePortByte.port  = 0;
	//ptDioWritePortByte.mask  = 0xFF;
	//ptDioWritePortByte.state = 0;
	//if((g_pIoCard->m_ErrCde = DRV_DioWritePortByte(g_pIoCard->m_hDev,
	//	(LPT_DioWritePortByte)&ptDioWritePortByte)) != 0)
	//{
	//	DRV_GetErrorMessage(g_pIoCard->m_ErrCde,(LPSTR)g_pIoCard->m_szErrMsg);
	//}
	//Sleep(50);
	//ptDioWritePortByte.port  = 0;
	//ptDioWritePortByte.mask  = 0xFF;
	//ptDioWritePortByte.state = 3;
	//if((g_pIoCard->m_ErrCde = DRV_DioWritePortByte(g_pIoCard->m_hDev,
	//	(LPT_DioWritePortByte)&ptDioWritePortByte)) != 0)
	//{
	//	DRV_GetErrorMessage(g_pIoCard->m_ErrCde,(LPSTR)g_pIoCard->m_szErrMsg);
	//}

}
