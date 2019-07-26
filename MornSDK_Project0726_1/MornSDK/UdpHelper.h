#pragma once

// CUdpHelper 命令目标
#pragma once
#include <afxsock.h>            // MFC 套接字扩展

#define UDP_BUF_RECV 1024*4

class CUdpHelper : public CSocket
{
public:
	CUdpHelper();
	void setValue(int g_isxLocalPort, int g_isxRemotePort, CString cstg_sxServerIP);
	virtual ~CUdpHelper();
	//作为UDP服务器打开
	BOOL OpenAsServer();
	//发送文本
	BOOL SendString(CString szData);
	//发送二进制
	BOOL Send(uchar* pdata, int len);

public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUdpHelper)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CUdpHelper)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	//接收数据缓存
	uchar* m_pBufRecv;
	//本地端口
	UINT m_iPortLocal;
	//远程端口
	UINT m_iPortRemote;
	//远程服务器IP
	CString m_serverIP;

protected:
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);

};


