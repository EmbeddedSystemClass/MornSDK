// UdpHelper.cpp : 实现文件
//

#include "General.h"
#include "MornSDK.h"
#include "MornSDKDlg.h"

// CUdpHelper
//构造函数
CUdpHelper::CUdpHelper()
{
	m_pBufRecv = new uchar[UDP_BUF_RECV + 1];
	m_iPortLocal = g_iLocalPort;
	m_iPortRemote = g_iRemotePort;
	m_serverIP = g_strServerIP;
}

void CUdpHelper::setValue(int localPort, int remotePort, CString ip)
{
	m_pBufRecv = new uchar[UDP_BUF_RECV + 1];
	m_iPortLocal = localPort;
	m_iPortRemote = remotePort;
	m_serverIP = ip;
}

//析构函数
CUdpHelper::~CUdpHelper()
{
	if (m_pBufRecv != NULL)
	{
		delete m_pBufRecv;
		m_pBufRecv = NULL;
	}
}

#if 0
BEGIN_MESSAGE_MAP(CUdpHelper, CSocket)
	//{{AFX_MSG_MAP(CUdpHelper)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

//作为UDP服务器打开
BOOL CUdpHelper::OpenAsServer()
{
	try
	{
		this->Create(m_iPortLocal, SOCK_DGRAM);
		this->Bind(m_iPortLocal);
		g_Log.WriteLog(LOG_INF, _T("UDP端口打开成功。"));
		//::MessageBox(NULL, _T("OpenAsServer Success"), _T("加载参数反馈"), MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("UDP端口打开时出现异常。"));
		return FALSE;
	}
}

//发送二进制数据
BOOL CUdpHelper::Send(uchar* pdata, int len)
{
	try
	{
		//int iLen = szData.GetLength();
		int iSend = this->SendTo(pdata, len, m_iPortRemote, m_serverIP);
		if(SOCKET_ERROR == iSend)
		{
			g_Log.WriteLog(LOG_ERR, _T("UDP发送二进制数据失败。"));
			return FALSE;
		}
		//g_Log.WriteLog(LOG_INF, _T("UDP端口发送二进制数据成功。"));
		//::MessageBox(NULL, szData, _T("加载参数反馈"), MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("UDP发送二进制数据失败。"));
		return FALSE;
	}
}

//发送文本
BOOL CUdpHelper::SendString(CString szData)
{
	try
	{
		int iLen = szData.GetLength();
		int iSend = this->SendTo(LPCTSTR(szData), iLen, m_iPortRemote, m_serverIP);
		if(SOCKET_ERROR == iSend)
		{
			g_Log.WriteLog(LOG_ERR, _T("UDP发送数据失败。"));
			return FALSE;
		}
		g_Log.WriteLog(LOG_INF, _T("UDP端口发送数据成功。"));
		//::MessageBox(NULL, szData, _T("加载参数反馈"), MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("UDP发送数据失败。"));
		return FALSE;
	}
}

//下列的Socket事件，设置其与对话框相关联，重载代码如下：
void CUdpHelper::OnClose(int nErrorCode) 
{
	this->Close();
	g_Log.WriteLog(LOG_INF, _T("UDP端口关闭。"));
}

void CUdpHelper::OnReceive(int nErrorCode) 
{
	//清空接收缓存
	memset(m_pBufRecv, 0, UDP_BUF_RECV+1);
	int		iBufSize = UDP_BUF_RECV;
	int		iRecvd;
	CString	strRecvd;	//接收的数据信息字符串

	// 接收信息，并返回接收到的字节数
	iRecvd = this->Receive(m_pBufRecv, iBufSize);
	// 判断是否有错误发生
	if (iRecvd == SOCKET_ERROR)
	{
		//g_Log.WriteLog(LOG_ERR, _T("UDP接收数据时出现错误。"));
	}
	else
	{
		// 利用NULL来截去接收缓存区中多余的字符
		m_pBufRecv[iRecvd] = NULL;
		//g_Log.WriteLog(LOG_INF, _T("UDP端口接收到数据。"));
		if(m_iPortRemote == g_iRemotePort)
			g_pMainDlg->OnUdpReceived(m_pBufRecv, iRecvd);
		if(m_iPortRemote == g_isxARemotePort)
		{
			//CString str = strToHex(m_pBufRecv, iRecvd);
			CString str, tem;
			for(int i= 0; i < iRecvd; i++)
			{
				if( i >= 2 && i <=3)
				{
					CString tmp;
					BYTE k = m_pBufRecv[i];
					for(int j = 0; j < 8; j++)
					{
						//tmp += k & 1 ? "1" : "0";
						tmp.Insert(0, k & 1 ? "1" : "0");
						k = k>>1;
					}
					str += tmp + " ";
				}
				else
				{
					tem.Format("%02X ", m_pBufRecv[i]);
					str += tem;
				}
				
			}
			if(str[3] == '2' && str[4] == '2')
				g_pMainDlg->m_Page3.GetDlgItem(IDC_EDIT_sxA_state)->SetWindowText(str.Mid(6, 17));
			else
				g_pMainDlg->m_Page3.GetDlgItem(IDC_EDIT_SHIXURECV)->SetWindowText(str);
			g_pMainDlg->OnUdpReceivedFromSXA(m_pBufRecv, iRecvd);
		}
		if(m_iPortRemote == g_isxBRemotePort)
		{
			//CString str = strToHex(m_pBufRecv, iRecvd);
			CString str, tem;
			for(int i= 0; i < iRecvd; i++)
			{
				if( i >= 2 && i <=3)
				{
					CString tmp;
					BYTE k = m_pBufRecv[i];
					for(int j = 0; j < 8; j++)
					{
						//tmp += k & 1 ? "1" : "0";
						tmp.Insert(0, k & 1 ? "1" : "0");
						k = k>>1;
					}
					str += tmp + " ";
				}
				else
				{
					tem.Format("%02X ", m_pBufRecv[i]);
					str += tem;
				}

			}
			if(str[3] == '2' && str[4] == '2')
				g_pMainDlg->m_Page3.GetDlgItem(IDC_EDIT_sxB_state)->SetWindowText(str.Mid(6, 17));
			else
				g_pMainDlg->m_Page3.GetDlgItem(IDC_EDIT_SHIXURECV2)->SetWindowText(str);
			g_pMainDlg->OnUdpReceivedFromSXB(m_pBufRecv, iRecvd);
		}
	}
}



// CUdpHelper 成员函数
