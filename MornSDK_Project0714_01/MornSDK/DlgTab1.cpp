// DlgTab1.cpp : 实现文件
//

#include "stdafx.h"
#include "MornSDK.h"
#include "DlgTab1.h"
#include "CanManager.h"
#include "General.h"
#include "MornSDKDlg.h"


// CDlgTab1 对话框
CFont m_editFont;

IMPLEMENT_DYNAMIC(CDlgTab1, CDialogEx)

CDlgTab1::CDlgTab1(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTab1::IDD, pParent)
{
	m_iIndex = 0;
	m_pCanManager = g_pCanManager3;
}

CDlgTab1::~CDlgTab1()
{
}

void CDlgTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_GirdCurrentSet, m_pCurrentSet);
	DDX_GridControl(pDX, IDC_GirdLight_A, m_pLightState);
//	DDX_GridControl(pDX, IDC_GirdLight_B, m_pLight_B);
	//DDX_GridControl(pDX, IDC_GirdBat1, m_pBat1);
	DDX_GridControl(pDX, IDC_GirdSeedSource1, m_pSeedSourceFB);
	DDX_GridControl(pDX, IDC_GirdPower1, m_pPower1);
	DDX_GridControl(pDX, IDC_GirdPower2, m_pPower2);
	DDX_GridControl(pDX, IDC_GirdPower1_Recv, m_pPower_Recv1);
	DDX_GridControl(pDX, IDC_GirdPower2_Recv, m_pPower_Recv2);
	//DDX_GridControl(pDX, IDC_GirdSetBattery, m_pBattery);
	DDX_GridControl(pDX, IDC_GirdReceiveBattery, m_pReceiveBattery);

	DDX_Control(pDX, IDC_LED_SETCURRENT, m_led_SETCURRENT);
	DDX_Control(pDX, IDC_LED_outputCURRENT, m_led_outputSETCURRENT);
	DDX_Control(pDX, IDC_LED_stopCURRENT, m_led_stopCURRENT);
	DDX_Control(pDX, IDC_LED_resetCURRENT, m_led_resetCURRENT);

	DDX_Control(pDX, IDC_LED_SETCURRENT2, m_led_SETCURRENT2);
	DDX_Control(pDX, IDC_LED_outputCURRENT2, m_led_outputSETCURRENT2);
	DDX_Control(pDX, IDC_LED_stopCURRENT2, m_led_stopCURRENT2);
	DDX_Control(pDX, IDC_LED_resetCURRENT2, m_led_resetCURRENT2);

	DDX_Control(pDX, IDC_led_setPARPOWER, m_led_setparPOWER);
	DDX_Control(pDX, IDC_led_quirePOWER, m_led_quirePOWER);
	DDX_Control(pDX, IDC_led_outputPOWER, m_led_outputPOWER);
	DDX_Control(pDX, IDC_led_stopoutputPOWER, m_led_stopoutputPOWER);
	DDX_Control(pDX, IDC_led_resetPOWER, m_led_resetPOWER);

	DDX_Control(pDX, IDC_led_setPARPOWER2, m_led_setparPOWER2);
	DDX_Control(pDX, IDC_led_quirePOWER2, m_led_quirePOWER2);
	DDX_Control(pDX, IDC_led_outputPOWER2, m_led_outputPOWER2);
	DDX_Control(pDX, IDC_led_stopoutputPOWER2, m_led_stopoutputPOWER2);
	DDX_Control(pDX, IDC_led_resetPOWER2, m_led_resetPOWER2);

	DDX_Control(pDX, IDC_LED_battery_charge, m_led_batteryCharge);
	DDX_Control(pDX, IDC_LED_stopbattery_charge, m_led_stopbatteryCharge);
	DDX_Control(pDX, IDC_LED_battery_discharge, m_led_batteryDisCharge);
	DDX_Control(pDX, IDC_LED_stopbattery_discharge, m_led_stopbatteryDisCharge);
}

BEGIN_MESSAGE_MAP(CDlgTab1, CDialogEx)
	ON_BN_CLICKED(IDC_LED_SETCURRENT, &CDlgTab1::OnBnClickedLedSetcurrent)
	ON_BN_CLICKED(IDC_BTN_set_output_current, &CDlgTab1::OnBtnsetoutputcurrent)
	ON_BN_CLICKED(IDC_BTN_output_current, &CDlgTab1::OnBtnoutputcurrent)
	ON_BN_CLICKED(IDC_BTN_stop_output_current, &CDlgTab1::OnBtnstopoutputcurrent)
	ON_BN_CLICKED(IDC_BTN_reset_current, &CDlgTab1::OnBtnresetcurrent)
	ON_BN_CLICKED(IDC_BTN_set_par_power, &CDlgTab1::OnBtnsetparpower)
	ON_BN_CLICKED(IDC_BTN_quire_par_power, &CDlgTab1::OnBnClickedBtnquireparpower)
	ON_BN_CLICKED(IDC_BTN_output_power, &CDlgTab1::OnBnClickedBtnoutputpower)
	ON_BN_CLICKED(IDC_BTN_stopoutput_power, &CDlgTab1::OnBnClickedBtnstopoutputpower)
	ON_BN_CLICKED(IDC_BTN_reset_power, &CDlgTab1::OnBnClickedBtnresetpower)
	ON_BN_CLICKED(IDC_BTN_set_output_current2, &CDlgTab1::OnBnClickedBtnsetoutputcurrent2)
	ON_BN_CLICKED(IDC_BTN_output_current2, &CDlgTab1::OnBnClickedBtnoutputcurrent2)
	ON_BN_CLICKED(IDC_BTN_stop_output_current2, &CDlgTab1::OnBnClickedBtnstopoutputcurrent2)
	ON_BN_CLICKED(IDC_BTN_reset_current2, &CDlgTab1::OnBnClickedBtnresetcurrent2)

	ON_NOTIFY(GVN_SELCHANGED, IDC_GirdPower1, OnClick)
	ON_BN_CLICKED(IDC_BTN_set_par_power2, &CDlgTab1::OnBnClickedBtnsetparpower2)
	ON_BN_CLICKED(IDC_BTN_quire_par_power2, &CDlgTab1::OnBnClickedBtnquireparpower2)
	ON_BN_CLICKED(IDC_BTN_output_power2, &CDlgTab1::OnBnClickedBtnoutputpower2)
	ON_BN_CLICKED(IDC_BTN_stopoutput_power2, &CDlgTab1::OnBnClickedBtnstopoutputpower2)
	ON_BN_CLICKED(IDC_BTN_reset_power2, &CDlgTab1::OnBnClickedBtnresetpower2)
	//ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgTab1::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BTN_battery_charge, &CDlgTab1::OnBnClickedBtnbatterycharge)
	ON_BN_CLICKED(IDC_BTN_stopbattery_charge, &CDlgTab1::OnBnClickedBtnstopbatterycharge)
	ON_BN_CLICKED(IDC_BTN_battery_discharge, &CDlgTab1::OnBnClickedBtnbatterydischarge)
	ON_BN_CLICKED(IDC_BTN_stopbattery_discharge, &CDlgTab1::OnBnClickedBtnstopbatterydischarge)
	ON_BN_CLICKED(IDC_BTN_ALLCHOOSE, &CDlgTab1::OnBnClickedBtnAllchoose)
	ON_BN_CLICKED(IDC_BTN_ALLNOTCHOOSE, &CDlgTab1::OnBnClickedBtnAllnotchoose)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DISPYAY_RECEIVE_BATETTY, display_receive_battery)
	ON_MESSAGE(WM_CHANGE_LED, change_led)
	ON_BN_CLICKED(IDC_BTN_ALLCHOOSE2, &CDlgTab1::OnBnClickedBtnAllchoose2)
	ON_BN_CLICKED(IDC_BTN_ALLNOTCHOOSE2, &CDlgTab1::OnBnClickedBtnAllnotchoose2)
	ON_BN_CLICKED(IDC_BTN_ALLCHOOSE3, &CDlgTab1::OnBnClickedBtnAllchoose3)
	ON_BN_CLICKED(IDC_BTN_ALLNOTCHOOSE3, &CDlgTab1::OnBnClickedBtnAllnotchoose3)
END_MESSAGE_MAP()

//初始化页面函数
void CDlgTab1::InitPage()
{
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(_T("整机"));    //添加字符串
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(_T("放大级1"));
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(_T("放大级2"));
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(_T("放大级3"));
	//((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(0);   //默认选择第一个

	//种子源信号灯
	m_led_SETCURRENT.SetIcon(IDI_ICON_RED);
	m_led_SETCURRENT.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_outputSETCURRENT.SetIcon(IDI_ICON_RED);
	m_led_outputSETCURRENT.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_stopCURRENT.SetIcon(IDI_ICON_RED);
	m_led_stopCURRENT.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_resetCURRENT.SetIcon(IDI_ICON_RED);
	m_led_resetCURRENT.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_SETCURRENT2.SetIcon(IDI_ICON_RED);
	m_led_SETCURRENT2.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_outputSETCURRENT2.SetIcon(IDI_ICON_RED);
	m_led_outputSETCURRENT2.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_stopCURRENT2.SetIcon(IDI_ICON_RED);
	m_led_stopCURRENT2.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_resetCURRENT2.SetIcon(IDI_ICON_RED);
	m_led_resetCURRENT2.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	//驱动电源信号灯
	m_led_setparPOWER.SetIcon(IDI_ICON_RED);
	m_led_setparPOWER.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_quirePOWER.SetIcon(IDI_ICON_RED);
	m_led_quirePOWER.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_outputPOWER.SetIcon(IDI_ICON_RED);
	m_led_outputPOWER.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_stopoutputPOWER.SetIcon(IDI_ICON_RED);
	m_led_stopoutputPOWER.DrawBorder(FALSE);
	//m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_resetPOWER.SetIcon(IDI_ICON_RED);
	m_led_resetPOWER.DrawBorder(FALSE);
	////m_led_SETCURRENT.DrawTransparent(TRUE);

	m_led_setparPOWER2.SetIcon(IDI_ICON_RED);
	m_led_setparPOWER2.DrawBorder(FALSE);
	//m_led_SETCURRENT2.DrawTransparent(TRUE);

	m_led_quirePOWER2.SetIcon(IDI_ICON_RED);
	m_led_quirePOWER2.DrawBorder(FALSE);
	//m_led_SETCURRENT2.DrawTransparent(TRUE);

	m_led_outputPOWER2.SetIcon(IDI_ICON_RED);
	m_led_outputPOWER2.DrawBorder(FALSE);
	//m_led_SETCURRENT2.DrawTransparent(TRUE);

	m_led_stopoutputPOWER2.SetIcon(IDI_ICON_RED);
	m_led_stopoutputPOWER2.DrawBorder(FALSE);
	//m_led_SETCURRENT2.DrawTransparent(TRUE);

	m_led_resetPOWER2.SetIcon(IDI_ICON_RED);
	m_led_resetPOWER2.DrawBorder(FALSE);
	////m_led_SETCURRENT2.DrawTransparent(TRUE);

	//电池充放电
	m_led_batteryCharge.SetIcon(IDI_ICON_RED);
	m_led_batteryCharge.DrawBorder(FALSE);
	//m_led_SETCURRENT2.DrawTransparent(TRUE);

	m_led_stopbatteryCharge.SetIcon(IDI_ICON_RED);
	m_led_stopbatteryCharge.DrawBorder(FALSE);
	//m_led_SETCURRENT2.DrawTransparent(TRUE);

	m_led_batteryDisCharge.SetIcon(IDI_ICON_RED);
	m_led_batteryDisCharge.DrawBorder(FALSE);
	//m_led_SETCURRENT2.DrawTransparent(TRUE);

	m_led_stopbatteryDisCharge.SetIcon(IDI_ICON_RED);
	m_led_stopbatteryDisCharge.DrawBorder(FALSE);
	////m_led_SETCURRENT2.DrawTransparent(TRUE);

	//驱动电源设置
	m_pCurrentSet.SetColumnCount(4);    // 设定总共的列数
	m_pCurrentSet.SetColumnWidth(0, 60);
	m_pCurrentSet.SetColumnWidth(1, 60);
	m_pCurrentSet.SetColumnWidth(2, 60);
	m_pCurrentSet.SetColumnWidth(3, 60);
	m_pCurrentSet.SetRowCount(3);       // 设定总共的行数S
	m_pCurrentSet.SetRowHeight(0,30);
	m_pCurrentSet.SetFixedRowCount(1);  // 设定固定行数
	m_pCurrentSet.SetItemText(0,0,"振荡级"); // SetItemText 用来设定某行某列的具体文本数值
	m_pCurrentSet.SetItemText(0,1,"放大级1");
	m_pCurrentSet.SetItemText(0,2,"放大级2");
	m_pCurrentSet.SetItemText(0,3,"放大级3");
	m_pCurrentSet.ShowScrollBar(SB_BOTH, FALSE);

	////种子源反馈状态
	m_pSeedSourceFB.SetColumnCount(8);    // 设定总共的列数
	for (int i = 0; i < 8; i++)
	{
		m_pSeedSourceFB.SetColumnWidth(i, 80);
	}
	m_pSeedSourceFB.SetRowCount(3);       // 设定总共的行数S
	m_pSeedSourceFB.SetRowHeight(0,30);
	m_pSeedSourceFB.SetFixedRowCount(1);  // 设定固定行数

	m_pSeedSourceFB.SetItemText(0,0,"振荡级电流"); // SetItemText 用来设定某行某列的具体文本数值
	m_pSeedSourceFB.SetItemText(0,1,"振荡级状态");
	m_pSeedSourceFB.SetItemText(0,2,"放大级1电流");
	m_pSeedSourceFB.SetItemText(0,3,"放大级1状态");
	m_pSeedSourceFB.SetItemText(0,4,"放大级2电流");
	m_pSeedSourceFB.SetItemText(0,5,"放大级2状态");
	m_pSeedSourceFB.SetItemText(0,6,"放大级3电流");
	m_pSeedSourceFB.SetItemText(0,7,"放大级3状态");
	m_pSeedSourceFB.ShowScrollBar(SB_BOTH, FALSE);

	////A激光状态
	m_pLightState.SetColumnCount(8);    // 设定总共的列数
	m_pLightState.SetColumnWidth(0, 40);
	m_pLightState.SetColumnWidth(1, 40);
	m_pLightState.SetColumnWidth(2, 40);
	m_pLightState.SetColumnWidth(3, 60);
	m_pLightState.SetColumnWidth(4, 40);
	m_pLightState.SetColumnWidth(5, 40);
	m_pLightState.SetColumnWidth(6, 40);
	m_pLightState.SetColumnWidth(7, 40);
	m_pLightState.SetRowCount(3);       // 设定总共的行数S
	m_pLightState.SetRowHeight(0,30);
	m_pLightState.SetFixedRowCount(1);  // 设定固定行数
	m_pLightState.SetItemText(0,0,"Pd1"); // SetItemText 用来设定某行某列的具体文本数值
	m_pLightState.SetItemText(0,1,"Pd2");
	m_pLightState.SetItemText(0,2,"Pd3");
	m_pLightState.SetItemText(0,3,"光源状态");
	m_pLightState.SetItemText(0,4,"T1");
	m_pLightState.SetItemText(0,5,"T2");
	m_pLightState.SetItemText(0,6,"T3");
	m_pLightState.SetItemText(0,7,"温度");
	m_pLightState.ShowScrollBar(SB_BOTH, FALSE);

	////B激光状态
	//m_pLight_B.SetColumnCount(8);    // 设定总共的列数
	//m_pLight_B.SetColumnWidth(0, 40);
	//m_pLight_B.SetColumnWidth(1, 40);
	//m_pLight_B.SetColumnWidth(2, 40);
	//m_pLight_B.SetColumnWidth(3, 60);
	//m_pLight_B.SetColumnWidth(4, 40);
	//m_pLight_B.SetColumnWidth(5, 40);
	//m_pLight_B.SetColumnWidth(6, 40);
	//m_pLight_B.SetColumnWidth(7, 40);
	//m_pLight_B.SetRowCount(2);       // 设定总共的行数S
	//m_pLight_B.SetRowHeight(0,30);
	//m_pLight_B.SetFixedRowCount(1);  // 设定固定行数
	//m_pLight_B.SetItemText(0,0,"Pd1"); // SetItemText 用来设定某行某列的具体文本数值
	//m_pLight_B.SetItemText(0,1,"Pd2");
	//m_pLight_B.SetItemText(0,2,"Pd3");
	//m_pLight_B.SetItemText(0,3,"光源状态");
	//m_pLight_B.SetItemText(0,4,"T1");
	//m_pLight_B.SetItemText(0,5,"T2");
	//m_pLight_B.SetItemText(0,6,"T3");
	//m_pLight_B.SetItemText(0,7,"温度");
	//m_pLight_B.ShowScrollBar(SB_BOTH, FALSE);
	
	//电源1设置
	m_pPower1.SetRowCount(10);       //设定总共的行数S
	m_pPower1.SetColumnCount(7);   // 设定总共的列数

	for (int i = 0; i < 7; i++)
	{
		m_pPower1.SetColumnWidth(i, 34);
	}
	m_pPower1.SetRowHeight(0,30);
	for (int i = 1; i < 10; i++)
	{
		m_pPower1.SetCellType(i, 1, RUNTIME_CLASS(CGridCellCheck));
	}
	m_pPower1.SetItemText(0,0,"id"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(0,2,"1");
	m_pPower1.SetItemText(0,3,"2");
	m_pPower1.SetItemText(0,4,"3");
	m_pPower1.SetItemText(0,5,"4");
	m_pPower1.SetItemText(0,6,"DLY");
	m_pPower1.SetItemText(1,0,"0x21");
	m_pPower1.SetItemText(2,0,"0x22");
	m_pPower1.SetItemText(3,0,"0x23");
	m_pPower1.SetItemText(4,0,"0x24");
	m_pPower1.SetItemText(5,0,"0x25");
	m_pPower1.SetItemText(6,0,"0x26");
	m_pPower1.SetItemText(7,0,"0x27");
	m_pPower1.SetItemText(8,0,"0x28");
	m_pPower1.SetItemText(9,0,"0x29");
	m_pPower1.SetItemText(1,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(1,3,"20");
	m_pPower1.SetItemText(1,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(1,5,"20");
	m_pPower1.SetItemText(1,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(2,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(2,3,"20");
	m_pPower1.SetItemText(2,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(2,5,"20");
	m_pPower1.SetItemText(2,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(3,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(3,3,"20");
	m_pPower1.SetItemText(3,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(3,5,"20");
	m_pPower1.SetItemText(3,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(4,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(4,3,"20");
	m_pPower1.SetItemText(4,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(4,5,"20");
	m_pPower1.SetItemText(4,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(5,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(5,3,"20");
	m_pPower1.SetItemText(5,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(5,5,"20");
	m_pPower1.SetItemText(5,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(6,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(6,3,"20");
	m_pPower1.SetItemText(6,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(6,5,"20");
	m_pPower1.SetItemText(6,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(7,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(7,3,"20");
	m_pPower1.SetItemText(7,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(7,5,"20");
	m_pPower1.SetItemText(7,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(8,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(8,3,"20");
	m_pPower1.SetItemText(8,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(8,5,"20");
	m_pPower1.SetItemText(8,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(9,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(9,3,"20");
	m_pPower1.SetItemText(9,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower1.SetItemText(9,5,"20");
	m_pPower1.SetItemText(9,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	for (int i = 1; i < 7; i++)
	{
		m_pPower1.SetItemState(0,i,GVIS_READONLY);/// 来设置单元格是否只读
	}
	for (int i = 1; i < 10; i++)
	{
		m_pPower1.SetItemState(i,0,GVIS_READONLY);/// 来设置单元格是否只读
	}
	m_pPower1.ShowScrollBar(SB_BOTH, FALSE);

	//电源2设置
	m_pPower2.SetRowCount(10);       //设定总共的行数S
	m_pPower2.SetColumnCount(7);   // 设定总共的列数

	for (int i = 0; i < 7; i++)
	{
		m_pPower2.SetColumnWidth(i, 34);
	}
	m_pPower2.SetRowHeight(0,30);
	for (int i = 1; i < 10; i++)
	{
		m_pPower2.SetCellType(i, 1, RUNTIME_CLASS(CGridCellCheck));
	}
	m_pPower2.SetItemText(0,0,"id"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(0,2,"1");
	m_pPower2.SetItemText(0,3,"2");
	m_pPower2.SetItemText(0,4,"3");
	m_pPower2.SetItemText(0,5,"4");
	m_pPower2.SetItemText(0,6,"DLY");
	m_pPower2.SetItemText(1,0,"0x21");
	m_pPower2.SetItemText(2,0,"0x22");
	m_pPower2.SetItemText(3,0,"0x23");
	m_pPower2.SetItemText(4,0,"0x24");
	m_pPower2.SetItemText(5,0,"0x25");
	m_pPower2.SetItemText(6,0,"0x26");
	m_pPower2.SetItemText(7,0,"0x27");
	m_pPower2.SetItemText(8,0,"0x28");
	m_pPower2.SetItemText(9,0,"0x29");
	m_pPower2.SetItemText(1,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(1,3,"20");
	m_pPower2.SetItemText(1,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(1,5,"20");
	m_pPower2.SetItemText(1,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(2,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(2,3,"20");
	m_pPower2.SetItemText(2,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(2,5,"20");
	m_pPower2.SetItemText(2,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(3,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(3,3,"20");
	m_pPower2.SetItemText(3,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(3,5,"20");
	m_pPower2.SetItemText(3,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(4,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(4,3,"20");
	m_pPower2.SetItemText(4,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(4,5,"20");
	m_pPower2.SetItemText(4,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(5,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(5,3,"20");
	m_pPower2.SetItemText(5,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(5,5,"20");
	m_pPower2.SetItemText(5,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(6,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(6,3,"20");
	m_pPower2.SetItemText(6,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(6,5,"20");
	m_pPower2.SetItemText(6,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(7,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(7,3,"20");
	m_pPower2.SetItemText(7,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(7,5,"20");
	m_pPower2.SetItemText(7,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(8,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(8,3,"20");
	m_pPower2.SetItemText(8,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(8,5,"20");
	m_pPower2.SetItemText(8,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(9,2,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(9,3,"20");
	m_pPower2.SetItemText(9,4,"20"); // SetItemText 用来设定某行某列的具体文本数值
	m_pPower2.SetItemText(9,5,"20");
	m_pPower2.SetItemText(9,6,"5"); // SetItemText 用来设定某行某列的具体文本数值
	for (int i = 1; i < 7; i++)
	{
		m_pPower2.SetItemState(0,i,GVIS_READONLY);/// 来设置单元格是否只读
	}
	for (int i = 1; i < 10; i++)
	{
		m_pPower2.SetItemState(i,0,GVIS_READONLY);/// 来设置单元格是否只读
	}
	m_pPower2.ShowScrollBar(SB_BOTH, FALSE);


	//电源状态1
	m_pPower_Recv1.SetRowCount(10); 
	m_pPower_Recv1.SetColumnCount(4);
	m_pPower_Recv1.SetColumnWidth(0, 40);
	m_pPower_Recv1.SetColumnWidth(1, 40);
	m_pPower_Recv1.SetColumnWidth(2, 60);
	m_pPower_Recv1.SetColumnWidth(3, 60);
	m_pPower_Recv1.SetRowHeight(0,30);
	m_pPower_Recv1.SetItemText(0,0,"电流");
	m_pPower_Recv1.SetItemText(0,1,"电压");
	m_pPower_Recv1.SetItemText(0,2,"状态字1");
	m_pPower_Recv1.SetItemText(0,3,"状态字2");
	m_pPower_Recv1.ShowScrollBar(SB_BOTH, FALSE);

	//电源状态2
	m_pPower_Recv2.SetRowCount(10); 
	m_pPower_Recv2.SetColumnCount(4);
	m_pPower_Recv2.SetColumnWidth(0, 40);
	m_pPower_Recv2.SetColumnWidth(1, 40);
	m_pPower_Recv2.SetColumnWidth(2, 60);
	m_pPower_Recv2.SetColumnWidth(3, 60);
	m_pPower_Recv2.SetRowHeight(0,30);
	m_pPower_Recv2.SetItemText(0,0,"电流");
	m_pPower_Recv2.SetItemText(0,1,"电压");
	m_pPower_Recv2.SetItemText(0,2,"状态字1");
	m_pPower_Recv2.SetItemText(0,3,"状态字2");
	m_pPower_Recv2.ShowScrollBar(SB_BOTH, FALSE);

	//电池设置
	//m_pBattery.SetColumnCount(3);    // 设定总共的列数
	//m_pBattery.SetRowCount(11);       // 设定总共的行数S
	//m_pBattery.SetColumnWidth(0, 30);
	//m_pBattery.SetColumnWidth(1, 30);
	//m_pBattery.SetColumnWidth(2, 110);
	//m_pBattery.SetRowHeight(0,30);
	//
	//m_pBattery.SetFixedRowCount(1);		// 设定固定行数
	////m_pBattery.SetFixedColumnCount(2);	// 设定固定列数  
	//for (int i = 1; i < 11; i++)
	//{
	//	m_pBattery.SetCellType(i, 0, RUNTIME_CLASS(CGridCellCheck));
	//}
	//for(int i = 1 ; i < 11; i++)
	//{
	//	CString str ;
	//	str.Format("%d", i);
	//	m_pBattery.SetItemText(i, 1, str);

	//	//电池放电combo
	//	m_pBattery.SetCellType(i,2, RUNTIME_CLASS(CGridCellCombo));
	//	/*m_pBattery.SetItemText(i,2, _T("预冲继电器"));
	//	CStringArray options;
	//	options.Add(_T("预冲继电器"));
	//	options.Add(_T("主继电器"));
	//	options.Add(_T("断开预冲继电器"));*/
	//	m_pBattery.SetItemText(i,2, _T("闭合预充"));
	//	CStringArray options;
	//	options.Add(_T("闭合预充"));
	//	options.Add(_T("闭合主继电器"));

	//	CGridCellCombo *pCell = (CGridCellCombo*) m_pBattery.GetCell(i,2);
	//	pCell->SetOptions(options);
	//	pCell->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
	//}
	//m_pBattery.SetItemText(0,1,"id");
	//m_pBattery.SetItemText(0,2,"放电模式");
	//m_pBattery.ShowScrollBar(SB_BOTH, FALSE);
	

	


	//电池反馈
	m_pReceiveBattery.SetColumnCount(7);    // 设定总共的列数
	m_pReceiveBattery.SetRowCount(11);       // 设定总共的行数S
	m_pReceiveBattery.SetColumnWidth(0, 30);
	m_pReceiveBattery.SetColumnWidth(1, 30);
	for (int i =2 ; i < 6; i++)
	{
		m_pReceiveBattery.SetColumnWidth(i, 50);
	}
	m_pReceiveBattery.SetColumnWidth(6, 110);
	for (int i = 1; i < 11; i++)
	{
		m_pReceiveBattery.SetCellType(i, 1, RUNTIME_CLASS(CGridCellCheck));
	}
	m_pReceiveBattery.SetRowHeight(0,25);
	m_pReceiveBattery.SetFixedRowCount(1);  // 设定固定行数
	m_pReceiveBattery.SetFixedColumnCount(1);  // 设定固定列数
	//m_pReceiveBattery.SetFixedColumnCount(2);  // 设定固定列数
	m_pReceiveBattery.SetItemText(0,0,"id"); // SetItemText 用来设定某行某列的具体文本数值
	m_pReceiveBattery.SetItemText(0,2,"总电流");
	m_pReceiveBattery.SetItemText(0,3,"总电压");
	m_pReceiveBattery.SetItemText(0,4,"soc");
	m_pReceiveBattery.SetItemText(0,5,"soh");
	m_pReceiveBattery.SetItemText(0,6,"继电器");
	for (int i = 1; i < 11; i++)
	{
		CString str = "";
		str.Format("%d", i);
		m_pReceiveBattery.SetItemText(i,0, str);
	}
	m_pReceiveBattery.ShowScrollBar(SB_BOTH, FALSE);

	//m_editFont.CreatePointFont(90, _T("宋体"),NULL);
	//((CListBox*)(g_pMainDlg->m_Page1.GetDlgItem(IDC_LIST2)))->SetFont(&m_editFont);

	SetTimer(0, 1000, NULL);
	SetTimer(1, 10000, NULL);
	//m_pCanManger->OpenCanPort("CAN6", FALSE);
	//m_pCanManger->SetBaud(250);
}

void CDlgTab1::OnBnClickedLedSetcurrent()
{
	// TODO: 在此添加控件通知处理程序代码
	switch (m_iIndex%2)
	{
	case 0:
		m_led_SETCURRENT.SetIcon(IDI_ICON_RED);
		break;
	case 1:
		m_led_SETCURRENT.SetIcon(IDI_ICON_GREEN);
		break;
	default:
		break;
	}
	m_iIndex++;
}

//按钮 种子源A设置
void CDlgTab1::OnBtnsetoutputcurrent()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str = "FE 03 00 ";
	CString sz1 = m_pCurrentSet.GetItemText(1, 0);
	CString sz2 = m_pCurrentSet.GetItemText(1, 1);
	CString sz3 = m_pCurrentSet.GetItemText(1, 2);
	CString sz4 = m_pCurrentSet.GetItemText(1, 3);
	CString resString = str + sz1 + " " + sz2 + " " + sz3 + " " + sz4 + " 00";
	GetDlgItem(IDC_current_send_EDIT)->SetWindowText(resString);
	m_pCanManager->Output_CurrentSupply(16, int(atof(sz1)*10), int(atof(sz2)*10), int(atof(sz3)*10), int(atof(sz4)*10), 500, NULL);
	g_Log.WriteLog(LOG_INF, _T("按钮 种子源A设置"));	//程序操作日志
}

//按钮 种子源A输出
void CDlgTab1::OnBtnoutputcurrent()
{
	// TODO: 在此添加控件通知处理程序代码
	CString resString = "fe 05 00 11 00 00 00 00";
	GetDlgItem(IDC_current_send_EDIT)->SetWindowText(resString);
	m_pCanManager->SendFrame_SeedSource_Output(16,500, NULL);
	g_Log.WriteLog(LOG_INF, _T("按钮 种子源A输出"));	//程序操作日志
	BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE};
	//g_pMainDlg->udpAsxHelper.Send(xx, 6);
}

//按钮 种子源A停止输出
void CDlgTab1::OnBtnstopoutputcurrent()
{
	// TODO: 在此添加控件通知处理程序代码
	CString resString = "fe 05 00 22 00 00 00 00";
	GetDlgItem(IDC_current_send_EDIT)->SetWindowText(resString);
	m_pCanManager->SendFrame_SeedSource_StopOutput(16, 500, NULL);
	g_Log.WriteLog(LOG_INF, _T("按钮 种子源A停止输出"));	//程序操作日志
}

//按钮 种子源A复位
void CDlgTab1::OnBtnresetcurrent()
{
	// TODO: 在此添加控件通知处理程序代码
	CString resString = "fe 07 11 00 00 00 00 00";
	GetDlgItem(IDC_current_send_EDIT)->SetWindowText(resString);
	m_pCanManager->Reset_SeedSource(16, 500, NULL);
	g_Log.WriteLog(LOG_INF, _T("按钮 种子源A复位"));	//程序操作日志
}

//按钮种子源B设置
void CDlgTab1::OnBnClickedBtnsetoutputcurrent2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str = "FE 03 00 ";
	CString sz1 = m_pCurrentSet.GetItemText(2, 0);
	CString sz2 = m_pCurrentSet.GetItemText(2, 1);
	CString sz3 = m_pCurrentSet.GetItemText(2, 2);
	CString sz4 = m_pCurrentSet.GetItemText(2, 3);
	CString resString = str + sz1 + " " + sz2 + " " + sz3 + " " + sz4 + " 00";
	GetDlgItem(IDC_current_send_EDIT2)->SetWindowText(resString);
	m_pCanManager->Output_CurrentSupply(16, int(atof(sz1)*10), int(atof(sz2)*10), int(atof(sz3)*10), int(atof(sz4)*10), 500, NULL);
	g_Log.WriteLog(LOG_INF, _T("按钮 种子源B设置"));	//程序操作日志
}

//按钮 种子源B输出
void CDlgTab1::OnBnClickedBtnoutputcurrent2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString resString = "fe 05 00 11 00 00 00 00";
	GetDlgItem(IDC_current_send_EDIT2)->SetWindowText(resString);
	m_pCanManager->SendFrame_SeedSource_Output(16, 500, NULL);
	g_Log.WriteLog(LOG_INF, _T("按钮 种子源B输出"));	//程序操作日志
	BYTE xx[6] = {0xBB, 0x12, 0x00, 0x00, 0x12, 0xEE};
	//g_pMainDlg->udpBsxHelper.Send(xx, 6);
}

//按钮 种子源B停止输出
void CDlgTab1::OnBnClickedBtnstopoutputcurrent2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString resString = "fe 05 00 22 00 00 00 00";
	GetDlgItem(IDC_current_send_EDIT2)->SetWindowText(resString);
	m_pCanManager->SendFrame_SeedSource_StopOutput(16, 500, NULL);
	g_Log.WriteLog(LOG_INF, _T("按钮 种子源B停止输出"));	//程序操作日志
}

//按钮 种子源B复位
void CDlgTab1::OnBnClickedBtnresetcurrent2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString resString = "fe 07 11 00 00 00 00 00";
	GetDlgItem(IDC_current_send_EDIT2)->SetWindowText(resString);
	m_pCanManager->Reset_SeedSource(16, 500, NULL);
	g_Log.WriteLog(LOG_INF, _T("按钮 种子源B复位"));	//程序操作日志
}

//按钮 电源A设置
void CDlgTab1::OnBtnsetparpower()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower1.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			BYTE current1 = atoi(m_pPower1.GetItemText(i,2));
			BYTE current2 = atoi(m_pPower1.GetItemText(i,3));
			BYTE current3 = atoi(m_pPower1.GetItemText(i,4));
			BYTE current4 = atoi(m_pPower1.GetItemText(i,5));
			BYTE delay = atoi(m_pPower1.GetItemText(i,6));
			res.Format("%s %d %s %02X %02X %02X %02X %02X %s", "A组", i, "号电源参数设置 FE A0", current1, current2, current3, current4, delay, "00");
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			res = "";
		}
	}
	for(int i = 1; i < row; i++)
	{
		if(((CGridCellCheck*)m_pPower1.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			if(m_pPower1.GetItemText(i, 2) != "" && m_pPower1.GetItemText(i, 3)!= ""
				&& m_pPower1.GetItemText(i, 4) != ""&& m_pPower1.GetItemText(i, 5) != ""
				&& m_pPower1.GetItemText(i, 6)!= "")
			{
				g_pCanManager1->SetParameter_PowerSupply(
					32 + i,
					atoi(m_pPower1.GetItemText(i, 2)),
					atoi(m_pPower1.GetItemText(i, 3)),
					atoi(m_pPower1.GetItemText(i, 4)),
					atoi(m_pPower1.GetItemText(i, 5)),
					atoi(m_pPower1.GetItemText(i, 6)),
					500,
					NULL);
				CString num;
				num.Format("%d", i);
				g_Log.WriteLog(LOG_INF, _T("按钮 电源A" + num + "设置"));	//程序操作日志
			}
		}
	}
}

//按钮 电源A查询
void CDlgTab1::OnBnClickedBtnquireparpower()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower1.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			BYTE current1 = atoi(m_pPower1.GetItemText(i,2));
			BYTE current2 = atoi(m_pPower1.GetItemText(i,3));
			BYTE current3 = atoi(m_pPower1.GetItemText(i,4));
			BYTE current4 = atoi(m_pPower1.GetItemText(i,5));
			BYTE delay = atoi(m_pPower1.GetItemText(i,6));
			//res.Format("%s %02X %02X %02X %02X %02X %s", "FE A0", current1, current2, current3, current4, delay, "FF");
			res.Format("%s %d %s %02X %02X %02X %02X %02X %s", "A组", i, "号电源参数查询 FE A0", current1, current2, current3, current4, delay, "FF");
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			res = "";
		}
	}
	for(int i = 1; i < row; i++)
	{
		if(((CGridCellCheck*)m_pPower1.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			if(m_pPower1.GetItemText(i, 2) != "" && m_pPower1.GetItemText(i, 3)!= ""
				&& m_pPower1.GetItemText(i, 4) != ""&& m_pPower1.GetItemText(i, 5) != ""
				&& m_pPower1.GetItemText(i, 6)!= "")
			{
				g_pCanManager1->QuerySetParameter_PowerSupply(
					32 + i,
					atoi(m_pPower1.GetItemText(i, 2)),
					atoi(m_pPower1.GetItemText(i, 3)),
					atoi(m_pPower1.GetItemText(i, 4)),
					atoi(m_pPower1.GetItemText(i, 5)),
					atoi(m_pPower1.GetItemText(i, 6)),
					500,
					NULL);
				CString num;
				num.Format("%d", i);
				g_Log.WriteLog(LOG_INF, _T("按钮 电源A" + num + "查询"));	//程序操作日志
			}
		}
	}
}

//按钮 电源A输出
void CDlgTab1::OnBnClickedBtnoutputpower()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower1.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			CString str;
			str.Format("%02X", BYTE(32 + i));
			res = "FE 03 00 01 01 00 00 00";
			res.Format("%s %d %s %s", "A组", i, "号电源 输出     ",  res);
			//IDC_LIST_SendToPower
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			g_pCanManager1->Output_PowerSupply(32 + i, 500, NULL);
			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电源A" + num + "输出"));	//程序操作日志
		}
	}
}

//按钮 电源A停止输出
void CDlgTab1::OnBnClickedBtnstopoutputpower()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower1.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			CString str;
			str.Format("%02X", BYTE(32 + i));
			res = "FE 03 00 00 00 00 00 00";
			res.Format("%s %d %s %s", "A组", i, "号电源 停止输出 ",  res);
			//IDC_LIST_SendToPower
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			g_pCanManager1->OutputStop_PowerSupply(32 + i, 500, NULL);

			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电源A" + num + "停止输出"));	//程序操作日志
		}
	}
}

//按钮 电源A复位
void CDlgTab1::OnBnClickedBtnresetpower()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;

	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower1.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			for(int j = 2; j < 7; j++)
			{
				m_pPower1.SetItemFgColour(i, j, RGB(0,0,0));
			}
			CString str;
			str.Format("%02X", BYTE(32 + i));
			res = "FE 05 00 00 00 00 00 00";
			res.Format("%s %d %s %s", "A组", i, "号电源 复位     ",  res);
			//IDC_LIST_SendToPower
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			g_pCanManager1->Reset_PowerSupply(32 + i, 500, NULL);

			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电源A" + num + "复位"));	//程序操作日志
		}
	}
}

//按钮 电源B设置
void CDlgTab1::OnBnClickedBtnsetparpower2()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower2.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			BYTE current1 = atoi(m_pPower2.GetItemText(i,2));
			BYTE current2 = atoi(m_pPower2.GetItemText(i,3));
			BYTE current3 = atoi(m_pPower2.GetItemText(i,4));
			BYTE current4 = atoi(m_pPower2.GetItemText(i,5));
			BYTE delay = atoi(m_pPower2.GetItemText(i,6));
			res.Format("%s %d %s %02X %02X %02X %02X %02X %s", "B组", i, "号电源参数设置 FE A0", current1, current2, current3, current4, delay, "00");
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			res = "";
		}
	}
	for(int i = 1; i < row; i++)
	{
		if(((CGridCellCheck*)m_pPower2.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			if(m_pPower2.GetItemText(i, 2) != "" && m_pPower2.GetItemText(i, 3)!= ""
				&& m_pPower2.GetItemText(i, 4) != ""&& m_pPower2.GetItemText(i, 5) != ""
				&& m_pPower2.GetItemText(i, 6)!= "")
			{
				g_pCanManager2->SetParameter_PowerSupply(
					32 + i,
					atoi(m_pPower2.GetItemText(i, 2)),
					atoi(m_pPower2.GetItemText(i, 3)),
					atoi(m_pPower2.GetItemText(i, 4)),
					atoi(m_pPower2.GetItemText(i, 5)),
					atoi(m_pPower2.GetItemText(i, 6)),
					500,
					"2");
				CString num;
				num.Format("%d", i);
				g_Log.WriteLog(LOG_INF, _T("按钮 电源B" + num + "设置"));	//程序操作日志
			}
		}
	}
}

//按钮 电源B查询
void CDlgTab1::OnBnClickedBtnquireparpower2()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower2.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			BYTE current1 = atoi(m_pPower2.GetItemText(i,2));
			BYTE current2 = atoi(m_pPower2.GetItemText(i,3));
			BYTE current3 = atoi(m_pPower2.GetItemText(i,4));
			BYTE current4 = atoi(m_pPower2.GetItemText(i,5));
			BYTE delay = atoi(m_pPower2.GetItemText(i,6));
			//res.Format("%s %02X %02X %02X %02X %02X %s", "FE A0", current1, current2, current3, current4, delay, "FF");
			res.Format("%s %d %s %02X %02X %02X %02X %02X %s", "B组", i, "号电源参数查询 FE A0", current1, current2, current3, current4, delay, "FF");
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			res = "";
		}
	}
	for(int i = 1; i < row; i++)
	{
		if(((CGridCellCheck*)m_pPower2.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			if(m_pPower2.GetItemText(i, 2) != "" && m_pPower2.GetItemText(i, 3)!= ""
				&& m_pPower2.GetItemText(i, 4) != ""&& m_pPower2.GetItemText(i, 5) != ""
				&& m_pPower2.GetItemText(i, 6)!= "")
			{
				g_pCanManager2->QuerySetParameter_PowerSupply(
					32 + i,
					atoi(m_pPower2.GetItemText(i, 2)),
					atoi(m_pPower2.GetItemText(i, 3)),
					atoi(m_pPower2.GetItemText(i, 4)),
					atoi(m_pPower2.GetItemText(i, 5)),
					atoi(m_pPower2.GetItemText(i, 6)),
					500,
					"2");
				CString num;
				num.Format("%d", i);
				g_Log.WriteLog(LOG_INF, _T("按钮 电源B" + num + "查询"));	//程序操作日志
			}
		}
	}
}

//按钮 电源B输出
void CDlgTab1::OnBnClickedBtnoutputpower2()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower2.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			CString str;
			str.Format("%02X", BYTE(32 + i));
			res = "FE 03 00 01 01 00 00 00";
			res.Format("%s %d %s %s", "B组", i, "号电源 输出     ",  res);
			//IDC_LIST_SendToPower
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			g_pCanManager2->Output_PowerSupply(32 + i, 500, "2");

			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电源B" + num + "输出"));	//程序操作日志
		}
	}
}

//按钮 电源B停止输出
void CDlgTab1::OnBnClickedBtnstopoutputpower2()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower2.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			CString str;
			str.Format("%02X", BYTE(32 + i));
			res = "FE 03 00 00 00 00 00 00";
			res.Format("%s %d %s %s", "B组", i, "号电源 停止输出 ",  res);
			//IDC_LIST_SendToPower
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			g_pCanManager2->OutputStop_PowerSupply(32 + i, 500, "2");

			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电源B" + num + "停止输出"));	//程序操作日志
		}
	}
}

//按钮 电源B复位
void CDlgTab1::OnBnClickedBtnresetpower2()
{
	// TODO: 在此添加控件通知处理程序代码
	int row = 10;
	for (int i = 1; i < row; i++)
	{
		CString res;
		if(((CGridCellCheck*)m_pPower2.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			for(int j = 2; j < 7; j++)
			{
				m_pPower2.SetItemFgColour(i, j, RGB(0,0,0));
			}
			CString str;
			str.Format("%02X", BYTE(32 + i));
			res = "FE 05 00 00 00 00 00 00";
			res.Format("%s %d %s %s", "B组", i, "号电源 复位     ",  res);
			//IDC_LIST_SendToPower
			((CListBox*)GetDlgItem(IDC_LIST_SendToPower))->InsertString(0, res);
			g_pCanManager2->Reset_PowerSupply(32 + i, 500, "2");

			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电源B" + num + "复位"));	//程序操作日志
		}
	}
}

void CDlgTab1::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
/*	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	if(pItem->iRow <= 0 || pItem->iColumn <= 0) return; */

	GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNMHDR;
	GV_ITEM     *pgvItem = &pgvDispInfo->item;

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNMHDR;
	CGridCtrl *pGrid = &m_pPower1;
	if (pGrid == NULL || pGrid->GetSafeHwnd() == NULL) return;
	CGridCell *pCell = (CGridCell*)pGrid->GetCell(pItem->iRow, pItem->iColumn);
	if (pCell == NULL) return;

	/*if(((CGridCellCheck*)m_pPower1.GetCell(0, 0))->GetCheck())
	{
	for (int i = 1; i < 21; i++)
	{
	((CGridCellCheck*)m_pPower1.GetCell(0, i))->SetCheck(true);
	}
	}*/

	//else
	//{
	//	for (int i = 1; i < 21; i++)
	//	{
	//		((CGridCellCheck*)m_pPower1.GetCell(0, i))->SetCheck(false);
	//	}
	//}

	//LPARAM lParam = pGrid->GetItemData(pItem->iRow, pItem->iColumn);
	/*switch (lParam)
	{
	case CHECKCELL_CHECKFLAG:
		if (pCell != NULL && pCell->IsKindOf(RUNTIME_CLASS(CGridCellCheck)))
		{
			if (((CGridCellCheck*)pCell)->GetCheck() == true)
			{
				pGrid->Refresh();
			}
		}

		break;
	default:
		break;
	}  */
}

//void CDlgTab1::OnCbnSelchangeCombo1()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	int index = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();//获取当前索引
//
//	CString str;
//	((CComboBox*)GetDlgItem(IDC_COMBO1))->GetLBText(index, str);
//	MessageBox(str);
//
//}

//按钮 电池充电
void CDlgTab1::OnBnClickedBtnbatterycharge()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 1; i < 11; i++)
	{
		if(((CGridCellCheck*)m_pReceiveBattery.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		//if(((CGridCellCheck*)m_pBattery.GetCell(i, 0))->GetCheck()) //判断checkbox是不是勾选了
		{
			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电池" + num + "充电"));	//程序操作日志

			int ChargeCtrl_    = 1; //开始充电
			int tmp    =  (g_pMainDlg->m_Page2).GetIndexOfParameterGrid(i, 1);
			int ChargeMode;
			if(tmp == 0)
				ChargeMode = 2;  //恒功率充电
			if (tmp == 1)
				ChargeMode = 1;  //恒压充电
			if (tmp == 2)
				ChargeMode = 0;  //恒流充电
			int ChargeId      =  _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,2));
			int ChargeCurrent;
			if(ChargeMode == 0)  //对应恒流充电电流(若为其他充电模式，此字节为最大充电电流限值)
				ChargeCurrent =  _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,3));
			else
				ChargeCurrent = 127;	//发送函数需要乘以2
			int ChargePower;
			if(ChargeMode == 2) //对应恒功率充电功率(若为其他充电模式，默认发FF)
				ChargePower=  _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,4));
			else
				ChargePower = 255*25;	//发送函数需要除于25，也就是发送FF
			int ChargeVoltage;
			if(ChargeMode == 1)  //对应恒压充电电压(若为其他充电模式，此字节为最高电压限值)
				ChargeVoltage = _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,5));
			else
				ChargeVoltage = 800;	//发送函数需要乘以10，也就是发送8000V
			int ChargeState   =  _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,6));
			

			DWORD tmpID = 0X1806FE00 + DWORD(i);
			g_pCanManager4->ChargeControl(tmpID,0,80,0,0,NULL);	
			CString s;
			s.Format("%s %02X,  %s","开始充电:",  i, "00 50 00 00 00 00 00 00");
			//MessageBox(s, "test");
			((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
			Sleep(20);
			tmpID = 0X1807FE00 + DWORD(i);
			g_pCanManager4->BatteryChargeParameter_Supply(tmpID, ChargeCtrl_, ChargeMode, ChargeCurrent, ChargePower, ChargeVoltage, ChargeState, NULL);
			s.Format("%s %02X,  %02X %02X %02X %02X %02X %02X %02X %s","电池充电:",  i, ChargeCtrl_, ChargeMode, ChargeCurrent*2, ChargePower/25, (ChargeVoltage*10)>>8, (ChargeVoltage*10)%256, ChargeState, "00");
			//MessageBox(s, "test");
			((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
			
			//g_pCanManager4->BatteryChargeParameter_Supply(tmpID, 01, 02, 02, 2000, 00, 01, NULL);
			//g_pCanManager4->BatteryChargeParameter_Supply(tmpID, 01, 02, 01, 2000, 450, 01, NULL);
		}
	}
	//for(int i = 1; i < 11; i++)
	//{
	//	if(((CGridCellCheck*)m_pReceiveBattery.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
	//	{
	//		DWORD tmpID = 0X1806FE00 + DWORD(i);
	//		/*int SwitchCtrl = (g_pMainDlg->m_Page2).GetIndexOfSwitchGrid(i, 1);
	//		int ChargeCtrl = 0;
	//		if(SwitchCtrl == 0)
	//		ChargeCtrl = 1;
	//		else
	//		ChargeCtrl = 0;
	//		int StrongCtrl = (g_pMainDlg->m_Page2).GetIndexOfSwitchGrid(i, 2);
	//		int StrongVoltage = _ttoi((g_pMainDlg->m_Page2).GetValueOfSwitchGrid(i,3));
	//		if(StrongCtrl == 0)
	//		StrongVoltage = 0;*/
	//		/*CString tmpS;
	//		tmpS.Format("%d,%d,%d,%d",SwitchCtrl, ChargeCtrl, StrongCtrl, StrongVoltage);
	//		MessageBox(tmpS, "test");*/
	//		g_pCanManager4->ChargeControl(tmpID, 0, 80, 0, 0, NULL);

	//		//如果最后一位返回40代表设置OK
	//		if(g_pMainDlg->m_Page2.m_chargeFlag[i])
	//		{
	//			int ChargeCtrl_    = 1; //开始充电
	//			int tmp    =  (g_pMainDlg->m_Page2).GetIndexOfParameterGrid(i, 1);
	//			int ChargeMode;
	//			if(tmp == 0)
	//				ChargeMode = 0;  //恒流充电
	//			if (tmp == 1)
	//				ChargeMode = 1;  //恒压充电
	//			if (tmp == 2)
	//				ChargeMode = 2;  //恒功率充电
	//			int ChargeId      =  _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,2));
	//			int ChargeCurrent;
	//			if(ChargeMode == 0)  //对应恒流充电电流(若为其他充电模式，此字节为最大充电电流限值)
	//				ChargeCurrent =  _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,3));
	//			else
	//				ChargeCurrent = 127;	//发送函数需要乘以2
	//			int ChargePower;
	//			if(ChargeMode == 2) //对应恒功率充电功率(若为其他充电模式，默认发FF)
	//				ChargePower=  _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,4));
	//			else
	//				ChargePower = 255*25;	//发送函数需要除于25，也就是发送FF
	//			int ChargeVoltage;
	//			if(ChargeMode == 1)  //对应恒压充电电压(若为其他充电模式，此字节为最高电压限值)
	//				ChargeVoltage = _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,5));
	//			else
	//				ChargeVoltage = 800;	//发送函数需要乘以10，也就是发送8000V
	//			int ChargeState   =  _ttoi((g_pMainDlg->m_Page2).GetValueOfParameterGrid(i,6));
	//			CString s;
	//			s.Format("%d,%d,%d,%d,%d,%d", ChargeCtrl_, ChargeMode, ChargeCurrent, ChargePower, ChargeVoltage, ChargeState);
	//			//MessageBox(s, "test");
	//			//接下去间隔100ms 猛发指令
	//			//g_pCanManager4->BatteryChargeParameter_Supply(tmpID, ChargeCtrl_, ChargeMode, ChargeCurrent, ChargePower, ChargeVoltage, ChargeState, NULL);
	//			tmpID = tmpID | 0x00070000;
	//			g_pCanManager4->BatteryChargeParameter_Supply(tmpID, ChargeCtrl_, ChargeMode, ChargeCurrent, ChargePower, ChargeVoltage, ChargeState, NULL);
	//			//g_pCanManager4->BatteryChargeParameter_Supply(tmpID, 01, 02, 02, 2000, 00, 01, NULL);
	//			//g_pCanManager4->BatteryChargeParameter_Supply(tmpID, 01, 02, 01, 2000, 450, 01, NULL);
	//		}	
	//	}
	//}
}

//按钮 电池停止充电
void CDlgTab1::OnBnClickedBtnstopbatterycharge()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i = 1; i < 11; i++)
	{
		if(((CGridCellCheck*)m_pReceiveBattery.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		//if(((CGridCellCheck*)m_pBattery.GetCell(i, 0))->GetCheck()) //判断checkbox是不是勾选了
		{
			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电池" + num + "充电"));	//程序操作日志

			DWORD tmpID = 0X1806FE00 + DWORD(i);
			g_pCanManager4->StopChargeControl(tmpID, 0, 0, 0, 0, NULL);

			CString s;
			s.Format("%s %02X,  %s","停止充电:",  i, "00 00 00 00 00 00 00 00");
			//MessageBox(s, "test");
			((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
			g_chargeEnableFlag=-1;
		}
	}
}

//按钮 电池放电
void CDlgTab1::OnBnClickedBtnbatterydischarge()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i = 1; i < 11; i++)
	{
		//if(((CGridCellCheck*)m_pBattery.GetCell(i, 0))->GetCheck()) //判断checkbox是不是勾选了
		if(((CGridCellCheck*)m_pReceiveBattery.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			if(g_BatRelayState[i] != 0)	//如果是无状态,才能进行放电.否则跳过
				continue;
			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电池" + num + "充电"));	//程序操作日志

			DWORD tmpID = 0X1806FE00 + DWORD(i);
			//int index = ((CGridCellCombo*)(g_pMainDlg->m_Page1.m_pBattery.GetCell(i, 2)))->getCurSel();//获得当先combo的index
			/*int	mode = 0;
			if(index == 0)
			mode = 16;
			if(index == 1)
			mode = 16*5;
			if(index == 2)
			mode = 16*4;
			g_pCanManager4->DisChargeControl(tmpID, mode, NULL);*/	
			//if (index == 0)
			//{
				g_pCanManager4->DisChargeControl(tmpID, 16, NULL);
				CString s;
				s.Format("%s %02X,  %s","电池放电:",  i, "10 00 00 00 00 00 00 00");
				((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
				g_batoutput = 1; //进入放电流程
			//}
			/*if(index == 1)
			{
			g_pCanManager4->DisChargeControl(tmpID, 80, NULL);
			Sleep(50);
			g_pCanManager4->DisChargeControl(tmpID, 64, NULL);
			CString s;
			s.Format("%s %02X,  %s","电池放电:",  i, "50 00 00 00 00 00 00 00");
			((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
			s.Format("%s %02X,  %s","电池放电:",  i, "40 00 00 00 00 00 00 00");
			((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
			}*/
		}
		Sleep(5);
	}
	//Sleep(1500);
	//for(int i = 1; i < 11; i++)
	//{
	//	if(((CGridCellCheck*)m_pBattery.GetCell(i, 0))->GetCheck()) //判断checkbox是不是勾选了
	//	{
	//		DWORD tmpID = 0X1806FE00 + DWORD(i);
	//		int index = ((CGridCellCombo*)(g_pMainDlg->m_Page1.m_pBattery.GetCell(i, 2)))->getCurSel();//获得当先combo的index
	//		if(index == 0)
	//		{
	//			if(g_yuchongOK[i] == TRUE)
	//			{
	//				g_pCanManager4->DisChargeControl(tmpID, 80, NULL);
	//				Sleep(50);
	//				g_pCanManager4->DisChargeControl(tmpID, 64, NULL);
	//				CString s;
	//				s.Format("%s %02X,  %s","电池放电:",  i, "50 00 00 00 00 00 00 00");
	//				((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
	//				s.Format("%s %02X,  %s","电池放电:",  i, "40 00 00 00 00 00 00 00");
	//				((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
	//			}
	//		}
	//	}
	//}
}

//按钮 电池停止放电
void CDlgTab1::OnBnClickedBtnstopbatterydischarge()
{
	// TODO: 在此添加控件通知处理程序代码
	g_batoutput = 0; //退出放电流程
	g_hotBoot = 0;
	for(int i = 1; i < 11; i++) 
	{
		//if(((CGridCellCheck*)m_pBattery.GetCell(i, 0))->GetCheck()) //判断checkbox是不是勾选了
		if(((CGridCellCheck*)m_pReceiveBattery.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
		{
			CString num;
			num.Format("%d", i);
			g_Log.WriteLog(LOG_INF, _T("按钮 电池" + num + "充电"));	//程序操作日志

			DWORD tmpID = 0X1806FE00 + DWORD(i);
			g_pCanManager4->StopDisChargeControl(tmpID, NULL);	

			CString s;
			s.Format("%s %02X,  %s","停止放电:",  i, "00 00 00 00 00 00 00 00");
			//MessageBox(s, "test");
			((CListBox*)GetDlgItem(IDC_LIST_SendToBat))->InsertString(0, s);
			g_yuchongOK[i] = FALSE;
		}
	}
}

void CDlgTab1::OnBnClickedBtnAllchoose3()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i = 1; i < 11; i++) 
	{
		//((CGridCellCheck*)m_pBattery.GetCell(i, 0))->SetCheck(true);
		((CGridCellCheck*)m_pReceiveBattery.GetCell(i, 1))->SetCheck(true);
	}
}


void CDlgTab1::OnBnClickedBtnAllnotchoose3()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i = 1; i < 11; i++) 
	{
		//((CGridCellCheck*)m_pBattery.GetCell(i, 0))->SetCheck(false);
		((CGridCellCheck*)m_pReceiveBattery.GetCell(i, 1))->SetCheck(false);
	}
}


void CDlgTab1::OnBnClickedBtnAllchoose()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 1; i < 10; i++)
	{
		((CGridCellCheck*)m_pPower1.GetCell(i, 1))->SetCheck(true);
		//((CGridCellCheck*)m_pPower2.GetCell(i, 1))->SetCheck(true);
	}
}


void CDlgTab1::OnBnClickedBtnAllnotchoose()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 1; i < 10; i++)
	{
		((CGridCellCheck*)m_pPower1.GetCell(i, 1))->SetCheck(false);
		//((CGridCellCheck*)m_pPower2.GetCell(i, 1))->SetCheck(false);
	}
}

void CDlgTab1::OnBnClickedBtnAllchoose2()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 1; i < 10; i++)
	{
		//((CGridCellCheck*)m_pPower1.GetCell(i, 1))->SetCheck(true);
		((CGridCellCheck*)m_pPower2.GetCell(i, 1))->SetCheck(true);
	}
}


void CDlgTab1::OnBnClickedBtnAllnotchoose2()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 1; i < 10; i++)
	{
		//((CGridCellCheck*)m_pPower1.GetCell(i, 1))->SetCheck(false);
		((CGridCellCheck*)m_pPower2.GetCell(i, 1))->SetCheck(false);
	}
}



void CDlgTab1::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case 0:
		{
			CString res;
			for (int i = 1; i < 11; i++)
			{
				//if(((CGridCellCheck*)m_pPower1.GetCell(i, 1))->GetCheck()) //判断checkbox是不是勾选了
				{
					/*CString id = m_pPower1.GetItemText(i, 0);
					res = res + id + " fe 07 00 00 00 00 00 00 \r\n";
					BYTE m_id = (id[2] - '0')*16 + (id[3] - '0');*/
					BYTE m_id = (BYTE)32+i;
					//电源 状态查询
					g_pCanManager1->GetStatus_PowerSupply(m_id, 500, NULL);
					Sleep(20);
					g_pCanManager2->GetStatus_PowerSupply(m_id, 500, "2");
				}
			}

			//种子源A 种子源B 激光状态查询
			g_pCanManager3->GetStatus_Laser(0x10, 500, NULL);
			g_pCanManager3->GetStatus_Laser(0x11, 500, NULL);

			////种子源A 种子源B 电源状态查询
			g_pCanManager3->GetStatus_Battery(0x10, 500, NULL);
			g_pCanManager3->GetStatus_Battery(0x11, 500, NULL);

			m_pLightState.Refresh();
			//m_pLight_B.Refresh();
			m_pSeedSourceFB.Refresh();
			m_pPower_Recv1.Refresh();
			m_pPower_Recv2.Refresh();
			m_pPower1.Refresh();
			m_pPower2.Refresh();
			m_pCurrentSet.Refresh();

			m_pReceiveBattery.Refresh();
			
		}
		break;
	case 1:
		{
			//int m_nMinRow;
			//int m_nMinCol;
			//int m_nMaxRow;
			//int m_nMaxCol;
			CCellRange range(1,2,10,6);
			g_pMainDlg->m_Page1.m_pReceiveBattery.ClearCells(range);
		}
		break;
	default:
		break;
	}


	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CDlgTab1::display_receive_battery(WPARAM wPARAM, LPARAM lPARAM)
{
	BYTE *data = (BYTE*)lPARAM;
	DWORD sub = ((DWORD)wPARAM) & ((DWORD)0x00000f00);
	DWORD CobId = (DWORD)wPARAM;
	int i;

	CString strTemp;
	strTemp.Format("%02X %02X %02X %02X %02X %02X %02X %02X",data[0], data[1]\
		,data[2], data[3],data[4], data[5],data[6], data[7] );
	//m_pBatStatusReceive.InsertString(0, strTemp);

	switch(sub)
	{
	case 0x00000100:
		break;
	case 0x00000200:
		{
			CString str;
			i = 2;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00030000:
				{
					//电池总电流
					str.Format("%d", (data[0] + data[1]*255)/2);
					m_pReceiveBattery.SetItemText(i, 2, str);
					//电池总电压
					str.Format("%.1f", (float)((data[2] + data[3]*255))/10);
					m_pReceiveBattery.SetItemText(i, 3, str);
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*255))/10);
					m_pReceiveBattery.SetItemText(i, 4, str);
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pReceiveBattery.SetItemText(i, 5, str);
					//继电器状态
					str.Format("%02X", data[7]);
					m_pReceiveBattery.SetItemText(i, 6, str);

					//Refresh();
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000300:
		break;
	default:
		break;
	}
	
	m_pReceiveBattery.Refresh();

	return 0;
}

LRESULT CDlgTab1::change_led(WPARAM wPARAM, LPARAM lPARAM)
{
	BYTE id   = (BYTE) wPARAM;
	BYTE type = (BYTE) lPARAM;
	if(id == 0x10)
	{
		switch(type)
		{
		case 0X04:
			{
				m_led_resetCURRENT.SetIcon(IDI_ICON_RED);
				m_led_SETCURRENT.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X06:
			{
				m_led_outputSETCURRENT.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X07:
			{
				m_led_stopCURRENT.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X08:
			{
				m_led_SETCURRENT.SetIcon(IDI_ICON_RED);
				m_led_outputSETCURRENT.SetIcon(IDI_ICON_RED);
				m_led_stopCURRENT.SetIcon(IDI_ICON_RED);
				m_led_resetCURRENT.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X0a:
			break;
		default:
			break;
		}
	}
	if(id == 0x11)
	{
		switch(type)
		{
		case 0X04:
			{
				m_led_resetCURRENT2.SetIcon(IDI_ICON_RED);
				m_led_SETCURRENT2.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X06:
			{
				m_led_outputSETCURRENT2.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X07:
			{
				m_led_stopCURRENT2.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X08:
			{
				m_led_SETCURRENT2.SetIcon(IDI_ICON_RED);
				m_led_outputSETCURRENT2.SetIcon(IDI_ICON_RED);
				m_led_stopCURRENT2.SetIcon(IDI_ICON_RED);
				m_led_resetCURRENT2.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X0a:
			break;
		default:
			break;
		}
	}

	switch(id)
	{
	case 0X21:
		switch(type)
		{
		case 0X02:
			{
				m_led_resetPOWER.SetIcon(IDI_ICON_RED);
				m_led_setparPOWER.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X04:
			{
				m_led_outputPOWER.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X05:
			{
				m_led_stopoutputPOWER.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X06:
			{
				m_led_setparPOWER.SetIcon(IDI_ICON_RED);
				m_led_outputPOWER.SetIcon(IDI_ICON_RED);
				m_led_stopoutputPOWER.SetIcon(IDI_ICON_RED);
				m_led_resetPOWER.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X08:
			{

			}
			break;
		default:
			break;
		}
		break;
	case 0X22:
		switch(type)
		{
		case 0X02:
			{
				m_led_setparPOWER.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X04:
			{
				m_led_outputPOWER.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X05:
			{
				m_led_stopoutputPOWER.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X06:
			{
				m_led_resetPOWER.SetIcon(IDI_ICON_GREEN);
			}
			break;
		case 0X08:
			{

			}
			break;
		default:
			break;
		}
		break;
	case 0X26:
		break;
	case 0X27:
		break;
	case 0X28:
		break;
	case 0X29:
		break;
	default:
		break;
	}
	return 0;
}





