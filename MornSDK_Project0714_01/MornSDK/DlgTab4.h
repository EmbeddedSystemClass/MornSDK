#pragma once
#include "grid/GridCtrl.h"
#include "grid/GridCellCheck.h"
#include "General.h"
#include "afxwin.h"

// CDlgTab4 对话框
#define WM_DISPYAY   (WM_USER+101) 
class CDlgTab4 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTab4)

public:
	CDlgTab4(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTab4();

	//初始化页面函数
	BOOL InitPage();

// 对话框数据
	enum { IDD = IDD_Tab4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	//CGridCtrl m_pReceive_LightA;
	//CGridCtrl m_pReceive_LightB;
	//CGridCtrl m_pReceive_SeedSource;
	CGridCtrl m_pReceive_Power;
	CGridCtrl m_pReceive_Status;
	CWinThread* hThreadSendAndReceive;

	CEdit m_pEditCount;
	void textReset(BYTE id);
	void GridInit();
	afx_msg LRESULT display2(WPARAM wPARAM, LPARAM lPARAM);
};
