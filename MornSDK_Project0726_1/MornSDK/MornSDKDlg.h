
// MornSDKDlg.h : ͷ�ļ�
//

#pragma once
#include "General.h"
#include "Task.h"
#include "DlgTab1.h"
#include "DlgTab2.h"
#include "DlgTab3.h"
#include "DlgTab4.h"

enum Function{Sin, Cos};
// CMornSDKDlg �Ի���
class CMornSDKDlg : public CDialogEx, public has_slots<>
{
// ����
public:
	CMornSDKDlg(CWnd* pParent = NULL);	// ��׼���캯��
	//Tab�ؼ�
	CTabCtrl m_tabCtrl;
	//��ǰ��ǩҳ���
	int m_iCurTab; 
	CDlgTab1 m_Page1; 
	CDlgTab2 m_Page2;
	CDlgTab3 m_Page3;
	CDlgTab4 m_Page4;
	CDialog* m_pPages[4];  //��������Ի������ָ��

// �Ի�������
	enum { IDD = IDD_MORNSDK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	void InitCnComm();
	//Udp���ݽ��մ���
	void OnUdpReceived(uchar *, int len);
	//�ص�������
	static int FeedBack(CTask* pTask, void* pData);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	//ͨ��RS422�·���������
	void SendSPCmdToDown(uchar cmd, uchar data1=0, uchar data2=0);
	//YWJ----------------
	//ͨ��UDP�·�ʱ�����������
	void SendUDPCmdToDownA(uchar cmd, uchar data1, uchar data2);
	void SendUDPCmdToDownB(uchar cmd, uchar data1, uchar data2);

	//����ʱ�����UDP����
	void OnUdpReceivedFromSXA(uchar* pData, int len);
	void OnUdpReceivedFromSXB(uchar* pData, int len);
	//����������λ���Ĵ����ַ���
	int AnalyzStringFromUp(CString s);

	CWinThread* hThreadRead;
	//UDP���ݴ��������
	CUdpHelper udpHelper;
	//ʱ����ƿ�A ���ݴ��������
	CUdpHelper udpAsxHelper;
	//ʱ����ƿ�B ���ݴ��������
	CUdpHelper udpBsxHelper;
	//��ʪ�� ���ݴ��������
	CUdpHelper udpWsxHelper;

	CStringArray options;

	//��ǰ������
	CString szCurTaskNo;
	//��¼����״̬��0��δ���⣬1�����ڳ���
	int isStartQg;
	void  splitStrUdp(int cmd, int index, int res);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//�жϴ���toUp�ǲ��ǽ��յ��������ַ���
	int isZuo;
	CString serialStr;

	void InitSerial();
	void OnSerialPortReceived();

	//��ؿ���ָ��m_index
	WORD m_index;

	void WriteLocalStateLog();	//��¼������������
	void WriteUpStateLog();	//��¼��������������

	//��ģ�任����
public:
	void SetScale(int scale);
	float TransformScale(float num);

private:
	int scale;
	//�任����͹�ģ
public:
	float TransformCoordinateScaleX(float x);
	float TransformCoordinateScaleY(float y);

	//���û��Ƶĺ�������
public:
	void SetDrawFunction(Function fun);
private:
	Function fun;

	//������Χ�Ͳ���
public:
	void SetPlotSin(float startX, float endX, float step);
private:
	float startX, endX, step;
CBitmap* draw_Bitmap(vector<vector<int>>& v);
};


