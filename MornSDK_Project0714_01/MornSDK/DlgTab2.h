#pragma once
#include "grid/GridCtrl.h"
#include "grid/GridCellCombo.h"
#include "afxwin.h"
#include "CanManager.h"
#include "General.h"

#define WM_DISPYAY_CHANGE    (WM_USER+100)
// CDlgTab2 �Ի���

class CDlgTab2 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTab2)

public:
	CDlgTab2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTab2();

	//��ʼ��ҳ�溯��
	void InitPage();

// �Ի�������
	enum { IDD = IDD_Tab2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CGridCtrl m_pBattery;
	CGridCtrl m_pCharge;
	//CGridCtrl m_pChargeState;
	//�˿�����
	CString m_szPortName;
	BOOL g_bExitWriteThreadFlag;
	CWinThread* hThreadReadBat;
	void GridInit();
	afx_msg LRESULT display(WPARAM wPARAM, LPARAM lPARAM);
	CListBox m_pBatStatusReceive;
	void test();
	int GetIndexOfParameterGrid(int x, int y);
	CString GetValueOfParameterGrid(int x, int y);
	BOOL GetIndexOfCharge(int x, int y);
	BOOL m_chargeFlag[11];
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	int m_BatRcvNum;
};
