
// MornSDK.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "General.h"
#include "MornSDK.h"
#include "MornSDKDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMornSDKApp

BEGIN_MESSAGE_MAP(CMornSDKApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMornSDKApp 构造

CMornSDKApp::CMornSDKApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMornSDKApp 对象

CMornSDKApp theApp;


// CMornSDKApp 初始化

BOOL CMornSDKApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	//初始化套接字YWJ
	if (!AfxSocketInit())
	{
		AfxMessageBox("Windows 套接字初始化失败。");
		return FALSE;
	}

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//程序自定义初始化
	//! 得到程序路径
	TCHAR szPath[MAX_PATH]; 
	GetModuleFileName(NULL,szPath,MAX_PATH);
	g_strExePath = szPath;
	g_strExePath = g_strExePath.Left(g_strExePath.ReverseFind('\\')+1);

	//读取配置值
	g_Config.SetINIFileName(g_strExePath + "config.ini");
	g_CfgTask.SetINIFileName(g_strExePath + "cfgTask.ini");
	g_Log.Init();
	g_Log.WriteLog(LOG_INF, "================ MornSDK v%s 启动... =============", g_Version);
	g_strServerIP = g_Config.GetKeyValue("ServerIP","Network");
	g_iRemotePort = _ttoi(g_Config.GetKeyValue("RemotePort","Network"));
	g_iLocalPort = _ttoi(g_Config.GetKeyValue("LocalPort","Network"));

	g_sxAServerIP = g_Config.GetKeyValue("sxAServerIP","Network");
	g_isxARemotePort = _ttoi(g_Config.GetKeyValue("sxARemotePort","Network"));
	g_isxALocalPort = _ttoi(g_Config.GetKeyValue("sxALocalPort","Network"));

	g_sxBServerIP = g_Config.GetKeyValue("sxBServerIP","Network");
	g_isxBRemotePort = _ttoi(g_Config.GetKeyValue("sxBRemotePort","Network"));
	g_isxBLocalPort = _ttoi(g_Config.GetKeyValue("sxBLocalPort","Network"));
	
	g_WsdServerIP = g_Config.GetKeyValue("wsdClientIP","Network");
	g_WsdPort = _ttoi(g_Config.GetKeyValue("wsdPort","Network"));

	g_iComPortToUp = _ttoi(g_Config.GetKeyValue("ComPortToUp","COM"));
	g_iComPortToDown = _ttoi(g_Config.GetKeyValue("ComPortToDown","COM"));

	g_nowTaskNum = g_CfgTask.GetKeyValue("nowNum","nowTaskNum");
	g_delay = _ttoi(g_CfgTask.GetKeyValue("D","Delay"));
	g_Log.WriteLog(LOG_DBG, "服务器IP：%s, 端口Port: %d", g_strServerIP, g_iRemotePort);
	g_Log.WriteLog(LOG_DBG, "时序控制卡A IP：%s, 端口Port: %d", g_sxAServerIP, g_isxARemotePort);
	g_Log.WriteLog(LOG_DBG, "时序控制卡B IP：%s, 端口Port: %d", g_sxBServerIP, g_isxBRemotePort);
	g_Log.WriteLog(LOG_DBG, "温湿度传感器 IP：%s, 端口Port: %d", g_WsdServerIP, g_WsdPort);
	g_Log.WriteLog(LOG_DBG, "串口号： %d", g_iComPortToUp);
	g_Log.WriteLog(LOG_INF, "程序初始化成功");
	//初始化串口
	g_pSP = new CSerialPort();

	//初始化串口
	g_pCnCommToUp = new cnComm();
	g_pCnCommToDown = new cnComm();
	//YWJ----------------
	//建立IO卡对象
	g_pIoCard = new CMIC3780();
	//YWJ----------------
		 

	CMornSDKDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
	g_Log.WriteLog(LOG_INF, "--------------MornSDK v%s 程序退出 -------------", g_Version);
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

