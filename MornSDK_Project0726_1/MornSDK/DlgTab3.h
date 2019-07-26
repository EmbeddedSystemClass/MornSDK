#pragma once

#include "General.h"
#include "afxwin.h"
// CDlgTab3 对话框

class CDlgTab3 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTab3)

public:
	CDlgTab3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTab3();

	//初始化页面函数
	void InitPage();

// 对话框数据
	enum { IDD = IDD_Tab3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnComsend();	
	afx_msg void OnBnClickedBtnUdpsend();

	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedButtonSetA();
	afx_msg void OnBnClickedButtonSetB();
	afx_msg void OnBnClickedButtonResetA();
	afx_msg void OnBnClickedButtonResetB();
	afx_msg void OnBnClickedButtonOutputA();
	afx_msg void OnBnClickedButtonOutputB();
	afx_msg void OnBnClickedButtonStopA();
	afx_msg void OnBnClickedButtonStopB();
	afx_msg void OnBnClickedButtonCxA();
	afx_msg void OnBnClickedButtonCxB();
	afx_msg void OnBnClickedButtonCxzA();
	afx_msg void OnBnClickedButtonCxztB();
	afx_msg void OnBnClickedBtnSetvalue();
	afx_msg void OnBnClickedBtnFeedback();

	struct CoInitializer
	{
		CoInitializer() { CoInitialize(nullptr); }
		~CoInitializer() { CoUninitialize(); }
	};
	CoInitializer initializer;// must call for COM initialization and deinitialization
	OphirLMMeasurement OphirLM;
	std::vector<std::wstring> serialNumbers, options;
	std::wstring info ,headSN, headType, headName, version;
	std::wstring deviceName, romVersion, serialNumber;
	long hOphirLMDevice, hOphirLMchannel;	//功率计句柄和通道
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void IntialCSocket();	//初始化温湿度计tcp
	CSocket aSocket;		//温湿度计对象
	void InitialOphirLM();	//初始化功率计
	afx_msg void OnBnClickedButton1();
};
