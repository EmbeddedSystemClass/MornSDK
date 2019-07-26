// DlgTab2.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MornSDK.h"
#include "DlgTab2.h"
#include "afxdialogex.h"
#include "grid/GridCtrl.h"
#include "grid/GridCellCombo.h"
#include "CanManager.h"
#include "General.h"
#include "MornSDKDlg.h"


HANDLE	g_hDeviceBat;
/*****************************************************************************
 DESCRIPTION: Send and rceive status
*****************************************************************************/
int     g_nSendNum;
int     g_nSendTimeOut;
int     g_nRcvNum;
int     g_nRcvTimeOut;

/*****************************************************************************
 DESCRIPTION: Read thread
*****************************************************************************/
UINT ThreadReadBat(void *param)
{
		DWORD	ReceiveFlag = 0;
		DWORD   CobId = 0;
		DWORD	Length = 0;
		BYTE	Data[8];
		DWORD	dwRead=0;
		CString strTemp, str;
		int     ret;
		BYTE	byBuf[100];
		OVERLAPPED ov;

		while( g_bBatThreadReadFlag )
		{
			//û�豸��ͨ������ר�ã���������ע��
			//g_pMainDlg->m_Page2.test();

			if(g_pMainDlg->m_Page2.m_pBatStatusReceive.GetCount() > 1000)
				g_pMainDlg->m_Page2.m_pBatStatusReceive.DeleteString(g_pMainDlg->m_Page2.m_pBatStatusReceive.GetCount() - 1);
			if(((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->GetCount() > 15)
			{
				((CListBox*)(g_pMainDlg->m_Page2.GetDlgItem(IDC_ReceivePowerState)))->ResetContent();
			}

			ZeroMemory(byBuf, 100);
			canmsg_t *pBuf = (canmsg_t *)byBuf;
			memset(&ov, 0, sizeof(OVERLAPPED));
			ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if(g_pCanManager4 != NULL)
			{
				ret = acCanRead(g_pCanManager4->m_hDevice, pBuf, 1, &dwRead, &ov);

				CloseHandle(ov.hEvent);
				if(ret == SUCCESS || ret==TIME_OUT)
				{
					if(ret == TIME_OUT)
					{
						if(TRUE)
							g_nRcvTimeOut ++;
					}

					if(pBuf->id != ERRORID)
						g_nRcvNum += dwRead;
				}
				else{
					//continue;
				}
				if(dwRead > 0 )
				{
					ReceiveFlag	= pBuf->flags;
					CobId		= pBuf->id;
					Length		= pBuf->length;
					memcpy(Data, pBuf->data, 8);

					strTemp.Format("No.%-5d ID:%-3x Len%d: %02X %02X %02X %02X %02X %02X %02X %02X",g_nRcvNum, CobId, Length, Data[0], Data[1]\
						,Data[2], Data[3],Data[4], Data[5],Data[6], Data[7] );

					::PostMessageA(g_pMainDlg->m_Page2, WM_DISPYAY_CHANGE, CobId, (LPARAM)Data);
				}
			}
			//Sleep(100);
		}
	//Sleep(2000);
	return 0;
}

// CDlgTab2 �Ի���

IMPLEMENT_DYNAMIC(CDlgTab2, CDialogEx)

CDlgTab2::CDlgTab2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTab2::IDD, pParent)
	, m_BatRcvNum(0)
{
	g_bExitWriteThreadFlag = TRUE;

}

CDlgTab2::~CDlgTab2()
{
}

void CDlgTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_Grid_battery, m_pBattery);
	DDX_GridControl(pDX, IDC_gridChargeSet, m_pCharge);
	//DDX_GridControl(pDX, IDC_gridChargeState, m_pChargeState);
	DDX_Control(pDX, IDC_LIST1, m_pBatStatusReceive);
}

// CDlgTab2 ��Ϣ�������
BEGIN_MESSAGE_MAP(CDlgTab2, CDialogEx)
	ON_MESSAGE(WM_DISPYAY_CHANGE, display)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//��ʼ��ҳ�溯��
void CDlgTab2::InitPage()
{
	GridInit();
	//���  CAN3
	g_pCanManager4 = new CCanManager();
	g_pCanManager4->OpenCanPort("CAN5", TRUE);

	//cycle receive
	//DWORD ThreadIDReadBat;
	//hThreadReadBat = AfxBeginThread(ThreadReadBat, &ThreadIDReadBat);
	hThreadReadBat = AfxBeginThread(ThreadReadBat, this);
}

void CDlgTab2::GridInit()
{
	m_pBattery.SetRowCount(33);       // �趨�ܹ�������S
	m_pBattery.SetColumnCount(11);    // �趨�ܹ�������

	m_pBattery.SetColumnWidth(0, 110);
	for(int i = 1; i < 11; i++)
	{
		m_pBattery.SetColumnWidth(i, 45);
		CString str;
		str.Format("%d", i);
		m_pBattery.SetItemText(0,i, str);
	}
	CString neirong[32] = {
		"��ߵ����ѹ", "��ߵ����ѹλ��","���ͨ����", "��͵����ѹ", "��͵����ѹλ��", "���ͨ����", 
		"��ߵ����¶�", "��ߵ����¶�λ��","���ͨ����","��͵����¶�", "��͵����¶�λ��", "���ͨ����", 
		"����ܵ���", "����ܵ�ѹ", "soc", "soh", "�̵���״̬",
		"���ϱ�־λ","���ϱ���1", "���ϱ���2","���ϱ���3", "���ϱ���3", "�Լ���ϱ���", "BMS�����ź�", 
		"��������ѹ", "��������ѹ", "�������ŵ����", "��������������","������������", "��Ե����", "ǿ�ƾ���״̬", "��س��״̬"
	};
	for(int i = 1; i < 33; i++)
	{
		m_pBattery.SetItemText(i,0, neirong[i - 1]);
	}

	m_pBattery.SetRowHeight(0,30);
	m_pBattery.SetFixedRowCount(1);  // �趨�̶�����
	m_pBattery.SetFixedColumnCount(1);  // �趨�̶�����
	m_pBattery.ShowScrollBar(SB_BOTH, FALSE);


	m_pCharge.SetRowCount(11);
	m_pCharge.SetColumnCount(7);
	m_pCharge.SetRowHeight(0, 30);
	m_pCharge.SetColumnWidth(0, 30);
	m_pCharge.SetColumnWidth(1, 90);
	m_pCharge.SetColumnWidth(2, 60);
	m_pCharge.SetColumnWidth(3, 120);
	m_pCharge.SetColumnWidth(4, 120);
	m_pCharge.SetColumnWidth(5, 120);
	m_pCharge.SetColumnWidth(6, 60);

	m_pCharge.SetItemText(0, 0, "id");
	for(int i = 1; i < 11; i++)
	{
		CString str;
		str.Format("%d", i);
		m_pCharge.SetItemText(i, 0, str);
	}
	m_pCharge.SetItemText(0, 1, _T("���ģʽ"));
	m_pCharge.SetItemText(0, 2, _T("����ID"));
	m_pCharge.SetItemText(0, 3, _T("������(0-127.0A)"));
	m_pCharge.SetItemText(0, 4, _T("��繦��(0-6375W)"));
	m_pCharge.SetItemText(0, 5, _T("����ѹ(0-800.0V)"));
	m_pCharge.SetItemText(0, 6, _T("���״̬"));
	for(int i = 1; i < 11; i++)
	{
		m_pCharge.SetItemText(i, 2, "00"); //���IDĬ��  00
		m_pCharge.SetItemText(i, 6, "1"); //���״̬Ĭ�� 1
		m_pCharge.SetItemText(i, 4, "4000"); //��繦��Ĭ��4000w
	}
	for(int i = 1; i < 11; i++)
	{
		m_pCharge.SetCellType(i,1, RUNTIME_CLASS(CGridCellCombo));
		m_pCharge.SetItemText(i,1, _T("�㹦�ʳ��"));
		CStringArray options;
		options.Add(_T("�㹦�ʳ��"));
		options.Add(_T("��ѹ���"));
		options.Add(_T("�������"));
		CGridCellCombo *pCell = (CGridCellCombo*) m_pCharge.GetCell(i,1);
		pCell->SetOptions(options);
		pCell->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
	}

	//m_pChargeState.SetRowCount(11);
	//m_pChargeState.SetColumnCount(7);
	//m_pChargeState.SetRowHeight(0, 30);
	//m_pChargeState.SetColumnWidth(0, 30);
	//m_pChargeState.SetColumnWidth(1, 90);
	//m_pChargeState.SetColumnWidth(2, 60);
	//m_pChargeState.SetColumnWidth(3, 120);
	//m_pChargeState.SetColumnWidth(4, 120);
	//m_pChargeState.SetColumnWidth(5, 120);
	//m_pChargeState.SetColumnWidth(6, 60);

	//m_pChargeState.SetItemText(0, 0, "id");
	//for(int i = 1; i < 11; i++)
	//{
	//	CString str;
	//	str.Format("%d", i);
	//	m_pChargeState.SetItemText(i, 0, str);
	//}
	//m_pChargeState.SetItemText(0, 1, _T("���ģʽ"));
	//m_pChargeState.SetItemText(0, 2, _T("����ID"));
	//m_pChargeState.SetItemText(0, 3, _T("������(0-127.0A)"));
	//m_pChargeState.SetItemText(0, 4, _T("��繦��(0-6375W)"));
	//m_pChargeState.SetItemText(0, 5, _T("����ѹ(0-800.0V)"));
	//m_pChargeState.SetItemText(0, 6, _T("���״̬"));

	for(int i = 0; i < 11; i++)
		m_chargeFlag[i] = FALSE;

	SetTimer(0, 1000, NULL);
}

LRESULT CDlgTab2::display(WPARAM wPARAM, LPARAM lPARAM)
{    
	if(!g_bBatThreadReadFlag)	return 0;
	//BYTE id[5] = {0x180102fe, 0x180202fe, 0x180302fe, 0x180402fe, 0x180502fe};
	//BYTE testData[8] = {0xd0, 0x07, 0xda, 0x0e, 0x2c, 0x01, 0xfa, 0x00};
	//BYTE	byBuf[100];
	//BYTE data[8];
	//ZeroMemory(byBuf, 100);
	//BYTE *pBuf = (BYTE*)byBuf;
	//memcpy(data, pBuf, 8);
	//memcpy(data, (BYTE*)lPARAM, 8);
	BYTE *data = (BYTE*)lPARAM;
	//if(data == NULL || sizeof(data) != 8)
	//	return 0;x

	DWORD sub = ((DWORD)wPARAM) & ((DWORD)0x00000f00); // ѡ�񼸺ŵ��
	DWORD CobId = (DWORD)wPARAM;
	int i; // ���ŵ��
	try
	{
		CString strTemp;
		strTemp.Format("%02X %02X %02X %02X %02X %02X %02X %02X",data[0], data[1]\
			,data[2], data[3],data[4], data[5],data[6], data[7] );

		m_pBatStatusReceive.InsertString(0, strTemp);
	}
	catch (CException* e)
	{
		g_Log.WriteLog(LOG_DBG, (char*)e);
	}

	m_BatRcvNum++;

	switch(sub)
	{
	case 0x00000100:
		{
			CString str;
			i = 1;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);//ѡ�����ݰ�
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "1�ŵ���ܵ���: " + str; //�������� 1�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("1�ŵ���ܵ���: " + str); //�������� 1�ŵ�� �ܵ���

					g_stateToUp[94+(i-1)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[95+(i-1)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[96+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[97+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[98+(i-1)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "1�ŵ���ܵ�ѹ: " + str; //�������� 1�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("1�ŵ���ܵ�ѹ: " + str); //�������� 1�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "1�ŵ����soc: " + str; //�������� 1�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("1�ŵ����soc: " + str); //�������� 1�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "1�ŵ����soh: " + str; //�������� 1�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("1�ŵ����soh: " + str); //�������� 1�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "1�ŵ�ؼ̵���״̬: " + tmp; //�������� 1�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("1�ŵ�ؼ̵���״̬: " + tmp); //�������� 1�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[103+(i-1)*10]=0x12; //����Ԥ��
						g_stateToUp[101+(i-1)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[103+(i-1)*10]=0x10;
						g_stateToUp[101+(i-1)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[103+(i-1)*10] =0x14 ; //����ŵ�����
						g_stateToUp[101+(i-1)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�

					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[103+(i-1)*10] =0x08; //����������
					   g_stateToUp[101+(i-1)*10]=5;
					   g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[102+(i-1)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[102+(i-1)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[102+(i-1)*10] = 1; //�澯����
						g_stateToUp[101+(i-1)*10]=7;
						
					}
					else	if((data[0]>>4)&1)	
						{
							g_stateToUp[102+(i-1)*10] = 0; //�澯����
							
					   }
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
					CString strTemp;
					strTemp.Format("%02X %02X %02X %02X %02X %02X %02X %02X",data[0], data[1]\
						,data[2], data[3],data[4], data[5],data[6], data[7] );
					//g_Log.WriteLog(LOG_INF, "BMS�����źŲ��ԣ�" + strTemp);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[99+(i-1)*10] = data[5]%256;	//��Ե����
					g_stateToUp[100+(i-1)*10] = data[5]/256;	//��Ե����
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000200:
		{
			CString str;
			i = 2;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "2�ŵ���ܵ���: " + str; //�������� 2�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("2�ŵ���ܵ���: " + str); //�������� 2�ŵ�� �ܵ���

					g_stateToUp[94+(i-1)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[95+(i-1)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[96+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[97+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[98+(i-1)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "2�ŵ���ܵ�ѹ: " + str; //�������� 2�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("2�ŵ���ܵ�ѹ: " + str); //�������� 2�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "2�ŵ����soc: " + str; //�������� 2�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("2�ŵ����soc: " + str); //�������� 2�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "2�ŵ����soh: " + str; //�������� 2�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("2�ŵ����soh: " + str); //�������� 2�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "2�ŵ�ؼ̵���״̬: " + tmp; //�������� 2�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("2�ŵ�ؼ̵���״̬: " + tmp); //�������� 2�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[103+(i-1)*10]=0x12; //����Ԥ��
						g_stateToUp[101+(i-1)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�	
					}
					else if(data[7]==0) 
					{
						g_stateToUp[103+(i-1)*10]=0x10;
						g_stateToUp[101+(i-1)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[103+(i-1)*10] =0x14 ; //����ŵ�����
						g_stateToUp[101+(i-1)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[103+(i-1)*10] =0x08; //����������
						g_stateToUp[101+(i-1)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[102+(i-1)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[102+(i-1)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[102+(i-1)*10] = 1; //�澯����
						g_stateToUp[101+(i-1)*10]=7;
						
					}
					else	if((data[0]>>4)&1)	g_stateToUp[102+(i-1)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
					CString strTemp;
					strTemp.Format("%02X %02X %02X %02X %02X %02X %02X %02X",data[0], data[1]\
						,data[2], data[3],data[4], data[5],data[6], data[7] );
					//g_Log.WriteLog(LOG_INF, "BMS�����źŲ��ԣ�" + strTemp);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[99+(i-1)*10] = data[5]%256;	//��Ե����
					g_stateToUp[100+(i-1)*10] = data[5]/256;	//��Ե����
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000300:
		{
			CString str;
			i = 3;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "3�ŵ���ܵ���: " + str; //�������� 3�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("3�ŵ���ܵ���: " + str); //�������� 3�ŵ�� �ܵ���

					g_stateToUp[94+(i-1)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[95+(i-1)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[96+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[97+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[98+(i-1)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "3�ŵ���ܵ�ѹ: " + str; //��������     3�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("3�ŵ���ܵ�ѹ: " + str); //��������     3�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "3�ŵ����soc: " + str; //��������      3�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("3�ŵ����soc: " + str); //��������      3�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "3�ŵ����soh: " + str; //��������      3�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("3�ŵ����soh: " + str); //��������      3�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "3�ŵ�ؼ̵���״̬: " + tmp; //�������� 3�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("3�ŵ�ؼ̵���״̬: " + tmp); //�������� 3�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[103+(i-1)*10]=0x12; //����Ԥ��
						g_stateToUp[101+(i-1)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[103+(i-1)*10]=0x10;
						g_stateToUp[101+(i-1)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[103+(i-1)*10] =0x14 ; //����ŵ�����
						g_stateToUp[101+(i-1)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[103+(i-1)*10] =0x08; //����������
						g_stateToUp[101+(i-1)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[102+(i-1)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[102+(i-1)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[102+(i-1)*10] = 1; //�澯����
						g_stateToUp[101+(i-1)*10]=7;
					}
					else	if((data[0]>>4)&1)	g_stateToUp[102+(i-1)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
					CString strTemp;
					strTemp.Format("%02X %02X %02X %02X %02X %02X %02X %02X",data[0], data[1]\
						,data[2], data[3],data[4], data[5],data[6], data[7] );
					//g_Log.WriteLog(LOG_INF, "BMS�����źŲ��ԣ�" + strTemp);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[99+(i-1)*10] = data[5]%256;	//��Ե����
					g_stateToUp[100+(i-1)*10] = data[5]/256;	//��Ե����
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000400:
		{
			CString str;
			i = 4;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "4�ŵ���ܵ���: " + str; //��������     4�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("4�ŵ���ܵ���: " + str); //��������     4�ŵ�� �ܵ���

					g_stateToUp[94+(i-1)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[95+(i-1)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[96+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[97+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[98+(i-1)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "4�ŵ���ܵ�ѹ: " + str; //��������     4�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("4�ŵ���ܵ�ѹ: " + str); //��������     4�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "4�ŵ����soc: " + str; //��������      4�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("4�ŵ����soc: " + str); //��������      4�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "4�ŵ����soh: " + str; //��������      4�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("4�ŵ����soh: " + str); //��������      4�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "4�ŵ�ؼ̵���״̬: " + tmp; //�������� 4�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("4�ŵ�ؼ̵���״̬: " + tmp); //�������� 4�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[103+(i-1)*10]=0x12; //����Ԥ��
						g_stateToUp[101+(i-1)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[103+(i-1)*10]=0x10;
						g_stateToUp[101+(i-1)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[103+(i-1)*10] =0x14 ; //����ŵ�����
						g_stateToUp[101+(i-1)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[103+(i-1)*10] =0x08; //����������
						g_stateToUp[101+(i-1)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[102+(i-1)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[102+(i-1)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[102+(i-1)*10] = 1; //�澯����
						g_stateToUp[101+(i-1)*10]=7;
					}
					else	if((data[0]>>4)&1)	g_stateToUp[102+(i-1)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
					CString strTemp;
					strTemp.Format("%02X %02X %02X %02X %02X %02X %02X %02X",data[0], data[1]\
						,data[2], data[3],data[4], data[5],data[6], data[7] );
					//g_Log.WriteLog(LOG_INF, "BMS�����źŲ��ԣ�" + strTemp);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[99+(i-1)*10] = data[5]%256;	//��Ե����
					g_stateToUp[100+(i-1)*10] = data[5]/256;	//��Ե����
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000500:
		{
			CString str;
			i = 5;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "5�ŵ���ܵ���: " + str; //��������     5�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("5�ŵ���ܵ���: " + str); //��������     5�ŵ�� �ܵ���

					g_stateToUp[94+(i-1)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[95+(i-1)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[96+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[97+(i-1)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[98+(i-1)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "5�ŵ���ܵ�ѹ: " + str; //��������     5�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("5�ŵ���ܵ�ѹ: " + str); //��������     5�ŵ�� �ܵ�ѹ					
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					g_localDataLog[34 + (i - 1)*5] = "5�ŵ����soc: " + str; //��������      5�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "5�ŵ����soh: " + str; //��������      5�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("5�ŵ����soh: " + str); //��������      5�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "5�ŵ�ؼ̵���״̬: " + tmp; //�������� 5�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("5�ŵ�ؼ̵���״̬: " + tmp); //�������� 5�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[103+(i-1)*10]=0x12; //����Ԥ��
						g_stateToUp[101+(i-1)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[103+(i-1)*10]=0x10;
						g_stateToUp[101+(i-1)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[103+(i-1)*10] =0x14 ; //����ŵ�����
						g_stateToUp[101+(i-1)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[103+(i-1)*10] =0x08; //����������
						g_stateToUp[101+(i-1)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[102+(i-1)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[102+(i-1)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[102+(i-1)*10] = 1; //�澯����
						g_stateToUp[101+(i-1)*10]=7;
					}
					else	if((data[0]>>4)&1)	g_stateToUp[102+(i-1)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
					CString strTemp;
					strTemp.Format("%02X %02X %02X %02X %02X %02X %02X %02X",data[0], data[1]\
						,data[2], data[3],data[4], data[5],data[6], data[7] );
					//g_Log.WriteLog(LOG_INF, "BMS�����źŲ��ԣ�" + strTemp);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[99+(i-1)*10] = data[5]%256;	//��Ե����
					g_stateToUp[100+(i-1)*10] = data[5]/256;	//��Ե����
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000600:
		{
			CString str;
			i = 6;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "6�ŵ���ܵ���: " + str; //��������     6�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("6�ŵ���ܵ���: " + str); //��������     6�ŵ�� �ܵ���

					g_stateToUp[238+(i-6)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[239+(i-6)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[240+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[241+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[242+(i-6)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "6�ŵ���ܵ�ѹ: " + str; //��������     6�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("6�ŵ���ܵ�ѹ: " + str); //��������     6�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "6�ŵ����soc: " + str; //��������      6�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("6�ŵ����soc: " + str); //��������      6�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "6�ŵ����soh: " + str; //��������      6�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("6�ŵ����soh: " + str); //��������      6�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "6�ŵ�ؼ̵���״̬: " + tmp; //�������� 6�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("6�ŵ�ؼ̵���״̬: " + tmp); //�������� 6�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[247+(i-6)*10]=0x12; //����Ԥ��
						g_stateToUp[245+(i-6)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[247+(i-6)*10]=0x10;
						g_stateToUp[245+(i-6)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[247+(i-6)*10] =0x14 ; //����ŵ�����
						g_stateToUp[245+(i-6)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[247+(i-6)*10] =0x08; //����������
						g_stateToUp[245+(i-6)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[246+(i-6)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[246+(i-6)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[246+(i-6)*10] = 1; //�澯����
						g_stateToUp[245+(i-6)*10]=7;
					}
					else	if((data[0]>>4)&1)	g_stateToUp[246+(i-6)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[243+(i-6)*10] = data[5]%256;	//��Ե����
					g_stateToUp[244+(i-6)*10] = data[5]/256;
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000700:
		{
			CString str;
			i = 7;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "7�ŵ���ܵ���: " + str; //��������     7�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("7�ŵ���ܵ���: " + str); //��������     7�ŵ�� �ܵ���

					g_stateToUp[238+(i-6)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[239+(i-6)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[240+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[241+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[242+(i-6)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "7�ŵ���ܵ�ѹ: " + str; //��������     7�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("7�ŵ���ܵ�ѹ: " + str); //��������     7�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "7�ŵ����soc: " + str; //��������      7�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("7�ŵ����soc: " + str); //��������      7�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "7�ŵ����soh: " + str; //��������      7�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("7�ŵ����soh: " + str); //��������      7�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "7�ŵ�ؼ̵���״̬: " + tmp; //�������� 7�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("7�ŵ�ؼ̵���״̬: " + tmp); //�������� 7�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[247+(i-6)*10]=0x12; //����Ԥ��
						g_stateToUp[245+(i-6)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[247+(i-6)*10]=0x10;
						g_stateToUp[245+(i-6)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[247+(i-6)*10] =0x14 ; //����ŵ�����
						g_stateToUp[245+(i-6)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[247+(i-6)*10] =0x08; //����������
						g_stateToUp[245+(i-6)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[246+(i-6)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[246+(i-6)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[246+(i-6)*10] = 1; //�澯����
						g_stateToUp[245+(i-6)*10]=7;
					}
					else	if((data[0]>>4)&1)	g_stateToUp[246+(i-6)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[243+(i-6)*10] = data[5]%256;	//��Ե����
					g_stateToUp[244+(i-6)*10] = data[5]/256;
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000800:
		{
			CString str;
			i = 8;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "8�ŵ���ܵ���: " + str; //��������     8�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("8�ŵ���ܵ���: " + str); //��������     8�ŵ�� �ܵ���

					g_stateToUp[238+(i-6)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[239+(i-6)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[240+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[241+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[242+(i-6)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "8�ŵ���ܵ�ѹ: " + str; //��������     8�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("8�ŵ���ܵ�ѹ: " + str); //��������     8�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "8�ŵ����soc: " + str; //��������      8�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("8�ŵ����soc: " + str); //��������      8�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "8�ŵ����soh: " + str; //��������      8�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("8�ŵ����soh: " + str); //��������      8�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "8�ŵ�ؼ̵���״̬: " + tmp; //�������� 8�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("8�ŵ�ؼ̵���״̬: " + tmp); //�������� 8�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[247+(i-6)*10]=0x12; //����Ԥ��
						g_stateToUp[245+(i-6)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[247+(i-6)*10]=0x10;
						g_stateToUp[245+(i-6)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[247+(i-6)*10] =0x14 ; //����ŵ�����
						g_stateToUp[245+(i-6)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[247+(i-6)*10] =0x08; //����������
						g_stateToUp[245+(i-6)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[246+(i-6)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[246+(i-6)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[246+(i-6)*10] = 1; //�澯����
						g_stateToUp[245+(i-6)*10]=7;
					}
					else	if((data[0]>>4)&1)	g_stateToUp[246+(i-6)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[243+(i-6)*10] = data[5]%256;	//��Ե����
					g_stateToUp[244+(i-6)*10] = data[5]/256;
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000900:
		{
			CString str;
			i = 9;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "9�ŵ���ܵ���: " + str; //��������     9�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("9�ŵ���ܵ���: " + str); //��������     9�ŵ�� �ܵ���

					g_stateToUp[238+(i-6)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[239+(i-6)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[240+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[241+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[242+(i-6)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "9�ŵ���ܵ�ѹ: " + str; //��������     9�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("9�ŵ���ܵ�ѹ: " + str); //��������     9�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "9�ŵ����soc: " + str; //��������      9�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("9�ŵ����soc: " + str); //��������      9�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "9�ŵ����soh: " + str; //��������      9�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("9�ŵ����soh: " + str); //��������      9�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "9�ŵ�ؼ̵���״̬: " + tmp; //�������� 9�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("9�ŵ�ؼ̵���״̬: " + tmp); //�������� 9�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[247+(i-6)*10]=0x12; //����Ԥ��
						g_stateToUp[245+(i-6)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[247+(i-6)*10]=0x10;
						g_stateToUp[245+(i-6)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[247+(i-6)*10] =0x14 ; //����ŵ�����
						g_stateToUp[245+(i-6)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[247+(i-6)*10] =0x08; //����������
						g_stateToUp[245+(i-6)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[246+(i-6)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[246+(i-6)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[246+(i-6)*10] = 1; //�澯����
						g_stateToUp[245+(i-6)*10]=7;
					}
					else	if((data[0]>>4)&1)	g_stateToUp[246+(i-6)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[243+(i-6)*10] = data[5]%256;	//��Ե����
					g_stateToUp[244+(i-6)*10] = data[5]/256;
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
		}
		break;
	case 0x00000a00:
		{
			CString str;
			i = 10;
			DWORD choose = ((DWORD)wPARAM) & ((DWORD)0x000f0000);
			switch(choose)
			{
			case 0x00010000:
				{
					//��ߵ����ѹ
					str.Format("%.3f", (float)((data[0] + data[1]*256))/1000);
					m_pBattery.SetItemText(1, i, str);
					//��ߵ����ѹλ��
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(2, i, str);
					//���ͨ����
					str.Format("%d",  data[3]);
					m_pBattery.SetItemText(3, i, str);
					//��͵����ѹ
					str.Format("%.3f", (float)((data[4] + data[5]*256))/1000);
					m_pBattery.SetItemText(4, i, str);
					//��͵����ѹλ��
					str.Format("%d", data[6]);
					m_pBattery.SetItemText(5, i, str);
					//���ͨ����
					str.Format("%d", data[7]);
					m_pBattery.SetItemText(6, i, str);
				}
				break;
			case 0x00020000:
				{
					//��ߵ����¶�
					str.Format("%.1f", ((float)(int)data[0])/2 - 45);
					m_pBattery.SetItemText(7, i, str);
					//��ߵ����¶�λ��
					str.Format("%d", data[1]);
					m_pBattery.SetItemText(8, i, str);
					//���ͨ����
					str.Format("%d", data[2]);
					m_pBattery.SetItemText(9, i, str);
					//��͵����¶�
					str.Format("%.1f", ((float)(int)data[3])/2 - 45);
					m_pBattery.SetItemText(10, i, str);
					//��͵����¶�λ��
					str.Format("%d", data[4]);
					m_pBattery.SetItemText(11, i, str);
					//���ͨ����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(12, i, str);
				}
				break;
			case 0x00030000:
				{
					//����ܵ���
					str.Format("%.1f", (float)(data[0] + data[1]*256)/2-1000);
					//g_localDataLog[32 + (i - 1)*5] = "10�ŵ���ܵ���: " + str; //��������     10�ŵ�� �ܵ���
					g_localDataLog[32 + (i - 1)*5].SetString("10�ŵ���ܵ���: " + str); //��������     10�ŵ�� �ܵ���

					g_stateToUp[238+(i-6)*10] = ((data[2] + data[3]*256)/10)%256;	//�ܵ�ѹ
					g_stateToUp[239+(i-6)*10] = ((data[2] + data[3]*256)/10)/256;
					g_stateToUp[240+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)%256; //�ܵ���
					g_stateToUp[241+(i-6)*10] = ((data[0] + data[1]*256)/2-1000)/256;
					g_stateToUp[242+(i-6)*10] = (data[4] + data[5]*256)/10;			//soc
					m_pBattery.SetItemText(13, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 2, str);
					//����ܵ�ѹ
					str.Format("%.1f", (float)((data[2] + data[3]*256))/10);
					m_pBattery.SetItemText(14, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 3, str);
					//g_localDataLog[33 + (i - 1)*5] = "10�ŵ���ܵ�ѹ: " + str; //��������     10�ŵ�� �ܵ�ѹ
					g_localDataLog[33 + (i - 1)*5].SetString("10�ŵ���ܵ�ѹ: " + str); //��������     10�ŵ�� �ܵ�ѹ
					//soc
					str.Format("%.1f", (float)((data[4] + data[5]*256))/10);
					m_pBattery.SetItemText(15, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 4, str);
					//g_localDataLog[34 + (i - 1)*5] = "10�ŵ����soc: " + str; //��������      10�ŵ�� soc
					g_localDataLog[34 + (i - 1)*5].SetString("10�ŵ����soc: " + str); //��������      10�ŵ�� soc
					//soh
					str.Format("%.1f", (float)((data[6]))*0.4);
					m_pBattery.SetItemText(16, i, str);
					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 5, str);
					//g_localDataLog[35 + (i - 1)*5] = "10�ŵ����soh: " + str; //��������      10�ŵ�� soh
					g_localDataLog[35 + (i - 1)*5].SetString("10�ŵ����soh: " + str); //��������      10�ŵ�� soh
					//�̵���״̬
					str.Format("%02X", data[7]);
					//m_pBattery.SetItemText(17, i, str);
					CString tmp1  =  (data[7]&1) ? "����" : "����";
					CString tmp2  =	 (data[7]>>2 & 1) ? "����" : "����";
					CString tmp3  =	 (data[7]>>4 & 1) ? "Ԥ��" : "Ԥ��";
					CString tmp4  =	 (data[7]>>6 & 1) ? "���" : "���";
					CString tmp = tmp1 + tmp2 + tmp3 + tmp4;
					m_pBattery.SetItemText(17, i, tmp);
					//g_localDataLog[36 + (i - 1)*5] = "10�ŵ�ؼ̵���״̬: " + tmp; //�������� 10�ŵ�� �̵���״̬
					g_localDataLog[36 + (i - 1)*5].SetString("10�ŵ�ؼ̵���״̬: " + tmp); //�������� 10�ŵ�� �̵���״̬

					g_pMainDlg->m_Page1.m_pReceiveBattery.SetItemText(i, 6, tmp);
					m_chargeFlag[i] = (str == "40" ? TRUE : FALSE);
					g_batState[i] = data[7];
					g_batVoltage[i] = (data[2] + data[3]*256)/10;
					if(((data[7]>>4)&1)&&(data[7]>>2 & 1)) 
					{
						g_stateToUp[247+(i-6)*10]=0x12; //����Ԥ��
						g_stateToUp[245+(i-6)*10]=4;
						g_yuchongOK[i] = TRUE;
						g_BatRelayState[i] = 1; //Ԥ�� �պ�
					}
					else if(data[7]==0) 
					{
						g_stateToUp[247+(i-6)*10]=0x10;
						g_stateToUp[245+(i-6)*10]=3;
						g_BatRelayState[i] = 0; //��������״̬
					}
					else if (data[7]&1)
					{
						g_isbatDisChargeState[i] = TRUE;
						g_stateToUp[247+(i-6)*10] =0x14 ; //����ŵ�����
						g_stateToUp[245+(i-6)*10]=5;
						g_yuchongOK[i] = FALSE;
						g_BatRelayState[i] = 2; //���ڷŵ�
					}
					else if (((float)(data[0] + data[1]*256)/2-1000 > 0)&&(data[7]>>6 & 1))
					{
						g_stateToUp[247+(i-6)*10] =0x08; //����������
						g_stateToUp[245+(i-6)*10]=5;
						g_BatRelayState[i] = 3; //���ڳ��
					}
				}
				break;
			case 0x00040000:
				{
					if(data[0]&1)	g_stateToUp[246+(i-6)*10] = 3; //�澯����
					else	if((data[0]>>2)&1)	g_stateToUp[246+(i-6)*10] = 2; //�澯����
					else	if((data[0]>>4)&1)	
					{ 
						g_stateToUp[246+(i-6)*10] = 1; //�澯����
						g_stateToUp[245+(i-6)*10]=7;
					}
					else	if((data[0]>>4)&1)	g_stateToUp[246+(i-6)*10] = 0; //�澯����
					//���ϱ�־λ
					str.Format("%02X", data[0]);
					m_pBattery.SetItemText(18, i, str);
					//���ϱ���1
					str.Format("%02X", data[1]);
					m_pBattery.SetItemText(19, i, str);
					//���ϱ���2
					str.Format("%02X", data[2]);
					m_pBattery.SetItemText(20, i, str);
					//���ϱ���3
					str.Format("%02X", data[3]);
					m_pBattery.SetItemText(21, i, str);
					//���ϱ���4
					str.Format("%02X", data[4]);
					m_pBattery.SetItemText(22, i, str);
					//�Լ���ϱ���
					str.Format("%02X", data[5]);
					m_pBattery.SetItemText(23, i, str);
					//BMS�����ź�
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(24, i, str);
				}
				break;
			case 0x00050000:
				{
					//��������ѹ
					str.Format("%d", data[0]*4);
					m_pBattery.SetItemText(25, i, str);
					//��������ѹ
					str.Format("%d", data[1]*4);
					m_pBattery.SetItemText(26, i, str);
					//�������ŵ����
					str.Format("%d", data[2]*4);
					m_pBattery.SetItemText(27, i, str);
					//��������������
					str.Format("%d", data[3]*4);
					m_pBattery.SetItemText(28, i, str);
					//������������
					str.Format("%d", data[4]*4);
					m_pBattery.SetItemText(29, i, str);
					//��Ե����
					str.Format("%d", data[5]);
					m_pBattery.SetItemText(30, i, str);
					g_stateToUp[243+(i-6)*10] = data[5]%256;	//��Ե����
					g_stateToUp[244+(i-6)*10] = data[5]/256;
					//ǿ�ƾ���״̬
					str.Format("%02X", data[6]);
					m_pBattery.SetItemText(31, i, str);
					//��س��״̬
					str.Format("%02X", data[7]);
					m_pBattery.SetItemText(32, i, str);
				}
				break;
			default:
				break;
			}
			
		}
		break;
	default:
		break;
	}
	//g_pMainDlg->m_Page1.m_pReceiveBattery.Refresh();
	//m_pBattery.Refresh();
	return 0;
}


//û�豸�����ã����豸��Ҫע��
void CDlgTab2::test()
{
	//û�豸��ͨ������ר�ã���������ע��
	DWORD id[10][5] = {
		{	
			0x180101fe, 
				0x180201fe, 
				0x180301fe, 
				0x180401fe, 
				0x180501fe
		},
		{	
			0x180102fe, 
				0x180202fe, 
				0x180302fe, 
				0x180402fe, 
				0x180502fe
			},
			{	
				0x180103fe, 
					0x180203fe, 
					0x180303fe, 
					0x180403fe, 
					0x180503fe
			},
			{	
				0x180104fe, 
					0x180204fe, 
					0x180304fe, 
					0x180404fe, 
					0x180504fe
				},
				{	
					0x180105fe, 
						0x180205fe, 
						0x180305fe, 
						0x180405fe, 
						0x180505fe
				},
				{	
					0x180106fe, 
						0x180206fe, 
						0x180306fe, 
						0x180406fe, 
						0x180506fe
					},
					{	
						0x180107fe, 
							0x180207fe, 
							0x180307fe, 
							0x180407fe, 
							0x180507fe
					},
					{	
						0x180108fe, 
							0x180208fe, 
							0x180308fe, 
							0x180408fe, 
							0x180508fe
						},
						{	
							0x180109fe, 
								0x180209fe, 
								0x180309fe, 
								0x180409fe, 
								0x180509fe
						},
						{	
							0x18010afe, 
								0x18020afe, 
								0x18030afe, 
								0x18040afe, 
								0x18050afe
							}
	};


	BYTE testData[5][8] = {
		{0x73, 0x08, 0x02, 0x02, 0x68, 0x08, 0x01, 0x19},
		{0x7e, 0x03, 0x05, 0x7a, 0x02, 0x04, 0x00, 0x00},
		{0xd0, 0x07, 0xda, 0x0e, 0x2c, 0x01, 0xfa, 0x00},
		{0x10, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0xeb},
		{0x58, 0x2d, 0x3e, 0x3e, 0x0f, 0x41, 0x00, 0x00},
	};
	for (int i = 0; i < 10; i++)
	{
		for (int j =0; j < 5; j++)
		{
			::PostMessageA(g_pMainDlg->m_Page2, WM_DISPYAY_CHANGE, id[i][j], (LPARAM)testData[j]); 
		}
		Sleep(100);
	}
	
	//::PostMessageA(g_pMainDlg->m_Page2, WM_DISPYAY_CHANGE, id[2], (LPARAM)testData);
	//::PostMessageA(g_pMainDlg->m_Page1, WM_DISPYAY_RECEIVE_BATETTY, id[2], (LPARAM)testData);
	//::SendMessageA(g_pMainDlg->m_Page1, WM_DISPYAY_RECEIVE_BATETTY, id[2], (LPARAM)testData);
}


int CDlgTab2::GetIndexOfParameterGrid(int x, int y)
{
	int index = ((CGridCellCombo*)(g_pMainDlg->m_Page2.m_pCharge.GetCell(x, y)))->getCurSel();
	return index;
}


CString CDlgTab2::GetValueOfParameterGrid(int x, int y)
{
	CString tmp = g_pMainDlg->m_Page2.m_pCharge.GetItemText(x, y);
	return tmp;
}


void CDlgTab2::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch(nIDEvent)
	{
	case 0:
		{
			m_pBattery.Refresh();
			CString tmp;
			tmp.Format("%d", m_BatRcvNum);
			//((CEdit*)GetDlgItem(IDC_EDIT_BatRcvNum))->SetItemText(tmp);
			SetDlgItemText(IDC_EDIT_BatRcvNum, tmp);
			m_BatRcvNum = 0;
		}
		
		break;
	default :
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
