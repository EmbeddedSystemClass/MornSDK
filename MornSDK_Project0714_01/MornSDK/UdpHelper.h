#pragma once

// CUdpHelper ����Ŀ��
#pragma once
#include <afxsock.h>            // MFC �׽�����չ

#define UDP_BUF_RECV 1024*4

class CUdpHelper : public CSocket
{
public:
	CUdpHelper();
	void setValue(int g_isxLocalPort, int g_isxRemotePort, CString cstg_sxServerIP);
	virtual ~CUdpHelper();
	//��ΪUDP��������
	BOOL OpenAsServer();
	//�����ı�
	BOOL SendString(CString szData);
	//���Ͷ�����
	BOOL Send(uchar* pdata, int len);

public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUdpHelper)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CUdpHelper)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	//�������ݻ���
	uchar* m_pBufRecv;
	//���ض˿�
	UINT m_iPortLocal;
	//Զ�̶˿�
	UINT m_iPortRemote;
	//Զ�̷�����IP
	CString m_serverIP;

protected:
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);

};


