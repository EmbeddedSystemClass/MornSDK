#pragma once

#include "General.h"
#include "afxwin.h"
// CDlgTab3 �Ի���

class CDlgTab3 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTab3)

public:
	CDlgTab3(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTab3();

	//��ʼ��ҳ�溯��
	void InitPage();

// �Ի�������
	enum { IDD = IDD_Tab3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	long hOphirLMDevice, hOphirLMchannel;	//���ʼƾ����ͨ��
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void IntialCSocket();	//��ʼ����ʪ�ȼ�tcp
	CSocket aSocket;		//��ʪ�ȼƶ���
	void InitialOphirLM();	//��ʼ�����ʼ�
	afx_msg void OnBnClickedButton1();
};
