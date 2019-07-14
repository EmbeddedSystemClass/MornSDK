/*
 * MyLog.h
 *
 *  Created on: 2018��12��16��
 *      Author: YWJ
 */

#ifndef MYLOG_H_
#define MYLOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>



const int MAX_LOG_LENGTH = 1024;

/* ��־�����ȼ� */
typedef enum
{
	LOG_NONE= 0,    	//�ر���־
	LOG_FTL = 1,      	//����
	LOG_ERR = 2,    	//����,����һ���޷��޸��Ĵ���
	LOG_WRN = 3,    	//����,��ʱ����һЩ�޸��ԵĹ���,ϵͳ�ܹ��ָ�,���Լ���������ȥ
	LOG_INF = 4,    	//��Ϣ,����ϵͳ�ĵ�ǰ״̬�������û���
	LOG_DBG = 5	    	//����,��ʵ�����й����в����
} LOG_LEVEL;


//��ͬ�ȼ���Ӧ���ַ���
const char LOG_LEVELNAME[][8] = {"NONE", "FTL", "ERR", "WRN", "INF", "DBG"};

class CMyLog
{
public:
    CMyLog();
    ~CMyLog();
    //��ʼ��
    void Init();
    // ��־��ӡ�ӿڣ������ɱ�
    void WriteLog(LOG_LEVEL level, const char* format,...);
    // ������־
    void ClearLogfile(int day);
protected:
    void PrintLog(const char* szMsg); // ��ӡ�����Ϣ
    void GetDateString(char* szDate); // �������, ʵ��ʡ��
    void GetTimeString(char* szTime); // ���ʱ�䣬ʵ��ʡ��
private:
    FILE *m_file; // ��־�ļ�
    LOG_LEVEL m_iLevel;
    //�ٽ���
	CCriticalSection critical_section;
    //pthread_mutex_t mutexLock;
};

#endif /* MYLOG_H_ */
