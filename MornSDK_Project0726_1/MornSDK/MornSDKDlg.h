
// MornSDKDlg.h : 头文件
//

#pragma once
#include "General.h"
#include "Task.h"
#include "DlgTab1.h"
#include "DlgTab2.h"
#include "DlgTab3.h"
#include "DlgTab4.h"

enum Function{Sin, Cos};
// CMornSDKDlg 对话框
class CMornSDKDlg : public CDialogEx, public has_slots<>
{
// 构造
public:
	CMornSDKDlg(CWnd* pParent = NULL);	// 标准构造函数
	//Tab控件
	CTabCtrl m_tabCtrl;
	//当前标签页序号
	int m_iCurTab; 
	CDlgTab1 m_Page1; 
	CDlgTab2 m_Page2;
	CDlgTab3 m_Page3;
	CDlgTab4 m_Page4;
	CDialog* m_pPages[4];  //用来保存对话框对象指针

// 对话框数据
	enum { IDD = IDD_MORNSDK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	void InitCnComm();
	//Udp数据接收处理
	void OnUdpReceived(uchar *, int len);
	//回调处理函数
	static int FeedBack(CTask* pTask, void* pData);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//afx_msg LRESULT OnCnCommRecv(WPARAM wPARAM, LPARAM lPARAM);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeTabctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	//afx_msg void OnBnClickedBtnTest();
	int ParseUdpTaskDown(uchar* pdata);
	int ParseUdpQGCtrl(uchar* pdata);
	int ParseUdpDataUp(uchar* pdata);
	int ParseUdpSysCtrl(uchar* pdata, int len);

	//YWJ----------------
	//通过RS422下发串口命令
	void SendSPCmdToDown(uchar cmd, uchar data1=0, uchar data2=0);
	//YWJ----------------
	//通过UDP下发时序控制器命令
	void SendUDPCmdToDownA(uchar cmd, uchar data1, uchar data2);
	void SendUDPCmdToDownB(uchar cmd, uchar data1, uchar data2);

	//解析时序控制UDP反馈
	void OnUdpReceivedFromSXA(uchar* pData, int len);
	void OnUdpReceivedFromSXB(uchar* pData, int len);
	//解析来自上位机的串口字符串
	int AnalyzStringFromUp(CString s);

	CWinThread* hThreadRead;
	//UDP数据传输类对象
	CUdpHelper udpHelper;
	//时序控制卡A 数据传输类对象
	CUdpHelper udpAsxHelper;
	//时序控制卡B 数据传输类对象
	CUdpHelper udpBsxHelper;
	//温湿度 数据传输类对象
	CUdpHelper udpWsxHelper;

	CStringArray options;

	//当前任务编号
	CString szCurTaskNo;
	//记录出光状态，0，未出光，1，正在出光
	int isStartQg;
	void  splitStrUdp(int cmd, int index, int res);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//判断串口toUp是不是接收到完整的字符串
	int isZuo;
	CString serialStr;

	void InitSerial();
	void OnSerialPortReceived();

	//电池控制指令m_index
	WORD m_index;

	void WriteLocalStateLog();	//记录本地试验数据
	void WriteUpStateLog();	//记录任务编号试验数据

	//规模变换函数
public:
	void SetScale(int scale);
	float TransformScale(float num);

private:
	int scale;
	//变换坐标和规模
public:
	float TransformCoordinateScaleX(float x);
	float TransformCoordinateScaleY(float y);

	//设置绘制的函数类型
public:
	void SetDrawFunction(Function fun);
private:
	Function fun;

	//函数范围和步长
public:
	void SetPlotSin(float startX, float endX, float step);
private:
	float startX, endX, step;
CBitmap* draw_Bitmap(vector<vector<int>>& v);
};


