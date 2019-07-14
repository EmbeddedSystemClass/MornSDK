#pragma once
#include "afxwin.h"
#include "BtnST.h"
#include "grid/GridCtrl.h"
#include "CanManager.h"
#include "General.h"

#define WM_DISPYAY_RECEIVE_BATETTY (WM_USER+200)
#define WM_CHANGE_LED    (WM_USER+300)
// CDlgTab1 对话框
class CDlgTab1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTab1)

public:
	CDlgTab1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTab1();

	//初始化页面函数
	void InitPage();

// 对话框数据
	enum { IDD = IDD_Tab1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_iIndex;
	CCanManager* m_pCanManager;
	CCanManager* m_pCanManager2;
	CCanManager* m_pCanManager3;
	CCanManager* m_pCanManager4;
	CGridCtrl m_pCurrentSet;
	CGridCtrl m_pLightState;
	//CGridCtrl m_pLight_B;
	//CGridCtrl m_pBat1;
	CGridCtrl m_pSeedSourceFB;
	CGridCtrl m_pPower1;
	CGridCtrl m_pPower2;
	CGridCtrl m_pPower_Recv1;
	CGridCtrl m_pPower_Recv2;
	//CGridCtrl m_pBattery;
	CGridCtrl m_pReceiveBattery;

	CButtonST m_led_SETCURRENT;
	CButtonST m_led_outputSETCURRENT;
	CButtonST m_led_stopCURRENT;
	CButtonST m_led_resetCURRENT;

	CButtonST m_led_SETCURRENT2;
	CButtonST m_led_outputSETCURRENT2;
	CButtonST m_led_stopCURRENT2;
	CButtonST m_led_resetCURRENT2;

	CButtonST m_led_setparPOWER;
	CButtonST m_led_quirePOWER;
	CButtonST m_led_outputPOWER;
	CButtonST m_led_stopoutputPOWER;
	CButtonST m_led_resetPOWER;

	CButtonST m_led_setparPOWER2;
	CButtonST m_led_quirePOWER2;
	CButtonST m_led_outputPOWER2;
	CButtonST m_led_stopoutputPOWER2;
	CButtonST m_led_resetPOWER2;

	//电池充放电
	CButtonST m_led_batteryCharge;
	CButtonST m_led_stopbatteryCharge;
	CButtonST m_led_batteryDisCharge;
	CButtonST m_led_stopbatteryDisCharge;

	afx_msg void OnBnClickedLedSetcurrent();
	afx_msg void OnBtnsetoutputcurrent();
	afx_msg void OnBtnoutputcurrent();
	afx_msg void OnBtnstopoutputcurrent();
	afx_msg void OnBtnresetcurrent();

	CButtonST button1;
	afx_msg void OnBtnsetparpower();
	afx_msg void OnBnClickedBtnquireparpower();
	afx_msg void OnBnClickedBtnoutputpower();
	afx_msg void OnBnClickedBtnstopoutputpower();
	afx_msg void OnBnClickedBtnresetpower();

	afx_msg void OnBnClickedBtnsetoutputcurrent2();
	afx_msg void OnBnClickedBtnoutputcurrent2();
	afx_msg void OnBnClickedBtnstopoutputcurrent2();
	afx_msg void OnBnClickedBtnresetcurrent2();

	afx_msg void OnBnClickedBtnsetparpower2();
	afx_msg void OnBnClickedBtnquireparpower2();
	afx_msg void OnBnClickedBtnoutputpower2();
	afx_msg void OnBnClickedBtnstopoutputpower2();
	afx_msg void OnBnClickedBtnresetpower2();
	afx_msg void OnCbnSelchangeCombo1();

	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnbatterycharge();
	afx_msg void OnBnClickedBtnstopbatterycharge();
	afx_msg void OnBnClickedBtnbatterydischarge();
	afx_msg void OnBnClickedBtnstopbatterydischarge();
	afx_msg void OnBnClickedBtnStatusquirypower();
	afx_msg void OnBnClickedBtnAllchoose();
	afx_msg void OnBnClickedBtnAllnotchoose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT display_receive_battery(WPARAM wPARAM, LPARAM lPARAM);
	afx_msg LRESULT change_led(WPARAM wPARAM, LPARAM lPARAM);
	afx_msg void OnBnClickedBtnAllchoose2();
	afx_msg void OnBnClickedBtnAllnotchoose2();
	afx_msg void OnBnClickedBtnAllchoose3();
	afx_msg void OnBnClickedBtnAllnotchoose3();
};
