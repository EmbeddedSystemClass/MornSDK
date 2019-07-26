// MornSDKDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "General.h"
#include "MornSDK.h"
#include "MornSDKDlg.h"
#include "afxdialogex.h"

#include "wordHeader/CApplication.h"
#include "wordHeader/CDocuments.h"
#include "wordHeader/CDocument0.h"
#include "wordHeader/CRange.h"
//#include "wordHeader/CParagraph.h"
//#include "wordHeader/CParagraphs.h"
#include "wordHeader/CCell.h"
#include "wordHeader/CCells.h"
#include "wordHeader/CTable0.h"
#include "wordHeader/CTables0.h"
//#include "wordHeader/CBorder.h"
//#include "wordHeader/CBorders.h"
#include "wordHeader/CFont0.h"
//#include "wordHeader/CPageSetup.h"
#include "wordHeader/CSelection.h"
//#include "wordHeader/CChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CMornSDKDlg 对话框

CMornSDKDlg::CMornSDKDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMornSDKDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//将当前窗口指针赋值给全局指针
	g_pMainDlg = this;

	isStartQg = 0; //初始值 未出光
	udpHelper.setValue(g_iLocalPort, g_iRemotePort, g_strServerIP);
	udpAsxHelper.setValue(g_isxALocalPort, g_isxARemotePort, g_sxAServerIP);
	udpBsxHelper.setValue(g_isxBLocalPort, g_isxBRemotePort, g_sxBServerIP);

	isZuo = 0;
	serialStr = "";
	m_index = 0;

	SetScale(70);
	// 设置绘制的函数
	//SetDrawFunction(Sin);
	// 设置自变量x范围[startX, endX]、取样步长step
	SetPlotSin((float)0.0, (float)6.3, (float)0.01);
}

void CMornSDKDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABCTRL, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CMornSDKDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCTRL, &CMornSDKDlg::OnTcnSelchangeTabctrl)
	//ON_MESSAGE(ON_COM_RECEIVE, OnCnCommRecv)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CMornSDKDlg 消息处理程序

BOOL CMornSDKDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	::SetWindowPos(this->m_hWnd,HWND_BOTTOM,0,0,1280,1024,SWP_NOZORDER|SWP_NOMOVE);
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//为Tab Control增加页面
	m_tabCtrl.InsertItem(0, _T("主页面"));
	m_tabCtrl.InsertItem(1, _T("电池状态"));
	m_tabCtrl.InsertItem(2, _T("485"));
	m_tabCtrl.InsertItem(3, _T("页面4"));

	//创建两个对话框
	m_Page1.Create(IDD_Tab1, &m_tabCtrl);
	m_Page2.Create(IDD_Tab2, &m_tabCtrl);
	m_Page3.Create(IDD_Tab3, &m_tabCtrl);
	m_Page4.Create(IDD_Tab4, &m_tabCtrl);

	//设定在Tab内显示的范围
	CRect rc;
	m_tabCtrl.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_Page1.MoveWindow(&rc);
	m_Page2.MoveWindow(&rc);
	m_Page3.MoveWindow(&rc);
	m_Page4.MoveWindow(&rc);
	//把对话框对象指针保存起来
	m_pPages[0] = &m_Page1;
	m_pPages[1] = &m_Page2;
	m_pPages[2] = &m_Page3;
	m_pPages[3] = &m_Page4;

	//显示初始页面
	m_pPages[0]->ShowWindow(SW_SHOW);
	m_pPages[1]->ShowWindow(SW_HIDE);
	m_pPages[2]->ShowWindow(SW_HIDE);
	m_pPages[3]->ShowWindow(SW_HIDE);

	//调用各页面的初始化函数
	((CDlgTab1*)m_pPages[0])->InitPage();
	((CDlgTab2*)m_pPages[1])->InitPage();
	((CDlgTab3*)m_pPages[2])->InitPage();
	((CDlgTab4*)m_pPages[3])->InitPage();

	//保存当前选择
	m_iCurTab = 0;

	//电源A CAN1
	g_pCanManager1 = new CCanManager();
	g_pCanManager1->OpenCanPort("CAN6", TRUE);
	acSetTimeOut(g_pCanManager1->m_hDevice, 50, 50);
	//电源B CAN2
	g_pCanManager2 = new CCanManager();
	g_pCanManager2->OpenCanPort("CAN4", TRUE);
	acSetTimeOut(g_pCanManager2->m_hDevice, 50, 50);
	//种子源 CAN3
	g_pCanManager3 = new CCanManager();
	g_pCanManager3->OpenCanPort("CAN3", TRUE);
	acSetTimeOut(g_pCanManager3->m_hDevice, 50, 50);
	//电池  CAN3
	/*g_pCanManager4 = new CCanManager();
	g_pCanManager4->OpenCanPort("CAN5", TRUE);*/
	/*for (int i = 0; i < 160; i++)
	{
		g_localDataLog[i] = "                                     ";
	}*/
	options.Add(_T("无故障"));
	options.Add(_T("1模块故障"));
	options.Add(_T("2模块故障"));
	options.Add(_T("3模块故障"));
	options.Add(_T("4模块故障"));
	options.Add(_T("5模块故障"));
	options.Add(_T("6模块故障"));
	options.Add(_T("母线欠压"));
	options.Add(_T("总输出故障"));
	options.Add(_T("超温故障"));
	options.Add(_T("水流故障"));
	options.Add(_T("刹车故障"));
	options.Add(_T("IGBT故障"));
	//初始化串口
	//InitCnComm();
	//初始化主UDP
	udpHelper.OpenAsServer();
	//初始化时序控制器A UDP
	udpAsxHelper.OpenAsServer();
	//初始化时序控制器B UDP
	udpBsxHelper.OpenAsServer();

	//打开IO卡
	g_pIoCard->OpenIOCard(2);
	SetTimer(0, 1000, NULL);
	SetTimer(1, 6000, NULL);
	SetTimer(2, 2000, NULL); //本地数据保存定时器
	SetTimer(3, 5000, NULL); //本地数据保存定时器
	//初始化串口
	InitSerial();
	// 设置串口数据接收函数
	g_pSP->readReady.connect(this, &CMornSDKDlg::OnSerialPortReceived);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMornSDKDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMornSDKDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMornSDKDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//----------------------------------------------------------------------
//程序退出处理
void CMornSDKDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (MessageBox(_T("确定要退出程序吗？"), _T("退出提示"), MB_ICONQUESTION|MB_YESNO) == IDNO)
		return;
	g_bAppRun = FALSE;
	g_bBatThreadReadFlag = FALSE;
	g_bCanManagerThreadReadFlag = FALSE;
	Sleep(5);
	g_pCanManager1->CloseCanPort();
	g_pCanManager2->CloseCanPort();
	g_pCanManager3->CloseCanPort();
	g_pCanManager4->CloseCanPort();
	//g_pCanManager1->m_hDevice = INVALID_HANDLE_VALUE;
	//g_pCanManager2->m_hDevice = INVALID_HANDLE_VALUE;
	//g_pCanManager3->m_hDevice = INVALID_HANDLE_VALUE;
	//g_pCanManager4->m_hDevice = INVALID_HANDLE_VALUE;
	if (g_pCnCommToUp)
	{
		delete g_pCnCommToUp;
		g_pCnCommToUp = NULL;
	}
	if (g_pCnCommToDown)
	{
		delete g_pCnCommToDown;
		g_pCnCommToDown = NULL;
	}
	if (g_pCanManager1)
	{
		delete g_pCanManager1;
		g_pCanManager1 = NULL;
	}
	if (g_pCanManager2)
	{
		delete g_pCanManager2;
		g_pCanManager2 = NULL;
	}
	if (g_pCanManager3)
	{
		delete g_pCanManager3;
		g_pCanManager3 = NULL;
	}
	if (g_pCanManager4)
	{
		delete g_pCanManager4;
		g_pCanManager4 = NULL;
	}
	if (g_pSP)
	{
		delete g_pSP;
		g_pSP = NULL;
	}
	//YWJ----------------
	if (g_pIoCard)
	{
		delete g_pIoCard;
		g_pIoCard = NULL;
	}
	//YWJ----------------
	CDialogEx::OnClose();
	_CrtDumpMemoryLeaks();

}



//初始化串口
void CMornSDKDlg::InitCnComm()
{
	if(!g_pCnCommToUp->IsOpen())
	{
		if (g_pCnCommToUp->Open(g_iComPortToUp, 115200))
		{
			g_pCnCommToUp->SetWnd(this->GetSafeHwnd());
			g_pCnCommToUp->SetBufferSize(100,100);
			g_Log.WriteLog(LOG_INF, "打开串口ToUp成功。");
			//MessageBox(_T("打开串口ToUp成功。"),_T("提示"));
		}
		else
		{
			g_Log.WriteLog(LOG_ERR, "打开串口ToUp失败。");
			MessageBox(_T("打开串口ToUp失败。"),_T("提示"));
		}	
	}
	if(!g_pCnCommToDown->IsOpen())
	{
		if (g_pCnCommToDown->Open(g_iComPortToDown, 115200))
		{
			g_pCnCommToDown->SetWnd(this->GetSafeHwnd());
			g_pCnCommToDown->SetBufferSize(300,300);
			g_Log.WriteLog(LOG_INF, "打开串口ToDown成功。");
			//MessageBox(_T("打开串口ToDown成功。"),_T("提示"));
		}
		else
		{
			g_Log.WriteLog(LOG_ERR, "打开串口ToDown失败。");
			MessageBox(_T("打开串口ToDown失败。"),_T("提示"));
		}	
	}
}

//串口接收处理
//LRESULT CMornSDKDlg::OnCnCommRecv(WPARAM wPARAM, LPARAM lPARAM)
//{
//	uint iPort = wPARAM;
//	uchar data[512]= {0};
//	int iRead = 0;
//	CString str;
//	if (iPort == g_iComPortToUp)
//	{
//		iRead = g_pCnCommToUp->Read(data,512);
//	}
//	else if (iPort == g_iComPortToDown)
//	{
//		iRead = g_pCnCommToDown->Read(data,512);
//	}
//	if (iRead == 0)
//		return 0;
//	if (iPort == g_iComPortToUp)
//	{
//		str = _T("SPToUp接收到数据:");
//		CString msg((char*)data);
//
//		//CString str2;
//		//str2 = str + msg;
//		//m_Page3.GetDlgItem(IDC_EDIT_COMRECV)->SetWindowText(str2);
//
//		//if(msg.Find('<') >= 0 && msg.Find('>') < 0) //找到做括号，找不到右括号
//		//{
//		//	isZuo = 1;
//		//	serialStr += msg;
//		//	return 0;
//		//}
//		//if(msg.Find('>') >= 0 && isZuo == 1)
//		//{
//		//	isZuo = 0;
//		//	serialStr += msg;
//		//}
//		//serialStr += msg;
//		//if(msg.Find('<') < 0  || msg.Find('>') < 0 )
//		//{
//		//	return 0;
//		//}
//
//
//		//解析来自上位机的串口字符串
//		//AnalyzStringFromUp(msg);
//
//
//		msg = str + msg + "\r\n";
//		CString str2;
//		//先获取文本框中已有的文本
//		m_Page3.GetDlgItem(IDC_EDIT_COMRECV)->GetWindowText(str2);
//		//添加新文本
//		str2 += msg;
//		//str2 += serialStr + "\r\n";
//		m_Page3.GetDlgItem(IDC_EDIT_COMRECV)->SetWindowText(str2);
//		//serialStr = "";
//	}
//	else if (iPort == g_iComPortToDown)
//	{
//		str = _T("SPToDown接收到数据:");
//		CHAR strHex[512] = {0};
//		::BinArrayToHexStr(data, strHex, iRead);
//		CString msg(strHex);
//		msg = str + msg + "\r\n";
//		CString str2;
//		//先获取文本框中已有的文本
//		m_Page3.GetDlgItem(IDC_EDIT_COMRECV)->GetWindowText(str2);
//		//添加新文本
//		str2 += msg;
//		m_Page3.GetDlgItem(IDC_EDIT_COMRECV)->SetWindowText(str2);
//
//		//YWJ----------------
//		//无效的数据包
//		if (iRead != 6 || data[0]!= 0xBB || data[5] != 0xEE)
//		{
//			g_Log.WriteLog(LOG_ERR, "接收到无效的下位机串口数据包");
//			return 0;
//		}
//		if (data[1] = 0xff)	//下方了错误的命令字
//		{
//			g_Log.WriteLog(LOG_ERR, "下发了错误的下位机串口命令字");
//			return 0;
//		}
//		if (data[1] = 0x21)		//查询参数帧
//		{
//			//处理数据
//			//data[2] 预主放间隔时间
//			//data[3] 出光时长
//		}
//		else if (data[1] = 0x22)	//查询工作状态
//		{
//			//处理数据
//			//返回9台电源的工作状态，bit15~9保留，bit8：预放状态；bit7~0：主放7~0状态
//			//data[2] 低8位
//			//data[3] 高8位
//		}
//		//YWJ----------------
//	}
//	return 0;
//}

//解析来自上位机的串口字符串
int CMornSDKDlg::AnalyzStringFromUp(CString s)
{
	//<src,dst/cmd:data1,data2,…,data10*checksum16>\n
	int zuoPos = s.Find('<');
	int youPos = s.Find('>');
	if( zuoPos < 0 || youPos < 0) //首先检验括号匹配，如果不匹配，直接返回错误
	{
		return -1;
		g_Log.WriteLog(LOG_ERR, "接收到无效的上位机串口数据包");
	}
	int _kang = s.Find('/');
	int _mao = s.Find(':');
	int _xing = s.Find('*');
	int _youKuohao = s.Find('>');
	int checkSum16FromToUp = atoi(s.Mid(_xing + 1, _youKuohao - _xing -1));
	unsigned short checkSum16 = 0;
	//splitStrUdp(1102, 1, 2);  //UDP反馈
	if(_mao > 0)
	{
		CString command = s.Mid(_kang + 1, _mao - _kang - 1);
		//MessageBox(command, "command");
		CStringArray strArr;
		vector<int> v;
		v.push_back(_mao);
		for(int i = _mao; i < s.GetLength(); i++)
		{
			if(s[i] == ',')
				v.push_back(i);
		}
		v.push_back(_xing);
		vector<CString> data;
		int vLen = v.size();
		for(int i = 1; i < vLen; i++)
		{
			data.push_back(s.Mid(v[i-1] + 1, v[i] - v[i-1] - 1));
		}
		//for(int i = 0; i < data.size(); i++)	
		//	MessageBox(data[i], "test");

		CString mid;
		for(int i = 1; i < _xing; i++)
		{
			mid += s[i];
			checkSum16 += (BYTE)s[i];
		}
		//if(checkSum16 == checkSum16FromToUp)
		{
			//MessageBox("check success", "check");
			//执行相应动作。。

			if(command == "start_Test")//开始出光
			{
				isStartQg = 1; //开始出光了 文件开始写入
				if(data[0] == "two")
				{
					if(!g_isQgCmd_Aoutput || !g_isQgCmd_Boutput)
					{
						BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; 
						g_pMainDlg->udpAsxHelper.Send(xx, 6);	//复位A时序控制器
						Sleep(5);
						g_pMainDlg->udpBsxHelper.Send(xx, 6);	//复位B时序控制器
						Sleep(5);
						//控制IO卡,high 开
						USHORT chValue = g_pIoCard->OnReadback();
						chValue &= 0xfffc;
						g_pIoCard->WriteWord(0, chValue);
						chValue |= 3;
						g_pIoCard->WriteWord(0, chValue);

						splitStrUdp(1104, 3, 0);
						g_isQgCmd_Aoutput = TRUE;
						g_isQgCmd_Boutput = TRUE;
					}

				}
				if(data[0] == "as1")
				{
					if(!g_isQgCmd_Aoutput)
					{
						BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //复位A时序控制器
						g_pMainDlg->udpAsxHelper.Send(xx, 6);
						Sleep(5);
						//控制IO卡,high 开
						USHORT chValue = g_pIoCard->OnReadback();
						chValue &= 0xfffe;
						g_pIoCard->WriteWord(0, chValue);
						chValue |= 1;
						g_pIoCard->WriteWord(0, chValue);

						splitStrUdp(1104, 1, 0);
						g_isQgCmd_Aoutput = TRUE;	
					}
				}
				if(data[0] == "bs2")
				{
					if(!g_isQgCmd_Boutput)
					{
						BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //复位B时序控制器
						g_pMainDlg->udpBsxHelper.Send(xx, 6);
						Sleep(5);
						//控制IO卡,high 开
						USHORT chValue = g_pIoCard->OnReadback();
						//chValue &= 0xfffd;
						chValue &= 0xfffc;
						g_pIoCard->WriteWord(0, chValue);
						//chValue |= 2;
						chValue |= 3;
						g_pIoCard->WriteWord(0, chValue);

						splitStrUdp(1104, 2, 0);
						g_isQgCmd_Boutput = TRUE;
					}
				}
			}
			else if(command == "stop_Test")//停止出光
			{
				
				isStartQg = 0; //停止出光 文件停止写入
				if(data[0] == "two")
				{
					if(g_isQgCmd_Aoutput || g_isQgCmd_Boutput)
					{
						//控制IO卡,low 关
						USHORT chValue = g_pIoCard->OnReadback();
						chValue &= 0xFFFC;
						g_pIoCard->WriteWord(0, chValue);
						g_isQgCmd_Aoutput = FALSE;
						g_isQgCmd_Boutput = FALSE;
						splitStrUdp(1104, 3, 0);
					}
					
				}
				if(data[0] == "as1")
				{
					if(g_isQgCmd_Aoutput)
					{
						//控制IO卡,low 关
						USHORT chValue = g_pIoCard->OnReadback();
						chValue &= 0xFFFE;
						g_pIoCard->WriteWord(0, chValue);
						g_isQgCmd_Aoutput = FALSE;
						splitStrUdp(1104, 1, 0);
					}
					
				}
				if(data[0] == "bs2")
				{
					if(g_isQgCmd_Boutput)
					{
						//控制IO卡,low 关
						USHORT chValue = g_pIoCard->OnReadback();
						//chValue &= 0xFFFD;
						chValue &= 0xFFFC;
						g_pIoCard->WriteWord(0, chValue);
						g_isQgCmd_Boutput = FALSE;
						splitStrUdp(1104, 2, 0);
					}	
				}	
			}
			else if(command == "brakeSys_Test")//刹车
			{
				if(data[0] == "two")
					;
				else if(data[0] == "as1")
					;
				else if(data[0] == "bs2")
					;
			}
			
		}
		/*else
		{
			MessageBox("上级系统向电源checkSum失败", "Fail");
			return -1;
		}
		MessageBox(mid, "mid");*/
	}
	else
	{
		CString mid;
		for(int i = 1; i < _xing; i++)
		{
			mid += s[i];
			checkSum16 += s[i];
		}
		//MessageBox(mid, "mid");
	}
	return 1;
}

//YWJ----------------
//通过RS422下发串口命令
void CMornSDKDlg::SendSPCmdToDown(uchar cmd, uchar data1, uchar data2)
{
	//非有效命令
	if (cmd < 0x11 || cmd > 0x22)
		return;
	//cmd : 11H  参数设置帧   12H  故障复位   13H 输出  14H 输出停止  21H 查询参数帧  22H 查询工作状态
	BYTE pack[6] = {0};
	pack[0] = 0xBB;
	pack[1] = cmd;
	pack[2] = data1;
	pack[3] = data2;
	pack[4] = cmd + data1 + data2;
	pack[5] = 0xEE;
	//发送
	g_pCnCommToDown->Write(pack, 6);
}
//YWJ----------------

void CMornSDKDlg::SendUDPCmdToDownA(uchar cmd, uchar data1, uchar data2)
{
	//非有效命令
	if (cmd < 0x11 || cmd > 0x22)
		return;
	//cmd : 11H  参数设置帧   12H  故障复位   13H 输出  14H 输出停止  21H 查询参数帧  22H 查询工作状态
	BYTE pack[6] = {0};
	pack[0] = 0xBB;
	pack[1] = cmd;
	pack[2] = data1;
	pack[3] = data2;
	pack[4] = cmd + data1 + data2;
	pack[5] = 0xEE;
	//发送
	//g_pCnCommToDown->Write(pack, 6);
	g_pMainDlg->udpAsxHelper.Send(pack, 6);
}

void CMornSDKDlg::SendUDPCmdToDownB(uchar cmd, uchar data1, uchar data2)
{
	//非有效命令
	if (cmd < 0x11 || cmd > 0x22)
		return;
	//cmd : 11H  参数设置帧   12H  故障复位   13H 输出  14H 输出停止  21H 查询参数帧  22H 查询工作状态
	BYTE pack[6] = {0};
	pack[0] = 0xBB;
	pack[1] = cmd;
	pack[2] = data1;
	pack[3] = data2;
	pack[4] = cmd + data1 + data2;
	pack[5] = 0xEE;
	//发送
	//g_pCnCommToDown->Write(pack, 6);
	g_pMainDlg->udpBsxHelper.Send(pack, 6);
}

//拼接udp字符串
void CMornSDKDlg::splitStrUdp(int cmd, int index, int res)//index16位，res17位
{
	uchar data[8+2+2+2+2+2+2]={255, 255, 255, 255, 255, 255, 68, 90};
	uchar head = 8;
	uchar packetLen = 8;
	//data[8] = packetLen / 256; //长度
	//data[9] = packetLen % 256;
	//data[10] = data[8]; //长度取反
	//data[11] = data[9];
	//data[12] = cmd / 256;
	//data[13] = cmd % 256;*
	//data[15] = 0;      //ack0
	data[8] = packetLen % 256; //长度
	data[9] = packetLen / 256;
	data[10] = ~data[8]; //长度取反
	data[11] = ~data[9];
	data[12] = cmd % 256;
	data[13] = cmd / 256;
	data[14] = 0;      //ack0
	data[16] = index;  //数据区内容
	data[17] = res;
	int sum = checkSum16(data, 8+8+4); //效验码
	//data[18] = sum / 256;
	//data[19] = sum % 256;
	data[18] = sum % 256;
	data[19] = sum / 256;
	udpHelper.Send(data, 8+4+8); //第三步，作战任务下发指令执行状态反馈
}

//解析时序控制UDP反馈A
void CMornSDKDlg::OnUdpReceivedFromSXA(uchar* pData, int len)
{
	CString str = strToHex(pData, len);
	//MessageBox(str, "A");
	if (len != 6 || pData[0]!= 0xBB || pData[5] != 0xEE)
	{
		//g_Log.WriteLog(LOG_ERR, "接收到无效的下位机串口数据包");
		return ;
	}
	if (pData[1] = 0xff)	//下方了错误的命令字
	{
		//g_Log.WriteLog(LOG_ERR, "下发了错误的下位机串口命令字");
		return ;
	}
	if (pData[1] = 0x21)		//查询参数帧
	{
		//处理数据
		//data[2] 预主放间隔时间
		//data[3] 出光时长
	}
	else if (pData[1] = 0x22)	//查询工作状态
	{
		//处理数据
		//返回9台电源的工作状态，bit15~9保留，bit8：预放状态；bit7~0：主放7~0状态
		//data[2] 低8位
		//data[3] 高8位
		//if(counter == 3)
		//{
		//	
		//	if(g_isQgCmd_Aoutput && (!g_isQgCmd_Boutput))
		//	{
		//		if((pData[2]&1) && (pData[3] == 0xff))
		//		{
		//			splitStrUdp(1104, 1, 0);
		//		}
		//		else
		//		{
		//			splitStrUdp(1104, 1, 1);
		//		}
		//		g_isQgCmd_Aoutput = FALSE;
		//		counter = 0;
		//	}
		//	else if(g_isQgCmd_Aoutput && g_isQgCmd_Boutput)
		//	{
		//		if((pData[2]&1) && (pData[3] == 0xff))
		//		{
		//			g_isQgAllFlag++;
		//		}
		//		else
		//		{
		//			splitStrUdp(1104, 3, 1);
		//		}
		//	}
		//}
	}
}

//解析时序控制UDP反馈B
void CMornSDKDlg::OnUdpReceivedFromSXB(uchar* pData, int len)
{
	CString str = strToHex(pData, len);
	//MessageBox(str, "B");
	if (len != 6 || pData[0]!= 0xBB || pData[5] != 0xEE)
	{
		//g_Log.WriteLog(LOG_ERR, "接收到无效的下位机串口数据包");
		return ;
	}
	if (pData[1] = 0xff)	//下方了错误的命令字
	{
		//g_Log.WriteLog(LOG_ERR, "下发了错误的下位机串口命令字");
		return ;
	}
	if (pData[1] = 0x21)		//查询参数帧
	{
		//处理数据
		//data[2] 预主放间隔时间
		//data[3] 出光时长
	}
	else if (pData[1] = 0x22)	//查询工作状态
	{
		//处理数据
		//返回9台电源的工作状态，bit15~9保留，bit8：预放状态；bit7~0：主放7~0状态
		//data[2] 低8位
		//data[3] 高8位
		//if(counter == 3)
		//{
		//	if(g_isQgCmd_Boutput && (!g_isQgCmd_Aoutput))
		//	{
		//		if((pData[2]&1) && (pData[3] == 0xff))
		//		{
		//			splitStrUdp(1104, 2, 0);
		//		}
		//		else
		//		{
		//			splitStrUdp(1104, 2, 1);
		//		}
		//		g_isQgCmd_Boutput = FALSE;
		//		counter = 0;
		//	}
		//	else if(g_isQgCmd_Aoutput && g_isQgCmd_Boutput)
		//	{
		//		if((pData[2]&1) && (pData[3] == 0xff))
		//		{
		//			g_isQgAllFlag++;
		//			if(g_isQgAllFlag == 2)
		//				splitStrUdp(1104, 3, 0);
		//			else
		//				splitStrUdp(1104, 3, 1);
		//		}
		//		else
		//		{
		//			splitStrUdp(1104, 3, 1);
		//		}
		//		g_isQgCmd_Aoutput = FALSE;
		//		g_isQgCmd_Boutput = FALSE;
		//		counter = 0;
		//	}
		//}
	}
}

//Udp数据接收处理
void CMornSDKDlg::OnUdpReceived(uchar* pData, int len)
{
	// 将接收到的有用的信息赋值给CString变量
	
	CString str = strToHex(pData, len);
	uchar tmp[1024];
	memcpy(&tmp, pData, len);
	//CString strRecvd(pData);
	CString  strRecvd = "UDP接收到数据：" + str;

	//g_Log.WriteLog(LOG_DBG, strRecvd);
	m_Page3.GetDlgItem(IDC_EDIT_UDPRECV)->SetWindowText(strRecvd);

	if(len < 18)
	{
		g_Log.WriteLog(LOG_ERR, "UDP123包长度不符合规则");
		return;
	}
	for(int i = 0; i < 6; i++)
	{
		if(pData[i] != 0xff)
		{
			g_Log.WriteLog(LOG_ERR, "UDP包0-5字节不符合规则");
			return;
		}
	}
	//ushort cmd = pData[12]*256+pData[13];
	ushort cmd = pData[12] + pData[13]*256;
	switch (cmd)
	{
	//武控->电源（共性指令）
	case 1001:			//作战任务下发指令
		{
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13] + 1;
			//pData[15] = 0;			//ack=0
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //第一步，作战任务下发指令反馈
			ParseUdpTaskDown(pData);	//第二部，执行动作
			g_Log.WriteLog(LOG_INF, "作战任务下发指令");	//程序操作日志
		}
		break;
	case 1101:			//强光控制指令
		{
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13] + 1; //cmd++
			//pData[15] = 0;			//ack=0
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //第一步，强光控制指令指令反馈
			ParseUdpQGCtrl(pData);		 //第二部，执行动作
		}
		break;
	case 1025:			//数据上报指令
		{
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13] + 1; //cmd++
			//pData[15] = 0;			//ack=0
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //ack=2 返回下发指令
			ParseUdpDataUp(pData);
			g_Log.WriteLog(LOG_INF, "数据上报指令");	//程序操作日志
		}
		break;;
	case 1105:			//系统流程控制指令
		{
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13] + 1; //cmd++
			//pData[15] = 0;			//ack=0
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //ack=2 返回下发指令
			ParseUdpSysCtrl(pData, len);
		}
		break;
	//上级->电源（个性指令）
	case 2001:			//种子源设置电流指令
		{
			g_Log.WriteLog(LOG_INF, "个性指令 种子源设置电流指令");	//程序操作日志
			/*
			I：ACK=1
			数据区内容：
			Int16 dx;//3-双路，1-A 单路，2-B 单路
			Int16 zdj;//振荡级电流，单位 A
			Int16 fdj1;//放大级 1 电流，单位 A
			Int16 fdj2;//放大级 2 电流，单位 A
			预留：填 0（byte）
			预留：填 0（byte）
			*/
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13] + 1; //cmd++
			//pData[15] = 0;				//ack确认
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //返回下发指令

			int dx   = pData[17]*256 + pData[16];
			int zdj  = pData[19]*256 + pData[18];
			int fdj1 = pData[21]*256 + pData[20];
			int fdj2 = pData[23]*256 + pData[22];
			int fdj3 = pData[25]*256 + pData[24];
			if(dx == 3) //双路
			{
				g_pCanManager3->Output_CurrentSupply(16, zdj, fdj1, fdj2, fdj3, 500, NULL);
				g_pCanManager3->Output_CurrentSupply(16, zdj, fdj1, fdj2, fdj3, 500, NULL);
			}
			if(dx == 1) //A
			{
				g_pCanManager3->Output_CurrentSupply(16, zdj, fdj1, fdj2, fdj3, 500, NULL);
			}
			if(dx == 2) //B
			{
				g_pCanManager3->Output_CurrentSupply(16, zdj, fdj1, fdj2, fdj3, 500, NULL);
			}
		}
		break;
	case 2101:			//种子源/弱光控制指令
		{
			g_Log.WriteLog(LOG_INF, "个性指令 种子源/弱光控制指令");	//程序操作日志
			/*
			I：ACK=1； 
			BYTE bytePara[2]；
			bytePara[0]：
			控制对象： 1- 种子源双路 2- 种子源 A 路 3- 种子源 B 路 4- 弱光双路 5- 弱光 A 路 6- 弱光 B 路 
			bytePara[1]：控制动作： 1- 输出； 2- 停止输出； 3- 复位；
			*/
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13] + 1; //cmd++
			//pData[15] = 0;		//ack确认
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //返回下发指令
			BYTE bytePara[2];
			bytePara[0] = pData[16];
			bytePara[1] = pData[17];
			if(bytePara[0] == 1)
			{
				//种子源双路
				if(bytePara[1] == 1) //输出
				{
					g_pCanManager3->SendFrame_SeedSource_Output(16, 500, NULL);
				//	g_pCanManager3->SendFrame_SeedSource_Output(16, 500, NULL);
					BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //复位A时序控制器
					g_pMainDlg->udpAsxHelper.Send(xx, 6);
					Sleep(5);
					g_pMainDlg->udpBsxHelper.Send(xx, 6);
					Sleep(5);
				}
				if(bytePara[1] == 2) //停止输出
				{
					g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
				//	g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
				}
				if(bytePara[1] == 3) //复位
				{
					g_pCanManager3->Reset_SeedSource(16, 500, NULL);
				//	g_pCanManager3->Reset_SeedSource(16, 500, NULL);
				}
			}
			if(bytePara[0] == 2)
			{
				//种子源A路
				if(bytePara[1] == 1) //输出
				{
					g_pCanManager3->SendFrame_SeedSource_Output(16, 500, NULL);
					BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //复位A时序控制器
					g_pMainDlg->udpAsxHelper.Send(xx, 6);
					Sleep(5);
					g_pMainDlg->udpBsxHelper.Send(xx, 6);
					Sleep(5);
				}
				if(bytePara[1] == 2) //停止输出
				{
					g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
				}
				if(bytePara[1] == 3) //复位
				{
					g_pCanManager3->Reset_SeedSource(16, 500, NULL);
				}
			}
			if(bytePara[0] == 3)
			{
				//种子源B路
				if(bytePara[1] == 1) //输出
				{
					g_pCanManager3->SendFrame_SeedSource_Output(16, 500, NULL);
					BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //复位A时序控制器
					g_pMainDlg->udpAsxHelper.Send(xx, 6);
					Sleep(5);
					g_pMainDlg->udpBsxHelper.Send(xx, 6);
					Sleep(5);
				}
				if(bytePara[1] == 2) //停止输出
				{
					g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
				}
				if(bytePara[1] == 3) //复位
				{
					g_pCanManager3->Reset_SeedSource(16, 500, NULL);
				}
			}
			if(bytePara[0] == 4)
			{
				//弱光双路
				//1.先设置电流
				//2.再输出
				if(bytePara[1] == 1) //输出
				{
					g_pCanManager3->Output_CurrentSupply(16, g_ruoGuangValueA, 0, 0, 0, 500, NULL);
					g_pCanManager3->Output_CurrentSupply(16, g_ruoGuangValueB, 0, 0, 0, 500, NULL);
					Sleep(20);
					g_pCanManager3->SendFrame_SeedSource_Output(16, 500, NULL);
				//	g_pCanManager3->SendFrame_SeedSource_Output(16, 500, NULL);
				}
				if(bytePara[1] == 2) //停止输出
				{
					g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
				//	g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
				}
				if(bytePara[1] == 3) //复位
				{
					g_pCanManager3->Reset_SeedSource(16, 500, NULL);
				//	g_pCanManager3->Reset_SeedSource(16, 500, NULL);
				}
			}
			if(bytePara[0] == 5)
			{
				//弱光A路
				//1.先设置电流
				//2.再输出
				if(bytePara[1] == 1) //输出
				{
					g_pCanManager3->Output_CurrentSupply(16, g_ruoGuangValueA, 0, 0, 0, 500, NULL);
					Sleep(20);
					g_pCanManager3->SendFrame_SeedSource_Output(16, 500, NULL);
				}
				if(bytePara[1] == 2) //停止输出
				{
					g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
				}
				if(bytePara[1] == 3) //复位
				{
					g_pCanManager3->Reset_SeedSource(16, 500, NULL);
				}
			}
			if(bytePara[0] == 6)
			{
				//弱光B路
				//1.先设置电流
				//2.再输出
				if(bytePara[1] == 1) //输出
				{
					g_pCanManager3->Output_CurrentSupply(16, g_ruoGuangValueB, 0, 0, 0, 500, NULL);
					Sleep(20);
					g_pCanManager3->SendFrame_SeedSource_Output(16, 500, NULL);
				}
				if(bytePara[1] == 2) //停止输出
				{
					g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
				}
				if(bytePara[1] == 3) //复位
				{
					g_pCanManager3->Reset_SeedSource(16, 500, NULL);
				}
			}
		}
		break;
	case 2033:			//电源设置出光电流指令
		{
			g_Log.WriteLog(LOG_INF, "个性指令 电源设置出光电流指令");	//程序操作日志
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13] + 1; //cmd++
			//pData[15] = 0;     //ack确认
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //返回下发指令

			int dx = pData[16];
			//pData[17]预留
			//WORD index = pData[18]*256 + pData[19];
			WORD index = pData[19]*256 + pData[18];
			WORD set[10]={0};
			for(int i = 0; i <= 9; i++)
			{
				//set[i] = pData[20 + i*2]*256 + pData[20 + i*2 +1];
				set[i] = pData[20 + i*2] + pData[20 + i*2 +1]*256;
			}
			if(dx == 1) //A
			{
				for(int i = 1; i < 10; i++)
				{
					//从低位开始设置
					if(index&1)
					{
						CString str;
						str.Format("%d", set[i]);
						CString keyValue = g_CfgTask.GetKeyValue( str ,"PowerSupply"); 
						int tmp = 0;
						vector<int> v;//用来存每个逗号之间的数字	
						keyValue += ',';	  //末尾添加，好判断
						for(int j = 0; j < keyValue.GetLength(); j++)
						{
							if(keyValue[j] == ',')
							{
								v.push_back(tmp);
								tmp = 0;
							}
							else
							{
								tmp = tmp*10 + keyValue[j] - '0';
							}
						}
						g_pCanManager1->SetParameter_PowerSupply(32 + i, v[0], v[1], v[2], v[3], v[4], 500, NULL);
					}
					index = index >> 1;
				}
			}
			if(dx == 2)//B
			{
				for(int i = 1; i < 10; i++)
				{
					//从低位开始设置
					if(index&1)
					{
						CString str;
						str.Format("%d", set[i]);
						CString keyValue = g_CfgTask.GetKeyValue( str ,"PowerSupply"); 
						int tmp = 0;
						vector<int> v;//用来存每个逗号之间的数字	
						keyValue += ',';	  //末尾添加，好判断
						for(int j = 0; j < keyValue.GetLength(); j++)
						{
							if(keyValue[j] == ',')
							{
								v.push_back(tmp);
								tmp = 0;
							}
							else
							{
								tmp = tmp*10 + keyValue[j] - '0';
							}
						}
						g_pCanManager2->SetParameter_PowerSupply(32 + i, v[0], v[1], v[2], v[3], v[4], 500, "2");
					}
					index = index >> 1;
				}
			}
			if(dx == 3)//AB
			{
				for(int i = 1; i < 10; i++)
				{
					//从低位开始设置
					if(index&1)
					{
						CString str;
						str.Format("%d", set[i]);
						CString keyValue = g_CfgTask.GetKeyValue( str ,"PowerSupply"); 
						int tmp = 0;
						vector<int> v;//用来存每个逗号之间的数字	
						keyValue += ',';	  //末尾添加，好判断
						for(int j = 0; j < keyValue.GetLength(); j++)
						{
							if(keyValue[j] == ',')
							{
								v.push_back(tmp);
								tmp = 0;
							}
							else
							{
								tmp = tmp*10 + keyValue[j] - '0';
							}
						}
						g_pCanManager1->SetParameter_PowerSupply(32 + i, v[0], v[1], v[2], v[3], v[4], 500, NULL);
						g_pCanManager2->SetParameter_PowerSupply(32 + i, v[0], v[1], v[2], v[3], v[4], 500, "2");
					}
					index = index >> 1;
				}
			}
		}
		break;
	case 2105:			//电源控制指令
		{
			g_Log.WriteLog(LOG_INF, "个性指令 电源控制指令");	//程序操作日志
			uchar tmp[1024];
			memcpy(&tmp, pData, len);
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13]+1;
			//pData[15] = 0;  //ack0
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //ack=0 返回下发指令

			//DWORD index = (DWORD)pData[16]<<24 +  (DWORD)pData[17]<<16 + (DWORD)pData[18]<<8 + (DWORD)pData[19];
			//int index = 0; 
			DWORD index = (((DWORD)pData[19])<<24) +  (((DWORD)pData[18])<<16) + (((DWORD)pData[17])<<8) + ((DWORD)pData[16]);
			//memcpy(&index, &tmp[16], 4);
			int dx = pData[20];
			if(dx == 1)  //输出
			{
				for(int i = 1; i <= 9; i ++)
				{
					if(index & 1)
					{
						g_pCanManager1->Output_PowerSupply(32 + i, 500, NULL);
					}
					index = index >> 1;
				}
				//index = index >> 7;
				for(int i = 1; i <= 9; i ++)
				{
					if(index & 1)
					{
						g_pCanManager2->Output_PowerSupply(32 + i, 500, "2");
					}
					index = index >> 1;
				}
			}
			if(dx == 2)	//停止输出
			{
				for(int i = 1; i <= 9; i ++)
				{
					if(index & 1)
					{
						g_pCanManager1->OutputStop_PowerSupply(32 + i, 500, NULL);
					}
					index = index >> 1;
				}
				//index = index >> 7;
				for(int i = 1; i <= 9; i ++)
				{
					if(index & 1)
					{
						g_pCanManager2->OutputStop_PowerSupply(32 + i, 500, "2");
					}
					index = index >> 1;
				}
			}
			if(dx == 3)	//复位
			{
				for(int i = 1; i <= 9; i ++)
				{
					if(index & 1)
					{
						g_pCanManager1->Reset_PowerSupply(32 + i, 500, NULL);
					}
					index = index >> 1;
				}
				//index = index >> 7;
				for(int i = 1; i <= 9; i ++)
				{
					if(index & 1)
					{
						g_pCanManager2->Reset_PowerSupply(32 + i, 500, "2");
					}
					index = index >> 1;
				}
			}
		}
		break;
	case 2109:			//电池控制指令
		{
			g_Log.WriteLog(LOG_INF, "个性指令 电池控制指令");	//程序操作日志
			g_chargeEnableFlag = -1;
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13]+1;
			//pData[15] = 0;  //ack确认
			pData[12] = pData[12] + 1;
			pData[14] = 0; 
			udpHelper.Send(pData, len); //ack=2 返回下发指令

			//WORD index = (WORD)pData[16]*256 + pData[17];
			WORD index = (WORD)pData[17]*256 + pData[16];
			m_index = index;
			int dx = pData[18];
			if(dx == 1)  //开始充电
			{
				for (int i = 1; i < 11; i++)
				{
					if (index&1)
					{
						DWORD tmpID = 0X1806FE00 + DWORD(i);
						g_pCanManager4->ChargeControl(tmpID,0,80,0,0,NULL);
						Sleep(20);
						tmpID = 0X1807FE00 + DWORD(i);
						g_pCanManager4->BatteryChargeParameter_Supply(tmpID, 01, 02, 127,4000, 800, 01, NULL);
					}
					index = index >> 1;
				}
			}
			if(dx == 2) //停止充电
			{
				for (int i = 1; i < 11; i++)
				{
					if (index&1)
					{
						DWORD tmpID = 0X1806FE00 + DWORD(i);
						g_pCanManager4->StopChargeControl(tmpID, 0, 0, 0, 0, NULL);
					}
					index = index >> 1;
				}
			}
			if(dx == 3)  //开始放电
			{
				
				WORD tmp = index;
				for (int i = 1; i < 11; i++)
				{
					if (index&1)
					{
						DWORD tmpID = 0X1806FE00 + DWORD(i);
						g_pCanManager4->DisChargeControl(tmpID, 16, NULL);
					}
					index = index >> 1;
				}
				g_batoutput = 1; //进入放电流程
				/*Sleep(1500);
				for (int i = 1; i < 11; i++)
				{
				if (tmp&1)
				{
				if(g_yuchongOK[i] == TRUE)
				{
				DWORD tmpID = 0X1806FE00 + DWORD(i);
				g_pCanManager4->DisChargeControl(tmpID, 80, NULL);
				Sleep(100);
				g_pCanManager4->DisChargeControl(tmpID, 64, NULL);
				}
				}
				tmp = tmp >> 1;
				}*/
			}
			if(dx == 4)  //停止放电
			{
				g_batoutput = 0; 
				g_hotBoot = 0;
				for (int i = 1; i < 11; i++)
				{
					if (index&1)
					{
						DWORD tmpID = 0X1806FE00 + DWORD(i);
						g_pCanManager4->StopDisChargeControl(tmpID, NULL);	
					}
					index = index >> 1;
					g_yuchongOK[i] = FALSE;
				}
			}
		}
		break;
	case 2113:			//允许电池充电控制指令
		{
			g_Log.WriteLog(LOG_INF, "个性指令 允许电池充电控制指令");	//程序操作日志
			uchar tmp1 = pData[6];
			pData[6] = pData[7];
			pData[7] = tmp1;
			//pData[13] = pData[13] + 1;
			//pData[15] = 0;  //ack确认
			pData[12] = pData[12] + 1;
			pData[14] = 0;
			udpHelper.Send(pData, len); //ack=0 返回下发指令
			BYTE bytePara[2];
			bytePara[0] = pData[16];
			
			if(bytePara[0] == 0)//不允许充电
			{
				g_chargeEnableFlag = 0;
			}
			if(bytePara[0] == 1)//允许小功率充电
			{
				g_chargeEnableFlag = 1;
			}
			if(bytePara[0] == 2)//允许大功率充电
			{
					
				g_chargeEnableFlag = 2;
			}
			
		}
		break;
	default:
		break;
	}
}

//解析接收到的UDP包
//作战任务下发指令
int CMornSDKDlg::ParseUdpTaskDown(uchar* pdata)
{
	CString aa(pdata);
	//3、提取试验编号；
	szCurTaskNo = "";
	for (int i=0; i < 12; i++)
	{
		szCurTaskNo	+= CString((char)pdata[16+i]);
	}
	g_CfgTask.SetKey("nowTaskNum", "nowNum", szCurTaskNo);//更新配置文件当前任务编号
	//4、以试验编号及当前时间生成报表文件；
	//int ret = WriteFile(g_strExePath + "Tasks\\" + szCurTaskNo + ".txt","");
	//if(!ret)
	//{
	//	//g_taskDown = g_taskDown + 1;
	//	g_Log.WriteLog(LOG_INF, "以试验编号及当前时间生成报表文件 成功");
	//}
	//else
	//{
	//	g_Log.WriteLog(LOG_INF, "以试验编号及当前时间生成报表文件 失败");
	//	return -1;
	//}
	//5、提取出光时长作为计时依据,提取延时，下发给时序控制器
	//int times = pdata[31];
	float times = 0;
	memcpy(&times, &pdata[28], 4);
	//times = pdata[28];
	//float times = float( (((DWORD)pdata[28])<<24) + (((DWORD)pdata[29])<<16) + (((DWORD)pdata[30])<<8) + ((DWORD)pdata[31]) );
	BYTE xx[6] = {0xBB, 0x11, 0x00, 0x00, 0x11, 0xEE};	//时序控制器参数设置指令
	xx[2] = (BYTE)g_delay;
	xx[3] = (BYTE)times;
	xx[4] = 0x11^xx[2]^xx[3];
	udpAsxHelper.Send(xx, 6);		//时序控制器A设置参数
	udpBsxHelper.Send(xx, 6);		//时序控制器B设置参数
	//6、提取种子源工作模式，从预存表格中读取种子源的电流值，并且发设置指令给种子源；
	//int workType = pdata[32]*256 +pdata[33]; //比如workType提取出来为10。
	int workType = pdata[33]*256 +pdata[32]; //比如workType提取出来为10。
	//以10从配置文件中读取10=9,3,3,2
	//然后提取出str为9,3,3,2.
	CString str;
	str.Format("%d", workType);
	CString keyValue[2];
	 keyValue[0] = g_CfgTask.GetKeyValue( str ,"SeedSourceSetA"); 
	 keyValue[1] = g_CfgTask.GetKeyValue( str ,"SeedSourceSetB"); 
	int tmp = 0;
	vector<int> v;//用来存每个逗号之间的数字	
	for(int j = 0; j < 2; j++)
	{
		keyValue[j] += ',';	  //末尾添加，好判断
		for(int i = 0; i < keyValue[j].GetLength(); i++)
		{
			if(keyValue[j][i] == ',')
			{
				v.push_back(tmp);
				tmp = 0;
			}
			else
			{
				tmp = tmp*10 + keyValue[j][i] - '0';
			}
		}
	}
	for(int i = 0; i < v.size(); i++)
	{
		SeedSource_Parameter[i] = v[i];    //doc文件存储种子源A配置参数信息
		SeedSource_Parameter[4 + i] = v[i];//doc文件存储种子源B配置参数信息
	}
	if(!v.empty() && v[0] != 0)
	{
		g_ruoGuangValueA = v[0];  //如果提取成功的话，就把第一个值给这个变量，就是弱光的值.
		g_ruoGuangValueB = v[4];
	}
	//7、提取出光模式，从预存的表格中提取每个电源的输出电流值，并且发设置指令给每一台电源。
	int chuguangType = pdata[37];//出光模式
	if(chuguangType == 2)
	{
		g_pCanManager3->Output_CurrentSupply(16, g_ruoGuangValueA, 0, 0, 0, 500, NULL);
		g_pCanManager3->Output_CurrentSupply(16, g_ruoGuangValueB, 0, 0, 0, 500,NULL);
	}
	splitStrUdp(1004, 0, 0); //反馈
	return 0;
}

//强光控制指令
int CMornSDKDlg::ParseUdpQGCtrl(uchar* pdata)
{
	/*  I：ACK=1
		数据区内容：
		BYTE bytePara[2]；
		bytePara[0]：控制对象：
		1- A 路；
		2- B 路；
		3- 双路。
		bytePara[1]：控制动作：
		1- 输出
		2- 停止输出   */
	BYTE bytePara[2];
	bytePara[0] = pdata[16]; //控制对象
	bytePara[1] = pdata[17]; //控制动作
	g_qgCtrl = bytePara[0];
	if(bytePara[1] == 1)
	{
		isStartQg = 1; //开始出光了 文件开始写入
		if(bytePara[0] == 1) //A
		{
			if(!g_isQgCmd_Aoutput)
			{
				//BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //复位A时序控制器
				//g_pMainDlg->udpAsxHelper.Send(xx, 6);
				//Sleep(5);
				g_isQgCmd_Aoutput = TRUE;
				//控制IO卡,high 开
				USHORT chValue = g_pIoCard->OnReadback();
				chValue &= 0xfffe;
				g_pIoCard->WriteWord(0, chValue);
				chValue |= 1;
				g_pIoCard->WriteWord(0, chValue);
				splitStrUdp(1104, bytePara[0], 0);
				g_Log.WriteLog(LOG_INF, "强光控制指令 A输出");	//程序操作日志
			}
		}
		if(bytePara[0] == 2) //B
		{
			if(!g_isQgCmd_Boutput)
			{
				//BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //复位b时序控制器
				//g_pMainDlg->udpBsxHelper.Send(xx, 6);
				//Sleep(10);
				g_isQgCmd_Boutput = TRUE;
				//控制IO卡,high 开
				USHORT chValue = g_pIoCard->OnReadback();
				//chValue &= 0xfffd;
				chValue &= 0xfffc;
				g_pIoCard->WriteWord(0, chValue);
				//chValue |= 2;
				chValue |= 3;
				g_pIoCard->WriteWord(0, chValue);
				splitStrUdp(1104, bytePara[0], 0);
				g_Log.WriteLog(LOG_INF, "强光控制指令 B输出");	//程序操作日志
			}
		}
			
		if(bytePara[0] == 3) //双
		{
			if(!g_isQgCmd_Aoutput || !g_isQgCmd_Boutput)
			{
				//BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE}; //复位A时序控制器
				//g_pMainDlg->udpAsxHelper.Send(xx, 6);
				//Sleep(5);
				//g_pMainDlg->udpBsxHelper.Send(xx, 6);
				//Sleep(5);
				g_isQgCmd_Aoutput = TRUE;
				g_isQgCmd_Boutput = TRUE;
				//控制IO卡,high 开
				USHORT chValue = g_pIoCard->OnReadback();
				chValue &= 0xfffc;
				g_pIoCard->WriteWord(0, chValue);
				chValue |= 3;
				g_pIoCard->WriteWord(0, chValue);
				splitStrUdp(1104, bytePara[0], 0);
				g_Log.WriteLog(LOG_INF, "强光控制指令 A输出");	//程序操作日志
				g_Log.WriteLog(LOG_INF, "强光控制指令 B输出");	//程序操作日志
			}
		}
	}
	if(bytePara[1] == 2) //停止输出
	{
		//1、停止电源启动电平。时序控制器按照预定时序关闭电源；
		//这里还不知道怎么做，，，XXXXXXXXXXXXXXXXXXXXXXXX
		//2、由时序控制器发送电源已关闭信号给电源控制器；
		//这里还不知道怎么做，，，XXXXXXXXXXXXXXXXXXXXXXXX
		//3、电源控制器确认所有电源已关闭后，向种子源发送停止输出指令。

		
		isStartQg = 0; //停止出光 文件停止写入
		WriteUpStateLog();
		g_seedsourceA.clear(); //清空数据
		g_seedsourceB.clear();
		g_powersupplyA.clear();
		g_powersupplyB.clear();
		g_soc.clear();
		if(bytePara[0] == 1) //A
		{
			if(g_isQgCmd_Aoutput)
			{
				g_isQgCmd_Aoutput = FALSE;
				//控制IO卡,low 关
				USHORT chValue = g_pIoCard->OnReadback();
				chValue &= 0xFFFE;
				g_pIoCard->WriteWord(0, chValue);
				g_Log.WriteLog(LOG_INF, "强光控制指令 A停止输出");	//程序操作日志
			}
				
		}
		if(bytePara[0] == 2) //B
		{
			if(g_isQgCmd_Boutput)
			{
				g_isQgCmd_Boutput = FALSE;
				//控制IO卡,low 关
				USHORT chValue = g_pIoCard->OnReadback();
				//chValue &= 0xFFFD;
				chValue &= 0xFFFC;
				g_pIoCard->WriteWord(0, chValue);
				g_Log.WriteLog(LOG_INF, "强光控制指令 B停止输出");	//程序操作日志
			}
		}
		if(bytePara[0] == 3) //双
		{
			if(g_isQgCmd_Aoutput || g_isQgCmd_Boutput)
			{
				g_isQgCmd_Aoutput = FALSE;
				g_isQgCmd_Boutput = FALSE;
				//控制IO卡,low 关
				USHORT chValue = g_pIoCard->OnReadback();
				chValue &= 0xFFFC;
				g_pIoCard->WriteWord(0, chValue);
				g_Log.WriteLog(LOG_INF, "强光控制指令 A停止输出");	//程序操作日志
				g_Log.WriteLog(LOG_INF, "强光控制指令 B停止输出");	//程序操作日志
			}
				
		}
		splitStrUdp(1104, bytePara[0], 0);
		
	}
	return 0;
}

//数据上报指令
int CMornSDKDlg::ParseUdpDataUp(uchar* pdata)
{
	CString startNo = "";
	CString endNo = ""; 
	for (int i=0; i < 24; i++)
	{
		if(i < 12)
			startNo	+= CString((char)pdata[16+i]);
		else
			endNo += CString((char)pdata[16+i]);
	}
	long long start = _atoi64(startNo.Left(11));  //提取年月日然后+三位判断
	long long end = _atoi64(endNo.Left(11));	  //提取年月日然后+三位判断 

	CString filepath = g_strExePath + "Tasks\\";
    CString filename = _T("");
    CString fullname = _T("");
    CFileFind find;
    BOOL IsFind = find.FindFile(filepath + _T("/*.*"));
    while (IsFind)
	{
        IsFind = find.FindNextFile();
        if (find.IsDots())
		{
            continue;
		}
        else
        {
            filename = find.GetFileName();
            fullname = filepath + filename;
			CString tmpStr = filename.Left(11);  //提取年月日然后+三位判断
			long long tmpNo = _atoi64(tmpStr); 
			if(tmpNo <= end && tmpNo >= start)
			{
				BOOL m=CopyFile(fullname, "\\192.168.3.11\\试验数据\\电源分系统\\" + filename, FALSE); 
				//BOOL m=CopyFile(fullname, "C:\\Users\\YLL\\Desktop\\2323232\\" + filename, FALSE); 				
				if(!m) 
				{
					g_Log.WriteLog(LOG_ERR, "数据上报指令, 拷贝文件失败");
				}
			}
		}
	}
	//WORD index = (WORD)pdata[40]*256+(WORD)pdata[41];
	//int num = (startNo[6] - '0')*10 + startNo[7] - '0'; //把日期计算出来
	//for(int i = 0; i < 16; i++)
	//{
	//	int tmp = num + i;
	//	startNo.SetAt(6, '0' + tmp/10);   //新开始日期十位数
	//	startNo.SetAt(7, '0' + tmp%10);   //新开始日期个位数
	//	if(index&1)
	//	{
	//		//加入存在，就覆盖
	//		BOOL m=CopyFile(g_strExePath + "Tasks\\" + startNo+".txt", "C:\\Users\\yangleilei\\Desktop\\11111\\"+startNo+".txt", FALSE); 
	//		if(!m) 
	//		{
	//			g_Log.WriteLog(LOG_ERR, "数据上报指令, 拷贝文件失败");
	//			//splitStrUdp(1028, 1, 0);  //拷贝执行失败
	//		}
	//	}
	//	index  = index >> 1;
	//}
	splitStrUdp(1028, 0, 0);  //拷贝执行成功
	return 0;
}

//系统流程控制指令
int CMornSDKDlg::ParseUdpSysCtrl(uchar* pdata, int len)
{
	/*  BYTE bytePara[2]；
		bytePara[0]：动作：
		1- 热启动；
		2- 退出热启动；
		3- 转火；
		4- 本发试验结束；
		5- 撤收；
		6- 刹车；
		bytePara[1] = 0   */
	BYTE bytePara[2];
	bytePara[0] = pdata[16];  //动作
	//bytePara[1] = pdata[17];
	
	//热启动
	//1、如果现在在充电则首先停止充电
	//2、电池启动由上级系统在热启动时完成。（电池放电）
	if(bytePara[0] == 1)
	{
		for (int i = 1; i <= 10; i++)
		{
			DWORD tmpID = 0X1806FE00 + DWORD(i);
			if(g_pMainDlg->m_Page2.m_chargeFlag[i] == TRUE)
			{
				g_pCanManager4->StopChargeControl(tmpID, 0, 0, 0, 0, NULL);
			}
		}
		for (int i = 1; i <= 10; i++)
		{
			if(g_isbatDisChargeState[i] == TRUE)
				continue;
			DWORD tmpID = 0X1806FE00 + DWORD(i);
			if(g_pMainDlg->m_Page2.m_chargeFlag[i] == FALSE)
			{
				g_pCanManager4->DisChargeControl(tmpID, 16, NULL);
			}
		}
		g_Log.WriteLog(LOG_INF, "系统流程控制指令 热启动");	//程序操作日志
		g_hotBoot = 1;	//热启动标志位
	}
	//退出热启动 电池停止放电
	if(bytePara[0] == 2)
	{
		g_hotBoot = 0;	//清除热启动标志位
		g_batoutput = 0;
		for (int i = 1; i <= 10; i++)
		{
			DWORD tmpID = 0X1806FE00 + DWORD(i);
			g_pCanManager4->StopDisChargeControl(tmpID, NULL);
			g_yuchongOK[i] = FALSE;
		}
		splitStrUdp(1108, 2, 0);  //退出热启动 成功
		g_Log.WriteLog(LOG_INF, "系统流程控制指令 退出热启动");	//程序操作日志
	}
	//转火
	//1、UDP 包检查；
	//2、向 W 控反馈指令 1014
	if(bytePara[0] == 3)
	{
		//szCurTaskNo = "201905011300";
		szCurTaskNo = g_nowTaskNum;
		if (szCurTaskNo.GetLength() != 11)
		{
			g_Log.WriteLog(LOG_ERR, "当前任务编号不存在");
			return -1;
		}
		long long number = _atoi64(szCurTaskNo);
		number = number + 1;
		szCurTaskNo.Format("%I64u", number);
		int ret = WriteFile(g_strExePath + "Tasks\\" + szCurTaskNo + ".txt","");
		if(!ret)
		{
			splitStrUdp(1108,3, 0);	//上传udp包
			//g_Log.WriteLog(LOG_INF, "系统流程控制指令：转火,执行成功！");
		}
		else
		{
			splitStrUdp(1108,3, 1); //上传udp包
			//g_Log.WriteLog(LOG_ERR, "系统流程控制指令：转火,执行失败！");
		}
		g_Log.WriteLog(LOG_INF, "系统流程控制指令 转火");	//程序操作日志
	}
	//本发试验结束 电源分系统无动作。
	if(bytePara[0] == 4)
	{
		splitStrUdp(1108, 4, 0);
		g_Log.WriteLog(LOG_INF, "系统流程控制指令：本发试验结束 电源分系统无动作！");
	}	
	//撤收
	//1、发送撤收指令反馈。
	//2、关闭计算机。
	if(bytePara[0] == 5)
	{
		g_pIoCard->WriteWord(0, 0);//IO reset
		g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
		g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
		for(int i = 1; i < 11; i++)
		{
			DWORD tmpID = 0X1806FE00 + DWORD(i);
			g_pCanManager4->StopDisChargeControl(tmpID, NULL);
		}

		splitStrUdp(1108, 5, 0); 
		g_Log.WriteLog(LOG_INF, "系统流程控制指令：撤收！");

		//自动关机代码
		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		{
			MessageBox(_T("关机失败..."), _T("警告"), MB_OK);
			return -1;
		}
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;  
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, NULL);
		if (GetLastError() != ERROR_SUCCESS)
		{
			MessageBox(_T("关机失败..."), _T("警告"), MB_OK);
			return -1;
		}
		ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0); //重启只需要把EWX_SHUTDOWN改为EWX_REBOOT
	}
	//刹车
	//1、接收到刹车指令，执行停止出光指令的流程。
	//2、上级产生硬线刹车接到时序控制器，
	//时序控制器关闭电源后，
	//通过RS485 发送电源已关闭信号，
	//电源控制器关闭种子源；
	if(bytePara[0] == 6)
	{
		g_pIoCard->WriteWord(0, 0);
		g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
		g_pCanManager3->SendFrame_SeedSource_StopOutput(16, 500, NULL);
		splitStrUdp(1108, 6, 0);
		g_Log.WriteLog(LOG_INF, "系统流程控制指令 刹车");	//程序操作日志
	}
	return 0;
}

//响应Tab页面选择消息
void CMornSDKDlg::OnTcnSelchangeTabctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//把当前的页面隐藏起来
	m_pPages[m_iCurTab]->ShowWindow(SW_HIDE);
	//得到新的页面索引
	m_iCurTab = m_tabCtrl.GetCurSel();
	//把新的页面显示出来
	m_pPages[m_iCurTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}

//反馈回调函数（静态成员函数）
int CMornSDKDlg::FeedBack(CTask* pTask, void* pData)
{
	CString msg;
	//msg.Format("ID:%d, 数据：%s", pTask->m_CanMsgSend.id, (LPCTSTR)pData);
	//::MessageBox(NULL, msg, _T("测试"), MB_OK|MB_ICONINFORMATION);
	//g_Log.WriteLog(LOG_DBG,msg);

	CString ReceiveFrame;
	ReceiveFrame.Format("ID:%d, 数据：%s", pTask->m_CanMsgRecv.data, (LPCTSTR)pData);
	//g_Log.WriteLog(LOG_DBG,ReceiveFrame);

	//测试用，不测试注释
	

	//pTask->m_CanMsgRecv.data[0] = 0x10;
	//pTask->m_CanMsgRecv.data[1] = 0x0e;
	//pTask->m_CanMsgRecv.data[2] = 0x01;
	//pTask->m_CanMsgRecv.data[3] = 0xff;
	//pTask->m_CanMsgRecv.data[4] = 0xff;
	//pTask->m_CanMsgRecv.data[5] = 0xff;
	//pTask->m_CanMsgRecv.data[6] = 0x01;
	//pTask->m_CanMsgRecv.data[7] = 0x00;

	/*pTask->m_CanMsgRecv.data[0] = 0x10;
	pTask->m_CanMsgRecv.data[1] = 0x0e;
	pTask->m_CanMsgRecv.data[2] = 0x02;
	pTask->m_CanMsgRecv.data[3] = 0xff;
	pTask->m_CanMsgRecv.data[4] = 0xff;
	pTask->m_CanMsgRecv.data[5] = 0xff;
	pTask->m_CanMsgRecv.data[6] = 0x01;
	pTask->m_CanMsgRecv.data[7] = 0x00;*/

	//pTask->m_CanMsgRecv.data[0] = 0x11;
	//pTask->m_CanMsgRecv.data[1] = 0x0e;
	//pTask->m_CanMsgRecv.data[2] = 0x01;
	//pTask->m_CanMsgRecv.data[3] = 0xff;
	//pTask->m_CanMsgRecv.data[4] = 0xff;
	//pTask->m_CanMsgRecv.data[5] = 0xff;
	//pTask->m_CanMsgRecv.data[6] = 0x01;
	//pTask->m_CanMsgRecv.data[7] = 0x00;

	//pTask->m_CanMsgRecv.data[0] = 0x11;
	//pTask->m_CanMsgRecv.data[1] = 0x0e;
	//pTask->m_CanMsgRecv.data[2] = 0x02;
	//pTask->m_CanMsgRecv.data[3] = 0xff;
	//pTask->m_CanMsgRecv.data[4] = 0xff;
	//pTask->m_CanMsgRecv.data[5] = 0xff;
	//pTask->m_CanMsgRecv.data[6] = 0x01;
	//pTask->m_CanMsgRecv.data[7] = 0x00;

	/*pTask->m_szCanPort = (char*)(NULL);
	pTask->m_CanMsgRecv.data[0] = 0x29;
	pTask->m_CanMsgRecv.data[1] = 0x08;
	pTask->m_CanMsgRecv.data[2] = 0x01;
	pTask->m_CanMsgRecv.data[3] = 0x01;
	pTask->m_CanMsgRecv.data[4] = 0x00;
	pTask->m_CanMsgRecv.data[5] = 0x01;
	pTask->m_CanMsgRecv.data[6] = 0x00;
	pTask->m_CanMsgRecv.data[7] = 0x00;*/

	/*pTask->m_szCanPort = "2";
	pTask->m_CanMsgRecv.data[0] = 0x01;
	pTask->m_CanMsgRecv.data[1] = 0xA1;
	pTask->m_CanMsgRecv.data[2] = 0x01;
	pTask->m_CanMsgRecv.data[3] = 0x02;
	pTask->m_CanMsgRecv.data[4] = 0x03;
	pTask->m_CanMsgRecv.data[5] = 0x04;
	pTask->m_CanMsgRecv.data[6] = 0x05;
	pTask->m_CanMsgRecv.data[7] = 0x00;*/

	//BYTE	data[8];
	//memcpy(data, pTask->m_CanMsgRecv.data, 8);
	BYTE *data = pTask->m_CanMsgRecv.data;
	CString ReceiveData;
	ReceiveData.Format("%02X %02X %02X %02X %02X %02X %02X %02X",data[0], data[1]\
		,data[2], data[3],data[4], data[5],data[6], data[7] );                                                                                
	
	//if(pTask->m_CanMsgSend.data[1] == 0x05)  //强光A
	//{
	//	if(pTask->m_CanMsgRecv.data[3] != 0x11)
	//	{
	//		if(g_qgCtrl.index == 1 && g_qgCtrl.dongzuo == 1)
	//			g_pMainDlg->splitStrUdp(1104, 1, 1);  //A output fail
	//		if(g_qgCtrl.index == 3 && g_qgCtrl.dongzuo == 1)
	//			g_pMainDlg->splitStrUdp(1104, 3, 1);  //AB output fail

	//	}
	//	if(pTask->m_CanMsgRecv.data[3] != 0x22)
	//	{
	//		if(g_qgCtrl.index == 1 && g_qgCtrl.dongzuo == 2)
	//			g_pMainDlg->splitStrUdp(1104, 1, 1);  //A output fail
	//		if(g_qgCtrl.index == 3 && g_qgCtrl.dongzuo == 2)
	//			g_pMainDlg->splitStrUdp(1104, 3, 1);  //AB output fail
	//	}
	//}
	//if(pTask->m_CanMsgSend.data[0] == 0x11 &&
	//	pTask->m_CanMsgSend.data[1] == 0x05)  //强光A
	//{
	//	if(pTask->m_CanMsgRecv.data[3] != 0x11)
	//	{
	//		if(g_qgCtrl.index == 2 && g_qgCtrl.dongzuo == 1)
	//			g_pMainDlg->splitStrUdp(1104, 2, 1);  //A output fail
	//		if(g_qgCtrl.index == 3 && g_qgCtrl.dongzuo == 1)
	//			g_pMainDlg->splitStrUdp(1104, 3, 1);  //AB output fail
	//	}
	//	if(pTask->m_CanMsgRecv.data[3] != 0x22)
	//	{
	//		if(g_qgCtrl.index == 2 && g_qgCtrl.dongzuo == 2)
	//			g_pMainDlg->splitStrUdp(1104, 2, 1);  //A output fail
	//		if(g_qgCtrl.index == 3 && g_qgCtrl.dongzuo == 2)
	//			g_pMainDlg->splitStrUdp(1104, 3, 1);  //AB output fail
	//	}
	//}
	
	//种子源选项
	switch(pTask->m_CanMsgRecv.data[0])
	{
		case 0X10:
			switch(pTask->m_CanMsgRecv.data[1])
			{
				case 0X04://输出电流设置反馈
					g_Log.WriteLog(LOG_DBG,ReceiveFrame);
					if( pTask->m_CanMsgRecv.data[2] == 0X00	&&
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] &&
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x10, 0x04);
						int i = 1;
						CString zhendang, current1,current2,current3;
						zhendang.Format("%.1f", (float)data[3]/10);
						current1.Format("%.1f", (float)data[4]/10);
						current2.Format("%.1f", (float)data[5]/10);
						current3.Format("%.1f", (float)data[6]/10);
						g_pMainDlg->m_Page1.m_pCurrentSet.SetItemText(i, 0, zhendang);
						g_pMainDlg->m_Page1.m_pCurrentSet.SetItemText(i, 1, current1);
						g_pMainDlg->m_Page1.m_pCurrentSet.SetItemText(i, 2, current2);
						g_pMainDlg->m_Page1.m_pCurrentSet.SetItemText(i, 3, current3);
						g_pMainDlg->m_Page1.m_pCurrentSet.SetItemFgColour(i, 0, RGB(0, 255, 0));
						g_pMainDlg->m_Page1.m_pCurrentSet.SetItemFgColour(i, 1, RGB(0, 255, 0));
						g_pMainDlg->m_Page1.m_pCurrentSet.SetItemFgColour(i, 2, RGB(0, 255, 0));
						g_pMainDlg->m_Page1.m_pCurrentSet.SetItemFgColour(i, 3, RGB(0, 255, 0));

						((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_current_receive_EDIT)))->SetWindowText(ReceiveData);
						if (g_taskDown > 0)
						{
							g_taskDown++;
						}
					}
					break;
				case 0X06://输出控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	&&
						pTask->m_CanMsgRecv.data[3] == 0X11 && //电源输出
						pTask->m_CanMsgRecv.data[4] == 0X00 &&
						pTask->m_CanMsgRecv.data[5] == 0X00 &&
						pTask->m_CanMsgRecv.data[6] == 0X00 &&
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x10, 0x06);
						((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_current_receive_EDIT)))->SetWindowText(ReceiveData);
					}

					if( pTask->m_CanMsgRecv.data[2] == 0X00	&&
						pTask->m_CanMsgRecv.data[3] == 0X22 && //电源停止输出
						pTask->m_CanMsgRecv.data[4] == 0X00 &&
						pTask->m_CanMsgRecv.data[5] == 0X00 &&
						pTask->m_CanMsgRecv.data[6] == 0X00 &&
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x10, 0x07);
						((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_current_receive_EDIT)))->SetWindowText(ReceiveData);
					}
					break;
				case 0X08://复位反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X11	&&
						pTask->m_CanMsgRecv.data[3] == 0X00 && //reset
						pTask->m_CanMsgRecv.data[4] == 0X00 &&
						pTask->m_CanMsgRecv.data[5] == 0X00 &&
						pTask->m_CanMsgRecv.data[6] == 0X00 &&
						pTask->m_CanMsgRecv.data[7] == 0X00)

					{
						::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x10, 0x08);
						((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_current_receive_EDIT)))->SetWindowText(ReceiveData);
					}
					break;
				case 0X0E:
					//激光器状态反馈。。。。。1
					if( pTask->m_CanMsgRecv.data[2] == 0X01	&&//数据包1
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						//g_stateToUp[78] = 0;
						//g_stateToUp[79] = pTask->m_CanMsgRecv.data[3]; //pd1
						//g_stateToUp[80] = 0;
						//g_stateToUp[81] = pTask->m_CanMsgRecv.data[4]; //pd2
						//g_stateToUp[82] = 0;
						//g_stateToUp[83] = pTask->m_CanMsgRecv.data[5];  //pd3
						g_stateToUp[78] = pTask->m_CanMsgRecv.data[3];
						g_stateToUp[79] = 0;							//pd1
						g_stateToUp[80] = pTask->m_CanMsgRecv.data[4];
						g_stateToUp[81] = 0;							//pd2
						g_stateToUp[82] = pTask->m_CanMsgRecv.data[5];
						g_stateToUp[83] = 0;							//pd3
						if(pTask->m_CanMsgRecv.data[6] == 0)	g_stateToUp[87] = 1;
						else if(pTask->m_CanMsgRecv.data[6] == 1)	g_stateToUp[87] = 2;
						else g_stateToUp[87] = 3;
						g_stateToUp[88] = 1;

						//((CButtonST*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LED_resetCURRENT)))->SetIcon(IDI_ICON_GREEN);
						//Pd1
						float s1 = ((float)pTask->m_CanMsgRecv.data[3])/100;
						CString str1;
						str1.Format("%.2f",s1);
						g_pMainDlg->m_Page1.m_pLightState.SetItemText(1,0, str1);
						//g_localDataLog[16] = "A路Pd1: " + str1; //本地数据 A路 Pd1
						g_localDataLog[16].SetString("A路Pd1: " + str1);//本地数据 A路 Pd1
						//Pd2
						float s2 = ((float)pTask->m_CanMsgRecv.data[4])/100;
						CString str2;
						str2.Format("%.2f",s2);
						g_pMainDlg->m_Page1.m_pLightState.SetItemText(1,1, str2);
						//g_localDataLog[17] = "A路Pd2: " + str2; //本地数据 A路 Pd2
						g_localDataLog[17].SetString("A路Pd2: " + str2);//本地数据 A路 Pd2
						//Pd3
						float s3 = ((float)pTask->m_CanMsgRecv.data[5])/100;
						CString str3;
						str3.Format("%.2f",s3);
						g_pMainDlg->m_Page1.m_pLightState.SetItemText(1,2, str3);
						//g_localDataLog[18] = "A路Pd3: " + str3; //本地数据 A路 Pd3
						g_localDataLog[18].SetString("A路Pd3: " + str3);//本地数据 A路 Pd3
						//光源状态
						CStringArray state;
						state.Add(_T("空闲状态"));
						state.Add(_T("出光正常"));
						state.Add(_T("指示光正常"));
						state.Add(_T("Pd1偏低"));
						state.Add(_T("Pd1偏高"));
						state.Add(_T("Pd2偏低"));
						state.Add(_T("Pd2偏高"));
						state.Add(_T("t1偏低"));
						state.Add(_T("t1偏高"));
						state.Add(_T("t2偏低"));
						state.Add(_T("t2偏高"));
						state.Add(_T("t3偏低"));
						state.Add(_T("t3偏高"));
						state.Add(_T("湿度偏高"));
						state.Add(_T("流量异常"));
						state.Add(_T("通信异常"));
						state.Add(_T("Pd3偏低"));
						state.Add(_T("Pd3偏高"));

						float s4 = ((float)pTask->m_CanMsgRecv.data[6])/100;
						CString str4;
						str4.Format("%.2f",s4);
						g_pMainDlg->m_Page1.m_pLightState.SetItemText(1,3, state[pTask->m_CanMsgRecv.data[6]]);
						//g_localDataLog[19] = "A路光源状态: " + state[pTask->m_CanMsgRecv.data[6]]; //本地数据 A路 光源状态
						g_localDataLog[19].SetString("A路光源状态: " + state[pTask->m_CanMsgRecv.data[6]]);//本地数据 A路 光源状态
					}

					if( pTask->m_CanMsgRecv.data[2] == 0X02	&&//数据包2
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						float s1 = ((float)pTask->m_CanMsgRecv.data[3])/100;
						CString str1;
						str1.Format("%.2f",s1);
						g_pMainDlg->m_Page1.m_pLightState.SetItemText(1,4, str1);
						//g_localDataLog[20] = "A路T1: " + str1; //本地数据 A路 T1
						g_localDataLog[20].SetString("A路T1: " + str1); //本地数据 A路 T1

						float s2 = ((float)pTask->m_CanMsgRecv.data[4])/100;
						CString str2;
						str2.Format("%.2f",s2);
						g_pMainDlg->m_Page1.m_pLightState.SetItemText(1,5, str2);
						//g_localDataLog[21] = "A路T2: " + str2; //本地数据 A路 T2
						g_localDataLog[21].SetString("A路T2: " + str2); //本地数据 A路 T2

						float s3 = ((float)pTask->m_CanMsgRecv.data[5])/100;
						CString str3;
						str3.Format("%.2f",s3);
						g_pMainDlg->m_Page1.m_pLightState.SetItemText(1,6, str3);
						//g_localDataLog[22] = "A路T3: " + str3; //本地数据 A路 T3
						g_localDataLog[22].SetString("A路T3: " + str3); //本地数据 A路 T3

						float s4 = ((float)pTask->m_CanMsgRecv.data[6])/100;
						CString str4;
						str4.Format("%.2f",s4);
						g_pMainDlg->m_Page1.m_pLightState.SetItemText(1,7, str4);
						//printf("激光器状态反馈数据包2");
						//g_localDataLog[23] = "A路温度: " + str4; //本地数据 A路 温度
						g_localDataLog[23].SetString("A路温度: " + str4); //本地数据 A路 温度

					}
					break;
				case 0X10:
					//电源状态反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == 0X01	&&//数据包1
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						//g_stateToUp[72] = 0;                           //A种子源震荡级电流
						//g_stateToUp[73] = pTask->m_CanMsgRecv.data[3]/10;
						//g_stateToUp[74] = 0;						   //A种子源放大级电流
						//g_stateToUp[75] = pTask->m_CanMsgRecv.data[5]/10;
						g_stateToUp[72] = pTask->m_CanMsgRecv.data[3]; //A种子源震荡级电流
						g_stateToUp[73] = 0;
						g_stateToUp[74] = pTask->m_CanMsgRecv.data[5];	 //A种子源放大级1电流
						g_stateToUp[75] = 0;
						
						CStringArray state;
						state.Add(_T("空闲状态"));
						state.Add(_T("外部刹车"));
						state.Add(_T("超温"));
						state.Add(_T("输出欠流"));
						state.Add(_T("输出过流"));
						state.Add(_T("输出过压"));
						state.Add(_T("输入异常"));
						state.Add(_T("正常输出"));

						CString str1;
						float s1 = ((float)pTask->m_CanMsgRecv.data[3])/10;  //震荡级电流
						str1.Format("%.1f",s1);
						g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(1,0, str1);

						//g_localDataLog[0] = "A震荡级电流" + str1; //本地数据 震荡级电流
						g_localDataLog[0].SetString("A震荡级电流" + str1); //本地数据 震荡级电流

						int tmp = 0;
						if(pTask->m_CanMsgRecv.data[4] == 0x00)	tmp = 0;
						if(pTask->m_CanMsgRecv.data[4] == 0x02)	tmp = 1;
						if(pTask->m_CanMsgRecv.data[4] == 0x04)	tmp = 2;
						if(pTask->m_CanMsgRecv.data[4] == 0x08)	tmp = 3;
						if(pTask->m_CanMsgRecv.data[4] == 0x10)	tmp = 4;
						if(pTask->m_CanMsgRecv.data[4] == 0x20)	tmp = 5;
						if(pTask->m_CanMsgRecv.data[4] == 0x40)	tmp = 6;
						if(pTask->m_CanMsgRecv.data[4] == 0x80)	tmp = 7;
						g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(1,1, state[tmp]);  //震荡级状态

						//g_localDataLog[1] = "A震荡级状态: " + state[tmp]; //本地数据 震荡级状态
						g_localDataLog[1].SetString("A震荡级状态: " + state[tmp]); //本地数据 震荡级状态

						if(tmp ==0)		g_stateToUp[86] = 1;
						else if(tmp ==7)	g_stateToUp[86] = 2;
						else	g_stateToUp[86] = 3;


						float s3 = ((float)pTask->m_CanMsgRecv.data[5])/10;
						str1.Format("%.1f",s3);
						g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(1,2, str1);   //放大级1电流
						
						//g_localDataLog[2] = "A放大级1电流: " + str1; //本地数据 放大级1电流
						g_localDataLog[2].SetString("A放大级1电流: " + str1); //本地数据 放大级1电流
						
						if(pTask->m_CanMsgRecv.data[6] == 0x00)	tmp = 0;
						if(pTask->m_CanMsgRecv.data[6] == 0x02)	tmp = 1;
						if(pTask->m_CanMsgRecv.data[6] == 0x04)	tmp = 2;
						if(pTask->m_CanMsgRecv.data[6] == 0x08)	tmp = 3;
						if(pTask->m_CanMsgRecv.data[6] == 0x10)	tmp = 4;
						if(pTask->m_CanMsgRecv.data[6] == 0x20)	tmp = 5;
						if(pTask->m_CanMsgRecv.data[6] == 0x40)	tmp = 6;
						if(pTask->m_CanMsgRecv.data[6] == 0x80)	tmp = 7;
						g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(1,3, state[tmp]);  //放大级1状态

						//g_localDataLog[3] = "A放大级1状态: " + state[tmp]; //本地数据 放大级1状态
						g_localDataLog[3].SetString("A放大级1状态: " + state[tmp]); //本地数据 放大级1状态

						((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_EDIT_StatusReceiveA)))->SetWindowText(ReceiveData);

						
					}
					if( pTask->m_CanMsgRecv.data[2] == 0X02	&&//数据包2
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						//g_stateToUp[76] = 0;      
						//g_stateToUp[77] = pTask->m_CanMsgRecv.data[3]/10;  //放大级2电流
						//g_stateToUp[220] = 0;      
						//g_stateToUp[221] = pTask->m_CanMsgRecv.data[3]/10;  //放大级2电流
						g_stateToUp[76] = pTask->m_CanMsgRecv.data[3];      
						g_stateToUp[77] = 0;                                //放大级电流
						g_stateToUp[89] = pTask->m_CanMsgRecv.data[5];    //放大级3电流

						CStringArray state;
						state.Add(_T("空闲状态"));
						state.Add(_T("外部刹车"));
						state.Add(_T("超温"));
						state.Add(_T("输出欠流"));
						state.Add(_T("输出过流"));
						state.Add(_T("输出过压"));
						state.Add(_T("输入异常"));
						state.Add(_T("正常输出"));

						CString str1;
						float s1 = ((float)pTask->m_CanMsgRecv.data[3])/10;  //放大级2电流
						str1.Format("%.1f",s1);
						g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(1,4, str1);  

						//g_localDataLog[4] = "A放大级2电流: " + str1; //本地数据 放大级2电流
						g_localDataLog[4].SetString("A放大级2电流: " + str1); //本地数据 放大级2电流

						int tmp = 0;
						if(pTask->m_CanMsgRecv.data[4] == 0x00)	tmp = 0;
						if(pTask->m_CanMsgRecv.data[4] == 0x02)	tmp = 1;
						if(pTask->m_CanMsgRecv.data[4] == 0x04)	tmp = 2;
						if(pTask->m_CanMsgRecv.data[4] == 0x08)	tmp = 3;
						if(pTask->m_CanMsgRecv.data[4] == 0x10)	tmp = 4;
						if(pTask->m_CanMsgRecv.data[4] == 0x20)	tmp = 5;
						if(pTask->m_CanMsgRecv.data[4] == 0x40)	tmp = 6;
						if(pTask->m_CanMsgRecv.data[4] == 0x80)	tmp = 7;
						g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(1,5, state[tmp]); //放大级2状态

						//g_localDataLog[5] = "A放大级2状态: " + state[tmp]; //本地数据 放大级2状态
						g_localDataLog[5].SetString("A放大级2状态: " + state[tmp]); //本地数据 放大级2状态

						float s3 = ((float)pTask->m_CanMsgRecv.data[5])/10;
						str1.Format("%.1f",s3);
						g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(1,6, str1);  //放大级3电流

						//g_localDataLog[6] = "A放大级3电流: " + str1; //本地数据 放大级3电流
						g_localDataLog[6].SetString("A放大级3电流: " + str1); //本地数据 放大级3电流

						if(pTask->m_CanMsgRecv.data[6] == 0x00)	tmp = 0;
						if(pTask->m_CanMsgRecv.data[6] == 0x02)	tmp = 1;
						if(pTask->m_CanMsgRecv.data[6] == 0x04)	tmp = 2;
						if(pTask->m_CanMsgRecv.data[6] == 0x08)	tmp = 3;
						if(pTask->m_CanMsgRecv.data[6] == 0x10)	tmp = 4;
						if(pTask->m_CanMsgRecv.data[6] == 0x20)	tmp = 5;
						if(pTask->m_CanMsgRecv.data[6] == 0x40)	tmp = 6;
						if(pTask->m_CanMsgRecv.data[6] == 0x80)	tmp = 7;
						g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(1,7, state[tmp]); //放大级3状态

						//g_localDataLog[7] = "A放大级3状态: " + state[tmp]; //本地数据 放大级3状态
						g_localDataLog[7].SetString("A放大级3状态: " + state[tmp]); //本地数据 放大级3状态

						((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_EDIT_StatusReceiveA2)))->SetWindowText(ReceiveData);
					}
					break;
				default:
					break;
			}
		break;
		case 0X11:
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0X04://输出电流设置反馈
				if( pTask->m_CanMsgRecv.data[2] == 0X00	&&
					pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] &&
					pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&
					pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&
					pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&
					pTask->m_CanMsgRecv.data[7] == 0X00)
				{
					::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x11, 0x04);
					int i = 2; //第2列
					CString zhendang, current1,current2,current3;
					zhendang.Format("%.1f", (float)data[3]/10);
					current1.Format("%.1f", (float)data[4]/10);
					current2.Format("%.1f", (float)data[5]/10);
					current3.Format("%.1f", (float)data[6]/10);
					g_pMainDlg->m_Page1.m_pCurrentSet.SetItemText(i, 0, zhendang);
					g_pMainDlg->m_Page1.m_pCurrentSet.SetItemText(i, 1, current1);
					g_pMainDlg->m_Page1.m_pCurrentSet.SetItemText(i, 2, current2);
					g_pMainDlg->m_Page1.m_pCurrentSet.SetItemText(i, 3, current3);
					g_pMainDlg->m_Page1.m_pCurrentSet.SetItemFgColour(i, 0, RGB(0, 255, 0));
					g_pMainDlg->m_Page1.m_pCurrentSet.SetItemFgColour(i, 1, RGB(0, 255, 0));
					g_pMainDlg->m_Page1.m_pCurrentSet.SetItemFgColour(i, 2, RGB(0, 255, 0));
					g_pMainDlg->m_Page1.m_pCurrentSet.SetItemFgColour(i, 3, RGB(0, 255, 0));
					((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_current_receive_EDIT2)))->SetWindowText(ReceiveData);
					if (g_taskDown > 0)
					{
						g_taskDown++;
					}
				}
				break;
			case 0X06://输出控制反馈
				if( pTask->m_CanMsgRecv.data[2] == 0X00	&&
					pTask->m_CanMsgRecv.data[3] == 0X11 && //电源输出
					pTask->m_CanMsgRecv.data[4] == 0X00 &&
					pTask->m_CanMsgRecv.data[5] == 0X00 &&
					pTask->m_CanMsgRecv.data[6] == 0X00 &&
					pTask->m_CanMsgRecv.data[7] == 0X00)
				{
					::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x11, 0x06);
					((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_current_receive_EDIT2)))->SetWindowText(ReceiveData);
				}
				if( pTask->m_CanMsgRecv.data[2] == 0X00	&&
					pTask->m_CanMsgRecv.data[3] == 0X22 && //电源停止输出
					pTask->m_CanMsgRecv.data[4] == 0X00 &&
					pTask->m_CanMsgRecv.data[5] == 0X00 &&
					pTask->m_CanMsgRecv.data[6] == 0X00 &&
					pTask->m_CanMsgRecv.data[7] == 0X00)
				{
					::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x11, 0x07);
					((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_current_receive_EDIT2)))->SetWindowText(ReceiveData);
				}
				break;
			case 0X08:
				if( pTask->m_CanMsgRecv.data[2] == 0X11	&&
					pTask->m_CanMsgRecv.data[3] == 0X00 && //reset
					pTask->m_CanMsgRecv.data[4] == 0X00 &&
					pTask->m_CanMsgRecv.data[5] == 0X00 &&
					pTask->m_CanMsgRecv.data[6] == 0X00 &&
					pTask->m_CanMsgRecv.data[7] == 0X00)
				{
					::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x11, 0x08);
					((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_current_receive_EDIT2)))->SetWindowText(ReceiveData);
				}
				break;
			case 0X0E:
				//激光器状态反馈。。。。。2
				if( pTask->m_CanMsgRecv.data[2] == 0X01	&&//数据包1
					pTask->m_CanMsgRecv.data[7] == 0X00)
				{
					//g_stateToUp[222] = 0;
					//g_stateToUp[223] = pTask->m_CanMsgRecv.data[3]; //pd1
					//g_stateToUp[224] = 0;
					//g_stateToUp[225] = pTask->m_CanMsgRecv.data[4];  //pd2
					//g_stateToUp[226] = 0;
					//g_stateToUp[227] = pTask->m_CanMsgRecv.data[5]; //pd3
					g_stateToUp[222] = pTask->m_CanMsgRecv.data[3];
					g_stateToUp[223] = 0;								//pd1
					g_stateToUp[224] = pTask->m_CanMsgRecv.data[4];
					g_stateToUp[225] = 0;								//pd2
					g_stateToUp[226] = pTask->m_CanMsgRecv.data[5]; 
					g_stateToUp[227] = 0;								//pd3
					if(pTask->m_CanMsgRecv.data[6] == 0)	g_stateToUp[231] = 1; //光源状态
					else if(pTask->m_CanMsgRecv.data[6] == 1)	g_stateToUp[231] = 2;
					else g_stateToUp[231] = 3;
					g_stateToUp[232] = 1;
					//Pd1
					float s1 = ((float)pTask->m_CanMsgRecv.data[3])/100;
					CString str1;
					str1.Format("%.2f",s1);
					g_pMainDlg->m_Page1.m_pLightState.SetItemText(2,0, str1);
					//g_localDataLog[24] = "B路Pd1: " + str1; //本地数据 B路 Pd1
					g_localDataLog[24].SetString("B路Pd1: " + str1); //本地数据 B路 Pd1
					//Pd2
					float s2 = ((float)pTask->m_CanMsgRecv.data[4])/100;
					CString str2;
					str2.Format("%.2f",s2);
					g_pMainDlg->m_Page1.m_pLightState.SetItemText(2,1, str2);
					//g_localDataLog[25] = "B路Pd2: " + str2; //本地数据 B路 Pd2
					g_localDataLog[25].SetString("B路Pd2: " + str2); //本地数据 B路 Pd2

					//Pd3
					float s3 = ((float)pTask->m_CanMsgRecv.data[5])/100;
					CString str3;
					str3.Format("%.2f",s3);
					g_pMainDlg->m_Page1.m_pLightState.SetItemText(2,2, str3);
					//g_localDataLog[26] = "B路Pd3: " + str3; //本地数据 B路 Pd3
					g_localDataLog[26].SetString("B路Pd3: " + str3); //本地数据 B路 Pd3
					//光源状态
					CStringArray state;
					state.Add(_T("空闲状态"));
					state.Add(_T("出光正常"));
					state.Add(_T("指示光正常"));
					state.Add(_T("Pd1偏低"));
					state.Add(_T("Pd1偏高"));
					state.Add(_T("Pd2偏低"));
					state.Add(_T("Pd2偏高"));
					state.Add(_T("t1偏低"));
					state.Add(_T("t1偏高"));
					state.Add(_T("t2偏低"));
					state.Add(_T("t2偏高"));
					state.Add(_T("t3偏低"));
					state.Add(_T("t3偏高"));
					state.Add(_T("湿度偏高"));
					state.Add(_T("流量异常"));
					state.Add(_T("通信异常"));
					state.Add(_T("Pd3偏低"));
					state.Add(_T("Pd3偏高"));

					float s4 = ((float)pTask->m_CanMsgRecv.data[6])/100;
					CString str4;
					str4.Format("%.2f",s4);
					g_pMainDlg->m_Page1.m_pLightState.SetItemText(2,3, state[pTask->m_CanMsgRecv.data[6]]);
					//g_localDataLog[27] = "B路光源状态: " + state[pTask->m_CanMsgRecv.data[6]]; //本地数据 B路 光源状态
					g_localDataLog[27].SetString("B路光源状态: " + state[pTask->m_CanMsgRecv.data[6]]); //本地数据 B路 光源状态
				}

				if( pTask->m_CanMsgRecv.data[2] == 0X02	&&//数据包2
					pTask->m_CanMsgRecv.data[7] == 0X00)
				{
					float s1 = ((float)pTask->m_CanMsgRecv.data[3])/100;
					CString str1;
					str1.Format("%.2f",s1);
					g_pMainDlg->m_Page1.m_pLightState.SetItemText(2,4, str1);
					//g_localDataLog[28] = "B路T1: " + str1; //本地数据 B路 T1
					g_localDataLog[28].SetString("B路T1: " + str1); //本地数据 B路 T1
					
					float s2 = ((float)pTask->m_CanMsgRecv.data[4])/100;
					CString str2;
					str2.Format("%.2f",s2);
					g_pMainDlg->m_Page1.m_pLightState.SetItemText(2,5, str2);
					//g_localDataLog[29] = "B路T2: " + str2; //本地数据 B路 T2
					g_localDataLog[29].SetString("B路T2: " + str2); //本地数据 B路 T2

					float s3 = ((float)pTask->m_CanMsgRecv.data[5])/100;
					CString str3;
					str3.Format("%.2f",s3);
					g_pMainDlg->m_Page1.m_pLightState.SetItemText(2,6, str3);
					//g_localDataLog[30] = "B路T3: " + str3; //本地数据 B路 T3
					g_localDataLog[30].SetString("B路T3: " + str3); //本地数据 B路 T3

					float s4 = ((float)pTask->m_CanMsgRecv.data[6])/100;		//温度
					CString str4;
					str4.Format("%.2f",s4);
					g_pMainDlg->m_Page1.m_pLightState.SetItemText(2,7, str4);
					//g_localDataLog[31] = "B路温度: " + str4; //本地数据 B路 温度
					g_localDataLog[31].SetString("B路温度: " + str4); //本地数据 B路 温度
				}
				break;
			case 0X10:
				//电池状态反馈2。。。。。
				if( pTask->m_CanMsgRecv.data[2] == 0X01	&&//数据包1
					pTask->m_CanMsgRecv.data[7] == 0X00)
				{
					/*g_stateToUp[216] = 0;
					g_stateToUp[217] = pTask->m_CanMsgRecv.data[3]/10;
					g_stateToUp[218] = 0;
					g_stateToUp[219] = pTask->m_CanMsgRecv.data[5]/10;*/
					g_stateToUp[216] = pTask->m_CanMsgRecv.data[3];    //B路震荡级电流
					g_stateToUp[217] = 0;
					g_stateToUp[218] = pTask->m_CanMsgRecv.data[5];	  //B路放大级1电流
					g_stateToUp[219] = 0;

					int i = 2;//第2行
					CStringArray state;
					state.Add(_T("空闲状态"));
					state.Add(_T("外部刹车"));
					state.Add(_T("超温"));
					state.Add(_T("输出欠流"));
					state.Add(_T("输出过流"));
					state.Add(_T("输出过压"));
					state.Add(_T("输入异常"));
					state.Add(_T("正常输出"));

					CString str1;
					float s1 = ((float)pTask->m_CanMsgRecv.data[3])/10;    //B路震荡级电流
					str1.Format("%.1f",s1);
					g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(i, 0, str1);

					//g_localDataLog[8] = "B震荡级电流: " + str1; //本地数据 //B路震荡级电流
					g_localDataLog[8].SetString("B震荡级电流: " + str1); //本地数据 //B路震荡级电流

					int tmp = 0;
					if(pTask->m_CanMsgRecv.data[4] == 0x00)	tmp = 0;
					if(pTask->m_CanMsgRecv.data[4] == 0x02)	tmp = 1;
					if(pTask->m_CanMsgRecv.data[4] == 0x04)	tmp = 2;
					if(pTask->m_CanMsgRecv.data[4] == 0x08)	tmp = 3;
					if(pTask->m_CanMsgRecv.data[4] == 0x10)	tmp = 4;
					if(pTask->m_CanMsgRecv.data[4] == 0x20)	tmp = 5;
					if(pTask->m_CanMsgRecv.data[4] == 0x40)	tmp = 6;
					if(pTask->m_CanMsgRecv.data[4] == 0x80)	tmp = 7;
					g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(i, 1, state[tmp]);  //震荡级状态

					//g_localDataLog[9] = "B震荡级状态: " + state[tmp]; //本地数据 B路震荡级状态
					g_localDataLog[9].SetString("B震荡级状态: " + state[tmp]); //本地数据 B路震荡级状态

					if(tmp ==0)		g_stateToUp[230] = 1;		//电源状态
					else if(tmp ==7)	g_stateToUp[230] = 2;
					else	g_stateToUp[230] = 3;

					float s3 = ((float)pTask->m_CanMsgRecv.data[5])/10;
					str1.Format("%.1f",s3);
					g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(i, 2, str1);	//放大级1电流

					//g_localDataLog[10] = "B放大级1电流: " + str1; //本地数据 B路放大级1电流
					g_localDataLog[10].SetString("B放大级1电流: " + str1); //本地数据 B路放大级1电流

					if(pTask->m_CanMsgRecv.data[6] == 0x00)	tmp = 0;
					if(pTask->m_CanMsgRecv.data[6] == 0x02)	tmp = 1;
					if(pTask->m_CanMsgRecv.data[6] == 0x04)	tmp = 2;
					if(pTask->m_CanMsgRecv.data[6] == 0x08)	tmp = 3;
					if(pTask->m_CanMsgRecv.data[6] == 0x10)	tmp = 4;
					if(pTask->m_CanMsgRecv.data[6] == 0x20)	tmp = 5;
					if(pTask->m_CanMsgRecv.data[6] == 0x40)	tmp = 6;
					if(pTask->m_CanMsgRecv.data[6] == 0x80)	tmp = 7;
					g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(i, 3, state[tmp]);  //放大级 1 状态
					
					//g_localDataLog[11] = "B放大级1状态: " + state[tmp]; //本地数据 B路放大级 1 状态
					g_localDataLog[11].SetString("B放大级1状态: " + state[tmp]); //本地数据 B路放大级 1 状态

					((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_EDIT_StatusReceiveB)))->SetWindowText(ReceiveData);
				}
				if( pTask->m_CanMsgRecv.data[2] == 0X02	&&//数据包2
					pTask->m_CanMsgRecv.data[7] == 0X00)
				{
					g_stateToUp[220] = pTask->m_CanMsgRecv.data[3];      
					g_stateToUp[221] = 0;                                //放大级电流
					g_stateToUp[233] = pTask->m_CanMsgRecv.data[5];    //放大级3电流

					int i = 2;//第2行
					CStringArray state;
					state.Add(_T("空闲状态"));
					state.Add(_T("外部刹车"));
					state.Add(_T("超温"));
					state.Add(_T("输出欠流"));
					state.Add(_T("输出过流"));
					state.Add(_T("输出过压"));
					state.Add(_T("输入异常"));
					state.Add(_T("正常输出"));

					CString str1;
					float s1 = ((float)pTask->m_CanMsgRecv.data[3])/10;     //放大级 2 输出电流
					str1.Format("%.1f",s1);
					g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(i, 4, str1);

					//g_localDataLog[12] = "B放大级2电流: " + str1; //本地数据 B路放大级 2 输出电流
					g_localDataLog[12].SetString("B放大级2电流: " + str1); //本地数据 B路放大级 2 输出电流

					int tmp = 0;
					if(pTask->m_CanMsgRecv.data[4] == 0x00)	tmp = 0;
					if(pTask->m_CanMsgRecv.data[4] == 0x02)	tmp = 1;
					if(pTask->m_CanMsgRecv.data[4] == 0x04)	tmp = 2;
					if(pTask->m_CanMsgRecv.data[4] == 0x08)	tmp = 3;
					if(pTask->m_CanMsgRecv.data[4] == 0x10)	tmp = 4;
					if(pTask->m_CanMsgRecv.data[4] == 0x20)	tmp = 5;
					if(pTask->m_CanMsgRecv.data[4] == 0x40)	tmp = 6;
					if(pTask->m_CanMsgRecv.data[4] == 0x80)	tmp = 7;
					g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(i, 5, state[tmp]); //放大级 2 状态

					//g_localDataLog[13] = "B放大级2状态: " + state[tmp]; //本地数据 B路放大级 2 状态
					g_localDataLog[13].SetString("B放大级2状态: " + state[tmp]); //本地数据 B路放大级 2 状态

					float s3 = ((float)pTask->m_CanMsgRecv.data[5])/10;
					str1.Format("%.1f",s3);
					g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(i, 6, str1);  //放大级 3 输出电流

					//g_localDataLog[14] = "B放大级3电流: " + str1; //本地数据 B路放大级 3 输出电流
					g_localDataLog[14].SetString("B放大级3电流: " + str1); //本地数据 B路放大级 3 输出电流

					if(pTask->m_CanMsgRecv.data[6] == 0x00)	tmp = 0;
					if(pTask->m_CanMsgRecv.data[6] == 0x02)	tmp = 1;
					if(pTask->m_CanMsgRecv.data[6] == 0x04)	tmp = 2;
					if(pTask->m_CanMsgRecv.data[6] == 0x08)	tmp = 3;
					if(pTask->m_CanMsgRecv.data[6] == 0x10)	tmp = 4;
					if(pTask->m_CanMsgRecv.data[6] == 0x20)	tmp = 5;
					if(pTask->m_CanMsgRecv.data[6] == 0x40)	tmp = 6;
					if(pTask->m_CanMsgRecv.data[6] == 0x80)	tmp = 7;
					g_pMainDlg->m_Page1.m_pSeedSourceFB.SetItemText(i, 7, state[tmp]);  //放大级 3 状态
					
					//g_localDataLog[15] = "B放大级3状态: " + state[tmp]; //本地数据 B路放大级 3 状态
					g_localDataLog[15].SetString("B放大级3状态: " + state[tmp]); //本地数据 B路放大级 3 状态

					((CEdit*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_EDIT_StatusReceiveB2)))->SetWindowText(ReceiveData);
				}
				break;
			default:
				break;
			}
		default:
			break;
	}

	//电源选项
	switch(pTask->m_CanMsgRecv.data[0])
	{
		case 0X21:
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 1;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
					//	pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
					//	pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
					//	pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
					//	pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
					//	pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 1;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x04);
						}
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x05);
						}
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x06);
						}
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 1;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						//g_localDataLog[82 + (i - 1)*4] = "A1电流: " + str; //本地数据 A1电流
						g_localDataLog[82 + (i - 1)*4].SetString("A1电流: " + str); //本地数据 A1电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str); 
						str.Format("%d", data[5]*256+ data[6]); //电压
						//g_localDataLog[83 + (i - 1)*4] = "A1电压: " + str; //本地数据 A1电压
						g_localDataLog[83 + (i - 1)*4].SetString("A1电压: " + str); //本地数据 A1电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str); 
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4] = "A1状态字1: " + str; //本地数据 A1状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A1状态字1: " + str); //本地数据 A1状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4] = "A1状态字2: " + g_pMainDlg->options[index]; //本地数据 A1状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A1状态字2: " + g_pMainDlg->options[index]); //本地数据 A1状态字2

						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[6];
						g_stateToUp[3+(i-1)*8] = data[5];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B1电流: " + str; //本地数据 B1电流
						g_localDataLog[118 + (i - 1)*4].SetString("B1电流: " + str); //本地数据 B1电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str);
						//g_localDataLog[119 + (i - 1)*4] = "B1电压: " + str; //本地数据 B1电压
						g_localDataLog[119 + (i - 1)*4].SetString("B1电压: " + str); //本地数据 B1电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
					    //g_localDataLog[120 + (i - 1)*4] = "B1状态字1: " + str; //本地数据 B1状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B1状态字1: " + str); //本地数据 B1状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B1状态字2: " + g_pMainDlg->options[index]; //本地数据 B1状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B1状态字2: " + g_pMainDlg->options[index]); //本地数据 B1状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = (((data[7]<<4)==0) ? 0 : 1);
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		case 0X22:
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);

						int i = 2;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i, 2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i, 3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i, 4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i, 5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i, 6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x22, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
				//		pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
				//		pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
				//		pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
				//		pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
				//		pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 2;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[5] == 0X00 //总输出电流整数位
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x22, 0x04);
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[5] == 0X00 //总输出电流整数位
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x22, 0x05);
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x22, 0x06);
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 2;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str); 
						//g_localDataLog[82 + (i - 1)*4] = "A2电流: " + str; //本地数据 A2电流
						g_localDataLog[82 + (i - 1)*4].SetString("A2电流: " + str); //本地数据 A2电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str); 
						//g_localDataLog[83 + (i - 1)*4] = "A2电压: " + str; //本地数据 A2电压
						g_localDataLog[83 + (i - 1)*4].SetString("A2电压: " + str); //本地数据 A2电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4] = "A2状态字1: " + str; //本地数据 A2状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A2状态字1: " + str); //本地数据 A2状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4] = "A2状态字2: " + g_pMainDlg->options[index]; //本地数据 A2状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A2状态字2: " + g_pMainDlg->options[index]); //本地数据 A2状态字2

						/*g_stateToUp[0+(i-1)*8] = data[3]/256;
						g_stateToUp[1+(i-1)*8] = data[3]%256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];*/
						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[6];
						g_stateToUp[3+(i-1)*8] = data[5];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B2电流: " + str; //本地数据 B2电流
						g_localDataLog[118 + (i - 1)*4].SetString("B2电流: " + str); //本地数据 B2电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str); 
						//g_localDataLog[119 + (i - 1)*4] = "B2电压: " + str; //本地数据 B2电压
						g_localDataLog[119 + (i - 1)*4].SetString("B2电压: " + str); //本地数据 B2电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
						//g_localDataLog[120 + (i - 1)*4] = "B2状态字1: " + str; //本地数据 B2状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B2状态字1: " + str); //本地数据 B2状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B2状态字2: " + g_pMainDlg->options[index]; //本地数据 B2状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B2状态字2: " + g_pMainDlg->options[index]); //本地数据 B2状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = (((data[7]<<4)==0) ? 0 : 1);
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		case 0X23:	
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 3;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x23, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
					//	pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
					//	pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
					//	pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
					//	pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
					//	pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 3;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x23, 0x04);
						}
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x23, 0x05);
						}
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x23, 0x06);
						}
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 3;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str);
						//g_localDataLog[82 + (i - 1)*4] = "A3电流: " + str; //本地数据 A3电流
						g_localDataLog[82 + (i - 1)*4].SetString("A3电流: " + str); //本地数据 A3电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str); 
						//g_localDataLog[83 + (i - 1)*4] = "A3电压: " + str; //本地数据 A3电压
						g_localDataLog[83 + (i - 1)*4].SetString("A3电压: " + str); //本地数据 A3电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4] = "A3状态字1: " + str; //本地数据 A3状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A3状态字1: " + str); //本地数据 A3状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4] = "A3状态字2: " + g_pMainDlg->options[index]; //本地数据 A3状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A3状态字2: " + g_pMainDlg->options[index]); //本地数据 A3状态字2

						/*g_stateToUp[0+(i-1)*8] = data[3]/256;
						g_stateToUp[1+(i-1)*8] = data[3]%256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];*/
						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B3电流: " + str; //本地数据 B3电流
						g_localDataLog[118 + (i - 1)*4].SetString("B3电流: " + str); //本地数据 B3电流

						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str); 
						//g_localDataLog[119 + (i - 1)*4] = "B3电压: " + str; //本地数据 B3电压
						g_localDataLog[119 + (i - 1)*4].SetString("B3电压: " + str); //本地数据 B3电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
						//g_localDataLog[120 + (i - 1)*4] = "B3状态字1: " + str; //本地数据 B3状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B3状态字1: " + str); //本地数据 B3状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B3状态字2: " + g_pMainDlg->options[index]; //本地数据 B3状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B3状态字2: " + g_pMainDlg->options[index]); //本地数据 B3状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = (((data[7]<<4)==0) ? 0 : 1);
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		case 0X24:	
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 4;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x24, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
						//pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
					//	pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						//pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						//pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						//pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 4;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x24, 0x04);
						}
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x24, 0x05);
						}
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x24, 0x06);
						}
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 4;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str); 
						//g_localDataLog[82 + (i - 1)*4]  = "A4电流: " + str;                           //本地数据 A4电流
						g_localDataLog[82 + (i - 1)*4].SetString("A4电流: " + str);                           //本地数据 A4电流

						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str);
						//g_localDataLog[83 + (i - 1)*4]  = "A4电压: " + str;                           //本地数据 A4电压
						g_localDataLog[83 + (i - 1)*4].SetString("A4电压: " + str);                           //本地数据 A4电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4]  = "A4状态字1: " + str;                        //本地数据 A4状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A4状态字1: " + str);                        //本地数据 A4状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4]  = "A4状态字2: " + g_pMainDlg->options[index]; //本地数据 A4状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A4状态字2: " + g_pMainDlg->options[index]); //本地数据 A4状态字2

						/*g_stateToUp[0+(i-1)*8] = data[3]/256;
						g_stateToUp[1+(i-1)*8] = data[3]%256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];*/
						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B4电流: " + str; 						  //本地数据 B4电流
						g_localDataLog[118 + (i - 1)*4].SetString("B4电流: " + str); 						  //本地数据 B4电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str); 
						//g_localDataLog[119 + (i - 1)*4] = "B4电压: " + str; 						  //本地数据 B4电压
						g_localDataLog[119 + (i - 1)*4].SetString("B4电压: " + str); 						  //本地数据 B4电压

						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
						//g_localDataLog[120 + (i - 1)*4] = "B4状态字1: " + str; 						  //本地数据 B4状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B4状态字1: " + str); 						  //本地数据 B4状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B4状态字2: " + g_pMainDlg->options[index]; //本地数据 B4状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B4状态字2: " + g_pMainDlg->options[index]); //本地数据 B4状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = (((data[7]<<4)==0) ? 0 : 1);
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		case 0X25:
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 5;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x25, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
						//pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
						//pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						//pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						//pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						//pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 5;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x25, 0x04);
						}
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x25, 0x05);
						}
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x25, 0x06);
						}
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 5;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str);
						//g_localDataLog[82 + (i - 1)*4]  = "A5电流: " + str;                           //本地数据 A5电流
						g_localDataLog[82 + (i - 1)*4].SetString("A5电流: " + str);                           //本地数据 A5电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str); 
						//g_localDataLog[83 + (i - 1)*4]  = "A5电压: " + str;                           //本地数据 A5电压
						g_localDataLog[83 + (i - 1)*4].SetString("A5电压: " + str);                           //本地数据 A5电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4]  = "A5状态字1: " + str;                        //本地数据 A5状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A5状态字1: " + str);                        //本地数据 A5状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4]  = "A5状态字2: " + g_pMainDlg->options[index]; //本地数据 A5状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A5状态字2: " + g_pMainDlg->options[index]); //本地数据 A5状态字2

						/*g_stateToUp[0+(i-1)*8] = data[3]/256;
						g_stateToUp[1+(i-1)*8] = data[3]%256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];*/
						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B5电流: " + str; 						  //本地数据 B5电流
						g_localDataLog[118 + (i - 1)*4].SetString("B5电流: " + str); 						  //本地数据 B5电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str);
						//g_localDataLog[119 + (i - 1)*4] = "B5电压: " + str; 						  //本地数据 B5电压
						g_localDataLog[119 + (i - 1)*4].SetString("B5电压: " + str); 						  //本地数据 B5电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
						//g_localDataLog[120 + (i - 1)*4] = "B5状态字1: " + str; 						  //本地数据 B5状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B5状态字1: " + str); 						  //本地数据 B5状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B5状态字2: " + g_pMainDlg->options[index]; //本地数据 B5状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B5状态字2: " + g_pMainDlg->options[index]); //本地数据 B5状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = (((data[7]<<4)==0) ? 0 : 1);
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		case 0X26:
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 6;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x26, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
					//	pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
					//	pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
					//	pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
					//	pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
					//	pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 6;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x26, 0x04);
						}
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x26, 0x05);
						}
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x26, 0x06);
						}
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 6;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str); 
						//g_localDataLog[82 + (i - 1)*4]  = "A6电流: " + str;                           //本地数据 A6电流
						g_localDataLog[82 + (i - 1)*4].SetString("A6电流: " + str);                           //本地数据 A6电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str); 
						//g_localDataLog[83 + (i - 1)*4]  = "A6电压: " + str;                           //本地数据 A6电压
						g_localDataLog[83 + (i - 1)*4].SetString("A6电压: " + str);                           //本地数据 A6电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4]  = "A6状态字1: " + str;                        //本地数据 A6状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A6状态字1: " + str);                        //本地数据 A6状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4]  = "A6状态字2: " + g_pMainDlg->options[index]; //本地数据 A6状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A6状态字2: " + g_pMainDlg->options[index]); //本地数据 A6状态字2

						/*g_stateToUp[0+(i-1)*8] = data[3]/256;
						g_stateToUp[1+(i-1)*8] = data[3]%256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];*/
						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B6电流: " + str; 						  //本地数据 B6电流
						g_localDataLog[118 + (i - 1)*4].SetString("B6电流: " + str); 						  //本地数据 B6电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str); 
						//g_localDataLog[119 + (i - 1)*4] = "B6电压: " + str; 						  //本地数据 B6电压
						g_localDataLog[119 + (i - 1)*4].SetString("B6电压: " + str); 						  //本地数据 B6电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
						//g_localDataLog[120 + (i - 1)*4] = "B6状态字1: " + str; 						  //本地数据 B6状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B6状态字1: " + str); 						  //本地数据 B6状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B6状态字2: " + g_pMainDlg->options[index]; //本地数据 B6状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B6状态字2: " + g_pMainDlg->options[index]); //本地数据 B6状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = ((data[7]<<4)==0) ? 0 : 1;
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		case 0X27:
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 7;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x27, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
					//	pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
					//	pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
					//	pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
					//	pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
					//	pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 7;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x27, 0x04);
						}
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x27, 0x05);
						}
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x27, 0x06);
						}
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 7;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str);
						//g_localDataLog[82 + (i - 1)*4]  = "A7电流: " + str;                           //本地数据 A7电流
						g_localDataLog[82 + (i - 1)*4].SetString("A7电流: " + str);                           //本地数据 A7电流						
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str); 
						//g_localDataLog[83 + (i - 1)*4]  = "A7电压: " + str;                           //本地数据 A7电压
						g_localDataLog[83 + (i - 1)*4].SetString("A7电压: " + str);                           //本地数据 A7电压						
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4]  = "A7状态字1: " + str;                        //本地数据 A7状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A7状态字1: " + str);                        //本地数据 A7状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4]  = "A7状态字2: " + g_pMainDlg->options[index]; //本地数据 A7状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A7状态字2: " + g_pMainDlg->options[index]); //本地数据 A7状态字2

						/*g_stateToUp[0+(i-1)*8] = data[3]/256;
						g_stateToUp[1+(i-1)*8] = data[3]%256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];*/
						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B7电流: " + str; 						  //本地数据 B7电流
						g_localDataLog[118 + (i - 1)*4].SetString("B7电流: " + str); 						  //本地数据 B7电流						
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str); 
						//g_localDataLog[119 + (i - 1)*4] = "B7电压: " + str; 						  //本地数据 B7电压
						g_localDataLog[119 + (i - 1)*4].SetString("B7电压: " + str); 						  //本地数据 B7电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
						//g_localDataLog[120 + (i - 1)*4] = "B7状态字1: " + str; 						  //本地数据 B7状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B7状态字1: " + str); 						  //本地数据 B7状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B7状态字2: " + g_pMainDlg->options[index]; //本地数据 B7状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B7状态字2: " + g_pMainDlg->options[index]); //本地数据 B7状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = (((data[7]<<4)==0) ? 0 : 1);
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		case 0X28:
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 8;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x28, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
					//	pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
					//	pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
					//	pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
					//	pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
					//	pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 8;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x28, 0x04);
						}
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x28, 0x05);
						}
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x28, 0x06);
						}
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 8;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str); 
						//g_localDataLog[82 + (i - 1)*4]  = "A8电流: " + str;                           //本地数据 A8电流
						g_localDataLog[82 + (i - 1)*4].SetString("A8电流: " + str);                           //本地数据 A8电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str); 
						//g_localDataLog[83 + (i - 1)*4]  = "A8电压: " + str;                           //本地数据 A8电压
						g_localDataLog[83 + (i - 1)*4].SetString("A8电压: " + str);                           //本地数据 A8电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4]  = "A8状态字1: " + str;                        //本地数据 A8状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A8状态字1: " + str);                        //本地数据 A8状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4]  = "A8状态字2: " + g_pMainDlg->options[index]; //本地数据 A8状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A8状态字2: " + g_pMainDlg->options[index]); //本地数据 A8状态字2

						/*g_stateToUp[0+(i-1)*8] = data[3]/256;
						g_stateToUp[1+(i-1)*8] = data[3]%256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];*/
						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B8电流: " + str; 						  //本地数据 B8电流
						g_localDataLog[118 + (i - 1)*4].SetString("B8电流: " + str); 						  //本地数据 B8电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str); 
						//g_localDataLog[119 + (i - 1)*4] = "B8电压: " + str; 						  //本地数据 B8电压
						g_localDataLog[119 + (i - 1)*4].SetString("B8电压: " + str); 						  //本地数据 B8电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
						//g_localDataLog[120 + (i - 1)*4] = "B8状态字1: " + str; 						  //本地数据 B8状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B8状态字1: " + str); 						  //本地数据 B8状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B8状态字2: " + g_pMainDlg->options[index]; //本地数据 B8状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B8状态字2: " + g_pMainDlg->options[index]); //本地数据 B8状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = (((data[7]<<4)==0) ? 0 : 1);
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		case 0X29:
			switch(pTask->m_CanMsgRecv.data[1])
			{
			case 0XA1:
				{
					//加载参数反馈。。。。。
					if( pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2]	&&//电流值1
						pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
						pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
						pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
						pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0X00)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 9;
						if(pTask->m_szCanPort != "2")
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(255, 0, 0));

							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x29, 0x02);
						}
						else
						{
							if (g_taskDown > 0)
							{
								g_taskDown++;
							}
							
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(255, 0, 0));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(255, 0, 0));
						}
					}
					if( 
					//	pTask->m_CanMsgRecv.data[2] == pTask->m_CanMsgSend.data[2] &&//电流值1
					//	pTask->m_CanMsgRecv.data[3] == pTask->m_CanMsgSend.data[3] && //电流值2
					//	pTask->m_CanMsgRecv.data[4] == pTask->m_CanMsgSend.data[4] &&//电流值3
					//	pTask->m_CanMsgRecv.data[5] == pTask->m_CanMsgSend.data[5] &&//电流值4
					//	pTask->m_CanMsgRecv.data[6] == pTask->m_CanMsgSend.data[6] &&//延迟间隔
						pTask->m_CanMsgRecv.data[7] == 0Xff)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						int i = 9;
						if(pTask->m_szCanPort != "2")
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower1.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower1.SetItemFgColour(i,6, RGB(0, 0, 255));

							//::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x21, 0x02);
						}
						else
						{
							CString current1,current2,current3,current4,delay;
							current1.Format("%d", data[2]);
							current2.Format("%d", data[3]);
							current3.Format("%d", data[4]);
							current4.Format("%d", data[5]);
							delay.Format("%d", data[6]);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 2, current1);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 3, current2);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 4, current3);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 5, current4);
							g_pMainDlg->m_Page1.m_pPower2.SetItemText(i, 6, delay);
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,2, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,3, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,4, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,5, RGB(0, 0, 255));
							g_pMainDlg->m_Page1.m_pPower2.SetItemFgColour(i,6, RGB(0, 0, 255));
						}
					}
				}
				break;
			case 0X04:
				{
					//输出控制反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X01 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X01 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x29, 0x04);
						}
					}
					//停止输出反馈
					if( 
						pTask->m_CanMsgRecv.data[2] == 0X00	&&//通道标识位 0X00
						pTask->m_CanMsgRecv.data[3] == 0X00 &&//控制指令
						pTask->m_CanMsgRecv.data[4] == 0X00 //控制指令
						)
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x29, 0x05);
						}
					}
				}
				break;
			case 0X06:
				{
					//复位控制反馈
					if( pTask->m_CanMsgRecv.data[2] == 0X00	)//通道标识位 0X00
					{
						((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST_ReceiveToPower)))->InsertString(0, ReceiveData);
						if(pTask->m_szCanPort != "2")
						{
							::PostMessageA(g_pMainDlg->m_Page1, WM_CHANGE_LED, 0x29, 0x06);
						}
					}
				}
				break;
			case 0X08:
				{
					((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->InsertString(0, ReceiveData);
					int i = 9;
					if(pTask->m_szCanPort != "2")
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,0, str); 
						//g_localDataLog[82 + (i - 1)*4]  = "A9电流: " + str;                           //本地数据 A9电流
						g_localDataLog[82 + (i - 1)*4].SetString("A9电流: " + str);                           //本地数据 A9电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,1, str); 
						//g_localDataLog[83 + (i - 1)*4]  = "A9电压: " + str;                           //本地数据 A9电压
						g_localDataLog[83 + (i - 1)*4].SetString("A9电压: " + str);                           //本地数据 A9电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,2, str);//状态字1
						//g_localDataLog[84 + (i - 1)*4]  = "A9状态字1: " + str;                        //本地数据 A9状态字1
						g_localDataLog[84 + (i - 1)*4].SetString("A9状态字1: " + str);                        //本地数据 A9状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv1.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[85 + (i - 1)*4]  = "A9状态字2: " + g_pMainDlg->options[index]; //本地数据 A9状态字2
						g_localDataLog[85 + (i - 1)*4].SetString("A9状态字2: " + g_pMainDlg->options[index]); //本地数据 A9状态字2

						/*g_stateToUp[0+(i-1)*8] = data[3]/256;
						g_stateToUp[1+(i-1)*8] = data[3]%256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];*/
						g_stateToUp[0+(i-1)*8] = data[3]%256;
						g_stateToUp[1+(i-1)*8] = data[3]/256;
						g_stateToUp[2+(i-1)*8] = data[5];
						g_stateToUp[3+(i-1)*8] = data[6];
						g_stateToUp[4+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[5+(i-1)*8] = ((data[7]<<4)==0 ? 0 : 1);
						g_stateToUp[6+(i-1)*8] = 0;
						//g_stateToUp[7+(i-1)*8] = 0;

						g_muxianOK[i] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
					else
					{
						CString str;
						str.Format("%u%s%u", data[3], ".",data[4]); //电流
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,0, str); 
						//g_localDataLog[118 + (i - 1)*4] = "B9电流: " + str; 						  //本地数据 B9电流
						g_localDataLog[118 + (i - 1)*4].SetString("B9电流: " + str); 						  //本地数据 B9电流
						str.Format("%d", (data[5]*256+ data[6])/10); //电压
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,1, str); 
						//g_localDataLog[119 + (i - 1)*4] = "B9电压: " + str; 						  //本地数据 B9电压
						g_localDataLog[119 + (i - 1)*4].SetString("B9电压: " + str); 						  //本地数据 B9电压
						if(data[7]>>4 == 0) 
							str = "空闲状态";
						if(data[7]>>4 == 1) 
							str = "输出状态";
						if(data[7]>>4 == 2) 
							str = "故障状态";
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,2, str);//状态字1
						//g_localDataLog[120 + (i - 1)*4] = "B9状态字1: " + str; 						  //本地数据 B9状态字1
						g_localDataLog[120 + (i - 1)*4].SetString("B9状态字1: " + str); 						  //本地数据 B9状态字1

						int index = data[7]&0x0f;
						g_pMainDlg->m_Page1.m_pPower_Recv2.SetItemText(i,3, g_pMainDlg->options[index]);//状态字2
						//g_localDataLog[121 + (i - 1)*4] = "B9状态字2: " + g_pMainDlg->options[index]; //本地数据 B9状态字2
						g_localDataLog[121 + (i - 1)*4].SetString("B9状态字2: " + g_pMainDlg->options[index]); //本地数据 B9状态字2

						/*g_stateToUp[144+(i-1)*8] = data[3]/256;
						g_stateToUp[145+(i-1)*8] = data[3]%256;
						g_stateToUp[146+(i-1)*8] = data[5];
						g_stateToUp[147+(i-1)*8] = data[6];*/
						g_stateToUp[144+(i-1)*8] = data[3]%256;
						g_stateToUp[145+(i-1)*8] = data[3]/256;
						g_stateToUp[146+(i-1)*8] = data[6];
						g_stateToUp[147+(i-1)*8] = data[5];
						g_stateToUp[148+(i-1)*8] = (data[7]>>4)+1;
						g_stateToUp[149+(i-1)*8] = (((data[7]<<4)==0) ? 0 : 1);
						g_stateToUp[150+(i-1)*8] = 0;
						//g_stateToUp[151+(i-1)*8] = 0;

						g_muxianOK[i+10] = ((data[7]>>7)==0 ? TRUE : FALSE);//母线正常flag
					}
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
	}
	delete pTask;
	pTask = NULL;
	return 0;
}


void CMornSDKDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case 0:
		{
			BYTE up[312]={0};
			up[0] = 0xff;up[1] = 0xff;up[2] = 0xff;up[3] = 0xff;up[4] = 0xff;up[5] = 0xff;
			up[6] = 0x44;up[7] = 0x5A;
			up[8] = 300%256;
			up[9] = 300/256;
			up[10] = ~(300%256);
			up[11] = ~(300/256);
			up[12] = 5000%256;
			up[13] = 5000/256;
			up[14] = 0;
			up[15] = 0;
			g_stateToUp[288]=0;
			g_stateToUp[289]=0;
			g_stateToUp[290]=0;
			g_stateToUp[291]=0;

			if(g_stateToUp[87]==3)
			g_stateToUp[288]|=0x01;
			if(g_stateToUp[101]==7)
				g_stateToUp[288]|=0x10;
			if(g_stateToUp[111]==7)
				g_stateToUp[288]|=0x20;
			if(g_stateToUp[121]==7)
				g_stateToUp[288]|=0x40;
			if(g_stateToUp[131]==7)
				g_stateToUp[288]|=0x80;
			if(g_stateToUp[141]==7)
				g_stateToUp[289]|=0x01;
			if(g_stateToUp[4]==3)
				g_stateToUp[289]|=0x02;
			if((g_stateToUp[12]==3)||(g_stateToUp[20]==3))
				g_stateToUp[289]|=0x04;
			if((g_stateToUp[28]==3)||(g_stateToUp[36]==3))
				g_stateToUp[289]|=0x08;
			if((g_stateToUp[44]==3)||(g_stateToUp[52]==3))
				g_stateToUp[289]|=0x10;
			if((g_stateToUp[60]==3)||(g_stateToUp[68]==3))
				g_stateToUp[289]|=0x20;
			if(g_stateToUp[231]==3)
				g_stateToUp[289]|=0x40;
			if(g_stateToUp[245]==7)
				g_stateToUp[290]|=0x04;
			if(g_stateToUp[255]==7)
				g_stateToUp[290]|=0x08;
			if(g_stateToUp[265]==7)
				g_stateToUp[290]|=0x10;
			if(g_stateToUp[275]==7)
				g_stateToUp[290]|=0x20;
			if(g_stateToUp[285]==7)
				g_stateToUp[290]|=0x40;
			if(g_stateToUp[148]==3)
				g_stateToUp[290]|=0x80;
			if((g_stateToUp[156]==3)||(g_stateToUp[164]==3))
				g_stateToUp[291]|=0x01;
			if((g_stateToUp[172]==3)||(g_stateToUp[180]==3))
				g_stateToUp[291]|=0x02;
			if((g_stateToUp[188]==3)||(g_stateToUp[196]==3))
				g_stateToUp[291]|=0x04;
			if((g_stateToUp[204]==3)||(g_stateToUp[212]==3))
				g_stateToUp[291]|=0x08;
            g_stateToUp[292]=0;
			g_stateToUp[293]=0;
			for(int i = 0; i < 294; i++)
			{
				up[16+i] = g_stateToUp[i];
			}
			up[310] = checkSum16(up, 310)%256;
			up[311] = checkSum16(up, 310)/256;
			g_pMainDlg->udpHelper.Send(up, 312);	//上报udp数据包
			
			for (int i = 0; i < 294; i++)
			{
				g_stateToUp[i] = 0;//清楚实时上报数据
			}

			if(g_hotBoot > 0)
			{
				g_hotBoot++;
				int sum = 0;
				if(g_hotBoot == 10)  //10s在判断
				{
					for(int i = 1; i < 11; i++)
					{
						if(g_yuchongOK[i] == TRUE)
						{
							sum++;
							DWORD tmpID = 0X1806FE00 + DWORD(i);
							g_pCanManager4->DisChargeControl(tmpID, 80, NULL);
							g_pCanManager4->DisChargeControl(tmpID, 64, NULL);
							g_yuchongOK[i] = FALSE;
						}
					}
					//if(sum > 9)
						splitStrUdp(1108, 1, 0);  //热启动 成功
					//else
					//	splitStrUdp(1108, 1, 1);  //热启动 失败
					//清楚标志位
					g_hotBoot = 0;
					
				}
			}	
			if(g_batoutput > 0)
			{
				g_batoutput++;
				int sum = 0;
				if(g_batoutput == 10)  //10s在判断
				{
					for(int i = 1; i < 11; i++)
					{
						DWORD tmpID = 0X1806FE00 + DWORD(i);
						//int index = ((CGridCellCombo*)(g_pMainDlg->m_Page1.m_pBattery.GetCell(i, 2)))->getCurSel();//获得当先combo的index
						if(g_yuchongOK[i] == TRUE)
						{
							g_pCanManager4->DisChargeControl(tmpID, 80, NULL);
							Sleep(50);
							g_pCanManager4->DisChargeControl(tmpID, 64, NULL);
							g_yuchongOK[i] = FALSE;
						}
					}
					g_batoutput = 0;
				}
			}
		}
		break;
	case 1:
		{
			if(g_chargeEnableFlag == 0)//不允许充电
			{
				for(int i = 1; i < 11; i++)
				{
					if(g_batState[i] == 0x40)  //如果是在充电，就停止充电，其他情况不动作
					{
						DWORD tmpID = 0X1806FE00 + DWORD(i);
						g_pCanManager4->StopChargeControl(tmpID, 0, 0, 0, 0, NULL);	
					}
				}
			}
			else if(g_chargeEnableFlag == 1)//允许小功率充电
			{
				for(int i = 1; i < 11; i++)
				{
					if(g_batVoltage[i] < 435)
					{   
						DWORD tmpID = 0X1806FE00 + DWORD(i);
						g_pCanManager4->ChargeControl(tmpID,0,80,0,0,NULL);
						Sleep(20);
						tmpID = 0X1807FE00 + DWORD(i);
						g_pCanManager4->BatteryChargeParameter_Supply(tmpID, 01, 02, 10, 2000, 450, 01, NULL);
					}	
				}
			}
			else if(g_chargeEnableFlag == 2)//允许大功率充电
			{
				for(int i = 1; i < 11; i++)
				{
					if(g_batVoltage[i] < 435)
					{
						DWORD tmpID = 0X1806FE00 + DWORD(i);
						g_pCanManager4->ChargeControl(tmpID,0,80,0,0,NULL);
						Sleep(20);
					     tmpID = 0X1807FE00 + DWORD(i);
						g_pCanManager4->BatteryChargeParameter_Supply(tmpID, 01, 02, 10, 4000, 450, 01, NULL);
					}
				}
			}
		}
		break;
	case 2:
		{
			//isStartQg++;
			WriteLocalStateLog();  //记录本地数据日志
			
		}
	case 3:
		{
			//isStartQg++;
			if(isStartQg == 1) //出光记录数据 5m记录一次
			{
				//WriteUpStateLog();
				g_seedsourceA[0].push_back(g_stateToUp[72]); //A 震荡级
				g_seedsourceA[1].push_back(g_stateToUp[74]); //A  放大1
				g_seedsourceA[2].push_back(g_stateToUp[76]); //A 放大2

				g_seedsourceB[0].push_back(g_stateToUp[216]); //B 震荡级
				g_seedsourceB[1].push_back(g_stateToUp[218]); //B  放大1
				g_seedsourceB[2].push_back(g_stateToUp[220]); //B 放大2

				for(int i = 0; i < 9; i++)
				{
					g_powersupplyA[i].push_back(g_stateToUp[i*8]);		 //  A路 电源
					g_powersupplyB[i].push_back(g_stateToUp[144 + i*8]); //  B路 电源
				}
				for(int i = 0; i < 10; i++) //记录soc数据
				{
					if(i < 5)
						g_soc[i].push_back(g_stateToUp[98 + i*10]);  //1-5号电池soc
					else
						g_soc[i].push_back(g_stateToUp[242 + (i-5)*10]); //6-10号电池soc
				}
			}
		}
	default: 
		break;
	}
	__super::OnTimer(nIDEvent);
}

void CMornSDKDlg::InitSerial()
{
	if (g_pSP == NULL)
		return;
	//若串口已经打开，则先关闭
	if (g_pSP->isOpened())
	{
		g_pSP->close();
		GetDlgItem(IDC_BTN_OPENCOM)->SetWindowText(_T("打开串口"));
	}
	else
	{
		g_pSP->init(13, 115200);
		//打开串口
		g_pSP->open();
		if (g_pSP->isOpened())
		{
			//GetDlgItem(IDC_BTN_OPENCOM)->SetWindowText(_T("关闭串口"));
			//MessageBox(_T("打开串口成功。"),_T("提示"));
		}
		else
		{

			//GetDlgItem(IDC_BTN_OPENCOM)->SetWindowText(_T("打开串口"));
			MessageBox(_T("打开串口失败。"),_T("提示"));
		}
	}
}

void CMornSDKDlg::OnSerialPortReceived()
{
	char * data = NULL;
	data = new char[512];
	g_pSP->readAllData(data);
	CString msg((char*)data);
	CString tmpStr = msg;

	msg = _T("接收到数据: ") + msg;
	CString str;
	//先获取文本框中已有的文本
	g_pMainDlg->m_Page3.GetDlgItem(IDC_EDIT_COMRECV)->GetWindowText(str);
	//添加新文本,显示
	str += msg + "\r\n";
	g_pMainDlg->m_Page3.GetDlgItem(IDC_EDIT_COMRECV)->SetWindowText(str);
	
	serialStr += tmpStr;
	if(serialStr.Find('<') >= 0  && serialStr.Find('>') >= 0 )
	{
		AnalyzStringFromUp(serialStr);
		serialStr = "";
	}
	else
	{
		return;
	}
}

//记录本地试验数据
void CMornSDKDlg::WriteLocalStateLog()	
{
	struct tm *newtime;
	time_t long_time;
	time(&long_time);
	newtime = localtime(&long_time);
	CString curtime;
	curtime.Format( "[%04d%02d%02d %02d:%02d:%02d]", newtime->tm_year + 1900,
		newtime->tm_mon + 1, newtime->tm_mday, newtime->tm_hour,
		newtime->tm_min, newtime->tm_sec);

	//WriteFile(g_strExePath + "..\\LocalDataFile\\" + g_nowTaskNum + ".txt", "");
	CString curDate = curtime.Mid(1, 8); //提取年月日
	WriteFile(curDate + ".txt", "");  //以日期为名
	CString res = curtime + "\r\n";
	for(int i = 0; i < 16; i++)
	{
		if(g_localDataLog[i] == "")
			continue;
		res += g_localDataLog[i] + "\r\n";
	}
	if(g_localDataLog[19] != "")
		res += g_localDataLog[19] + "\r\n";
	if(g_localDataLog[27] != "")
		res += g_localDataLog[27] + "\r\n";
	for(int i = 32; i < 82; i++)
	{
		if(g_localDataLog[i] == "")
			continue;
		res += g_localDataLog[i] + "\r\n";
	}
	for(int i = 82; i < 154; i++)
	{
		if((i - 82) % 4 == 1)	//去掉电压状态
			continue;
		res += g_localDataLog[i] + "\r\n";
	}
	/*for(int i = 0; i < 154; i++)
	{
	res += g_localDataLog[i] + " ";
	}*/
	res += "\r\n";
	//WriteFile(g_strExePath + "..\\LocalDataFile\\" + g_nowTaskNum + ".txt", res);
	WriteFile(curDate + ".txt", res);	//以日期为名
}

//记录任务编号试验数据
void CMornSDKDlg::WriteUpStateLog()	//需要修改
{
	CApplication wordApp;
	CDocuments docs;
	CDocument0 doc;
	CRange wordRange;
	try{
		//打开指定路径文件
		CString FileName = g_strExePath + "1.doc";
		if (!wordApp.CreateDispatch(_T("Word.Application"),NULL)) {
			AfxMessageBox(_T("Word failed to start!"));
			return ;
		}
		wordApp.put_Visible(FALSE);
		docs = wordApp.get_Documents();
		COleVariant vTRUE((short)TRUE);
		COleVariant vFALSE((short)FALSE);
		COleVariant vOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		COleVariant vFileName(FileName);
		doc = docs.Open(vFileName,vTRUE,vFALSE,vTRUE,vOptional,vOptional,vOptional,vOptional,vOptional,vOptional,vOptional,vOptional,vOptional,vOptional,vOptional,vOptional);
		wordRange = doc.Range(vOptional, vOptional);
		
		CSelection sel = wordApp.get_Selection();
		CString title = "HG100 " + g_nowTaskNum + "试验记录（电源分系统）\r\n";
		sel.TypeText(title);
		COleDateTime dt = COleDateTime::GetCurrentTime();
		CString strDT = dt.Format(_T("%Y-%m-%d"));
		sel.TypeText(strDT);

		CTables0 tables;
		tables = doc.get_Tables();
		//tables.AttachDispatch(doc.get_Tables());
		CTable0 table;
		table = tables.Item(1);
		//table.AttachDispatch(tables.Item(tables.get_Count()));

		CFont0 ocellFont;
		CCell cell = table.Cell(2, 2);//试验编号单元格
		CRange cellorange = cell.get_Range();
		ocellFont = cellorange.get_Font();
		ocellFont.put_Color(RGB(255, 0, 0)); //red
		cellorange.put_Text("试验编号");

		cell = table.Cell(2, 4);//出光时长
		cellorange = cell.get_Range();
		ocellFont = cellorange.get_Font();
		ocellFont.put_Color(RGB(255, 0, 0)); //red
		cellorange.put_Text("出光时长");

		cell = table.Cell(2, 6);//种子源出光模式%
		cellorange = cell.get_Range();
		ocellFont = cellorange.get_Font();
		ocellFont.put_Color(RGB(255, 0, 0)); //red
		cellorange.put_Text("种子源出光模式");

		int cnt = 0;
		for (int row = 6; row <= 9; row++) //种子源配置参数信息
		{
			for(int col = 2; col <= 4; col += 2)
			{
				cell = table.Cell(row, col);//种子源出光模式%
				cellorange = cell.get_Range();
				ocellFont = cellorange.get_Font();
				ocellFont.put_Color(RGB(255, 0, 0)); //red
				CString str;
				//str.Format("%d", cnt);
				str.Format("%d", SeedSource_Parameter[cnt]);
				cnt++;
				cellorange.put_Text(str);
			}
		}
		
		cnt = 0;
		for (int row = 11; row <= 20; row++) //驱动源配置参数信息
		{
			for(int col = 2; col <= 4; col += 2)
			{
				cell = table.Cell(row, col);//种子源出光模式%
				cellorange = cell.get_Range();
				ocellFont = cellorange.get_Font();
				ocellFont.put_Color(RGB(255, 0, 0)); //red
				CString str;
				str.Format("%d", cnt);
				cnt++;
				cellorange.put_Text(str);
			}
		}

		cell = table.Cell(22, 1); //种子源A输出曲线
		cellorange = cell.get_Range();
		cell.Select();
		if(OpenClipboard())
		{
			EmptyClipboard();
			CBitmap*  xBMP = new CBitmap();
			//vector<vector<int>> v(4);
			//int a1[10] = {10, 20, 30, 20, 10, 60, 80, 80, 90, 70};
			//int a2[10] = {50, 40, 50, 40, 30, 40, 50, 60, 50, 40};
			//int a3[10] = {20, 10, 20, 10, 20, 10, 30, 20, 40, 30};
			//int a4[10] = {20, 30, 40, 60, 50, 40, 30, 20, 10, 50};
			//for(int i = 0; i < 10; i++)
			//{
			//	v[0].push_back(a1[i]);
			//	v[1].push_back(a2[i]);
			//	v[2].push_back(a3[i]);
			//	v[3].push_back(a4[i]);
			//}
			//xBMP = draw_Bitmap(v); //生成图片指针
			xBMP = draw_Bitmap(g_seedsourceA); //生成图片指针
			SetClipboardData(CF_BITMAP, (xBMP)->m_hObject);
			CloseClipboard();
			sel.Paste();
			CloseClipboard();
		}

		cell = table.Cell(24, 1);	//种子源B输出曲线
		cellorange = cell.get_Range();
		cell.Select();
		if(OpenClipboard())
		{
			EmptyClipboard();
			CBitmap*  xBMP = new CBitmap();
			//vector<vector<int>> v(4);
			//int a1[10] = {10, 20, 30, 20, 10, 60, 80, 80, 90, 70};
			//int a2[10] = {50, 40, 50, 40, 30, 40, 50, 60, 50, 40};
			//int a3[10] = {20, 10, 20, 10, 20, 10, 30, 20, 40, 30};
			//int a4[10] = {20, 30, 40, 60, 50, 40, 30, 20, 10, 50};
			//for(int i = 0; i < 10; i++)
			//{
			//	v[0].push_back(a1[i]);
			//	v[1].push_back(a2[i]);
			//	v[2].push_back(a3[i]);
			//	v[3].push_back(a4[i]);
			//}
			//xBMP = draw_Bitmap(v); //生成图片指针
			xBMP = draw_Bitmap(g_seedsourceB); //生成图片指针
			SetClipboardData(CF_BITMAP, (xBMP)->m_hObject);
			CloseClipboard();
			sel.Paste();
			CloseClipboard();
		}

		cell = table.Cell(26, 1);	//驱动源A输出曲线
		cellorange = cell.get_Range();
		cell.Select();
		if(OpenClipboard())
		{
			EmptyClipboard();
			CBitmap*  xBMP = new CBitmap();
			//vector<vector<int>> v(4);
			//int a1[10] = {10, 20, 30, 20, 10, 60, 80, 80, 90, 70};
			//int a2[10] = {50, 40, 50, 40, 30, 40, 50, 60, 50, 40};
			//int a3[10] = {20, 10, 20, 10, 20, 10, 30, 20, 40, 30};
			//int a4[10] = {20, 30, 40, 60, 50, 40, 30, 20, 10, 50};
			//for(int i = 0; i < 10; i++)
			//{
			//	v[0].push_back(a1[i]);
			//	v[1].push_back(a2[i]);
			//	v[2].push_back(a3[i]);
			//	v[3].push_back(a4[i]);
			//}
			//xBMP = draw_Bitmap(v); //生成图片指针
			xBMP = draw_Bitmap(g_powersupplyA); //生成图片指针
			SetClipboardData(CF_BITMAP, (xBMP)->m_hObject);
			CloseClipboard();
			sel.Paste();
			CloseClipboard();
		}

		cell = table.Cell(28, 1);	//驱动源B输出曲线
		cellorange = cell.get_Range();
		cell.Select();
		if(OpenClipboard())
		{
			EmptyClipboard();
			CBitmap*  xBMP = new CBitmap();
			//vector<vector<int>> v(4);
			//int a1[10] = {10, 20, 30, 20, 10, 60, 80, 80, 90, 70};
			//int a2[10] = {50, 40, 50, 40, 30, 40, 50, 60, 50, 40};
			//int a3[10] = {20, 10, 20, 10, 20, 10, 30, 20, 40, 30};
			//int a4[10] = {20, 30, 40, 60, 50, 40, 30, 20, 10, 50};
			//for(int i = 0; i < 10; i++)
			//{
			//	v[0].push_back(a1[i]);
			//	v[1].push_back(a2[i]);
			//	v[2].push_back(a3[i]);
			//	v[3].push_back(a4[i]);
			//}
			//xBMP = draw_Bitmap(v); //生成图片指针
			xBMP = draw_Bitmap(g_powersupplyB); //生成图片指针
			SetClipboardData(CF_BITMAP, (xBMP)->m_hObject);
			CloseClipboard();
			sel.Paste();
			CloseClipboard();
		}

		cell = table.Cell(30, 1);	//电池电量输出曲线
		cellorange = cell.get_Range();
		cell.Select();
		if(OpenClipboard())
		{
			EmptyClipboard();
			CBitmap*  xBMP = new CBitmap();
			//vector<vector<int>> v(4);
			//int a1[10] = {10, 20, 30, 20, 10, 60, 80, 80, 90, 70};
			//int a2[10] = {50, 40, 50, 40, 30, 40, 50, 60, 50, 40};
			//int a3[10] = {20, 10, 20, 10, 20, 10, 30, 20, 40, 30};
			//int a4[10] = {20, 30, 40, 60, 50, 40, 30, 20, 10, 50};
			//for(int i = 0; i < 10; i++)
			//{
			//	v[0].push_back(a1[i]);
			//	v[1].push_back(a2[i]);
			//	v[2].push_back(a3[i]);
			//	v[3].push_back(a4[i]);
			//}
			//xBMP = draw_Bitmap(v); //生成图片指针
			xBMP = draw_Bitmap(g_soc); //生成图片指针
			SetClipboardData(CF_BITMAP, (xBMP)->m_hObject);
			CloseClipboard();
			sel.Paste();
			CloseClipboard();
		}
		//保存doc文件
		//CString filename = "C:\\Users\\YLL\\Desktop\\" + g_nowTaskNum + ".doc";
		CString filename = g_strExePath + "Tasks\\" + g_nowTaskNum + ".doc";

		COleVariant FileName1(filename);
		doc.SaveAs(FileName1, 
			vOptional,
			vOptional,
			vOptional,
			vOptional,
			vOptional,
			vOptional,
			vOptional,
			vOptional,
			vOptional,
			vOptional,vOptional,vOptional,vOptional,vOptional,vOptional);

		
		wordApp.Quit(vOptional, vOptional, vOptional);
		//释放资源
		CoUninitialize();
		cellorange.ReleaseDispatch();
		cell.ReleaseDispatch();
		ocellFont.ReleaseDispatch();
		tables.ReleaseDispatch();
		sel.ReleaseDispatch();
		wordRange.ReleaseDispatch();
		doc.ReleaseDispatch();
		docs.ReleaseDispatch();
		wordApp.ReleaseDispatch();
		//MessageBox("ok");
	}catch(...)
	{
		AfxMessageBox(_T("打开文件失败！"));
	}
}

CBitmap* CMornSDKDlg::draw_Bitmap(vector<vector<int>>& v)
{
	CDC*  pDC = new CDC();
	CBitmap* xBMP = new CBitmap();
	pDC->CreateCompatibleDC(GetDC());

	//xBMP.CreateCompatibleBitmap(GetDC(),600,500);
	xBMP->LoadBitmapA(IDB_BITMAP1);

	pDC->SelectObject(xBMP);
	pDC->SetBkColor(RGB(255, 255, 0));

	//这里用xDC画图
	// -------------------- 绘制坐标系 -------------------------

	float endPointX = 2 + endX + 2;
	// 坐标y轴
	pDC->MoveTo((int)TransformScale(2), (int)TransformScale(0));
	pDC->LineTo((int)TransformScale(2), (int)TransformScale(4));
	// 坐标x轴
	pDC->MoveTo((int)TransformScale(2), (int)TransformScale(4));
	pDC->LineTo((int)TransformScale(endPointX), (int)TransformScale(4));

	// -------------------- 绘制刻度线 -------------------------
	// 绘制y轴刻度线
	for (float scaleY = 0.6; scaleY <= 4; scaleY += 0.2)
	{
		pDC->MoveTo((int)TransformScale(2), (int)TransformScale(scaleY));
		pDC->LineTo((int)TransformScale((float)2.1), (int)TransformScale(scaleY));
	}
	// 绘制x轴刻度线
	for (float scaleX = 2.2; scaleX < endPointX - 1; scaleX += 0.2)
	{
		pDC->MoveTo((int)TransformScale(scaleX), (int)TransformScale(4));
		pDC->LineTo((int)TransformScale(scaleX), (int)TransformScale(3.9));
	}

	// -------------------- 绘制文字 -------------------------
	CString s;
	// 绘制y轴刻度文字
	for (float ScaleTextY = 3.9, text = 0.0; ScaleTextY >= 0.6; ScaleTextY -= 0.4, text += 0.4)
	{
		s.Format(_T("%.1f"), text);
		pDC->TextOut((int)TransformScale(1.6), (int)TransformScale(ScaleTextY), s);
	}
	// 绘制x轴刻度文字
	for (float ScaleTextX = 2.3; ScaleTextX < endPointX - 1; ScaleTextX += 0.4)
	{
		s.Format(_T("%.1f"), ScaleTextX - 1.9);
		pDC->TextOut((int)TransformScale(ScaleTextX), (int)TransformScale(4.1), s);
	}

	// -------------------- 绘制函数 -------------------------
	int n = v.size();
	for(int i = 0; i < n; i++)
		v[i].insert(v[i].begin(), 0);
	int m = v[0].size();
	//for (int i = 0; i < n; i++) //代表多少组向量
	//{
	//	// 对xy改变坐标和规模再显示点
	//	for(int j = 0; j < m; j++)	//代表每个向量对少个点
	//		pDC->SetPixel((int)TransformCoordinateScaleX(j*0.2), (int)TransformScale(4 - v[i][j]*0.04), 0);
	//}

	for (int i = 0; i < n; i++) //代表多少组向量
	{
		// 对xy改变坐标和规模再显示点
		if(m < 1)
			continue;
		for(int j = 1; j < m; j++)	//代表每个向量对少个点
			//pDC->SetPixel((int)TransformCoordinateScaleX(j*0.2), (int)TransformScale(4 - v[i][j]*0.04), 0);
		{
			pDC->MoveTo((int)TransformCoordinateScaleX((j-1)*0.2), (int)TransformScale(4 - v[i][j-1]*0.02));
			pDC->LineTo((int)TransformCoordinateScaleX(j*0.2), (int)TransformScale(4 - v[i][j]*0.02));
		}
	}

	if (xBMP->m_hObject )
	{
		CImage imgTemp;
		// CImage是MFC中的类。
		imgTemp.Attach(xBMP->operator HBITMAP());
		//imgTemp.Save("C:\\Users\\YLL\\Desktop\\123.bmp");
		return xBMP;
	}
	return xBMP;
}

void CMornSDKDlg::SetScale(int scale)
{
	this->scale = scale;
}

float CMornSDKDlg::TransformScale(float num)
{
	return num * scale;
}

float CMornSDKDlg::TransformCoordinateScaleX(float x)
{
	return TransformScale(x + 2);
}

float CMornSDKDlg::TransformCoordinateScaleY(float y)
{
	return TransformScale(y + 4);
}

void CMornSDKDlg::SetDrawFunction(Function fun)
{
	this->fun = fun;
}

void CMornSDKDlg::SetPlotSin(float startX, float endX, float step)
{
	this->startX = startX;
	this->endX = endX;
	this->step = step;
}