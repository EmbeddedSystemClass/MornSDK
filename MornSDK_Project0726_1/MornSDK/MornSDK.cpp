
// MornSDK.cpp : ����Ӧ�ó��������Ϊ��
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


// CMornSDKApp ����

CMornSDKApp::CMornSDKApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMornSDKApp ����

CMornSDKApp theApp;


// CMornSDKApp ��ʼ��

BOOL CMornSDKApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	//��ʼ���׽���YWJ
	if (!AfxSocketInit())
	{
		AfxMessageBox("Windows �׽��ֳ�ʼ��ʧ�ܡ�");
		return FALSE;
	}

	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	//�����Զ����ʼ��
	//! �õ�����·��
	TCHAR szPath[MAX_PATH]; 
	GetModuleFileName(NULL,szPath,MAX_PATH);
	g_strExePath = szPath;
	g_strExePath = g_strExePath.Left(g_strExePath.ReverseFind('\\')+1);

	//��ȡ����ֵ
	g_Config.SetINIFileName(g_strExePath + "config.ini");
	g_CfgTask.SetINIFileName(g_strExePath + "cfgTask.ini");
	g_Log.Init();
	g_Log.WriteLog(LOG_INF, "================ MornSDK v%s ����... =============", g_Version);
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
	g_Log.WriteLog(LOG_DBG, "������IP��%s, �˿�Port: %d", g_strServerIP, g_iRemotePort);
	g_Log.WriteLog(LOG_DBG, "ʱ����ƿ�A IP��%s, �˿�Port: %d", g_sxAServerIP, g_isxARemotePort);
	g_Log.WriteLog(LOG_DBG, "ʱ����ƿ�B IP��%s, �˿�Port: %d", g_sxBServerIP, g_isxBRemotePort);
	g_Log.WriteLog(LOG_DBG, "��ʪ�ȴ����� IP��%s, �˿�Port: %d", g_WsdServerIP, g_WsdPort);
	g_Log.WriteLog(LOG_DBG, "���ںţ� %d", g_iComPortToUp);
	g_Log.WriteLog(LOG_INF, "�����ʼ���ɹ�");
	//��ʼ������
	g_pSP = new CSerialPort();

	//��ʼ������
	g_pCnCommToUp = new cnComm();
	g_pCnCommToDown = new cnComm();
	//YWJ----------------
	//����IO������
	g_pIoCard = new CMIC3780();
	//YWJ----------------
		 

	CMornSDKDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
	g_Log.WriteLog(LOG_INF, "--------------MornSDK v%s �����˳� -------------", g_Version);
	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

