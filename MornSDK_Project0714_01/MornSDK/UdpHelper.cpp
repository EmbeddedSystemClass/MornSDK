// UdpHelper.cpp : ʵ���ļ�
//

#include "General.h"
#include "MornSDK.h"
#include "MornSDKDlg.h"

// CUdpHelper
//���캯��
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

//��������
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

//��ΪUDP��������
BOOL CUdpHelper::OpenAsServer()
{
	try
	{
		this->Create(m_iPortLocal, SOCK_DGRAM);
		this->Bind(m_iPortLocal);
		g_Log.WriteLog(LOG_INF, _T("UDP�˿ڴ򿪳ɹ���"));
		//::MessageBox(NULL, _T("OpenAsServer Success"), _T("���ز�������"), MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("UDP�˿ڴ�ʱ�����쳣��"));
		return FALSE;
	}
}

//���Ͷ���������
BOOL CUdpHelper::Send(uchar* pdata, int len)
{
	try
	{
		//int iLen = szData.GetLength();
		int iSend = this->SendTo(pdata, len, m_iPortRemote, m_serverIP);
		if(SOCKET_ERROR == iSend)
		{
			g_Log.WriteLog(LOG_ERR, _T("UDP���Ͷ���������ʧ�ܡ�"));
			return FALSE;
		}
		//g_Log.WriteLog(LOG_INF, _T("UDP�˿ڷ��Ͷ��������ݳɹ���"));
		//::MessageBox(NULL, szData, _T("���ز�������"), MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("UDP���Ͷ���������ʧ�ܡ�"));
		return FALSE;
	}
}

//�����ı�
BOOL CUdpHelper::SendString(CString szData)
{
	try
	{
		int iLen = szData.GetLength();
		int iSend = this->SendTo(LPCTSTR(szData), iLen, m_iPortRemote, m_serverIP);
		if(SOCKET_ERROR == iSend)
		{
			g_Log.WriteLog(LOG_ERR, _T("UDP��������ʧ�ܡ�"));
			return FALSE;
		}
		g_Log.WriteLog(LOG_INF, _T("UDP�˿ڷ������ݳɹ���"));
		//::MessageBox(NULL, szData, _T("���ز�������"), MB_OK|MB_ICONINFORMATION);
		return TRUE;
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_ERR, _T("UDP��������ʧ�ܡ�"));
		return FALSE;
	}
}

//���е�Socket�¼�����������Ի�������������ش������£�
void CUdpHelper::OnClose(int nErrorCode) 
{
	this->Close();
	g_Log.WriteLog(LOG_INF, _T("UDP�˿ڹرա�"));
}

void CUdpHelper::OnReceive(int nErrorCode) 
{
	//��ս��ջ���
	memset(m_pBufRecv, 0, UDP_BUF_RECV+1);
	int		iBufSize = UDP_BUF_RECV;
	int		iRecvd;
	CString	strRecvd;	//���յ�������Ϣ�ַ���

	// ������Ϣ�������ؽ��յ����ֽ���
	iRecvd = this->Receive(m_pBufRecv, iBufSize);
	// �ж��Ƿ��д�����
	if (iRecvd == SOCKET_ERROR)
	{
		//g_Log.WriteLog(LOG_ERR, _T("UDP��������ʱ���ִ���"));
	}
	else
	{
		// ����NULL����ȥ���ջ������ж�����ַ�
		m_pBufRecv[iRecvd] = NULL;
		//g_Log.WriteLog(LOG_INF, _T("UDP�˿ڽ��յ����ݡ�"));
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



// CUdpHelper ��Ա����
