#pragma once

// CMIC3780 ����Ŀ��
//YWJ----------------
#pragma once
#include "./MIC3780/include/Driver.h"

class CMIC3780 : public CObject
{
public:
	CMIC3780();
	virtual ~CMIC3780();

	//��IO��
	LRESULT OpenIOCard(ULONG lDevNum);
	//�ر�IO��
	void CloseIOCard();
	//��ͨ�������
	BOOL WriteWord(int chl, ushort wd);

	USHORT OnReadback();

public:
	LONG        m_hDev;							//�豸���
	PT_DioWritePortWord m_ptDioWriteWord;
	PT_DioGetCurrentDOByte m_ptDioGetCurrentDOByte;
	PT_DioWritePortByte m_ptDioWritePortByte;	// DioWritePortByte table
	PT_DioWriteBit      m_ptDioWriteBit;		// DioWriteBit table
	LRESULT m_ErrCde;                                 //Return Eror Code
	char m_szErrMsg[80];                              //Use for MessageBox function
};


