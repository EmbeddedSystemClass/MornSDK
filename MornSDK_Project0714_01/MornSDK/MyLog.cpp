/*
 * MyLog.cpp
 *
 *  Created on: 2018��12��16��
 *      Author: YWJ
 */
#include "General.h"

//���캯��
CMyLog::CMyLog()
{
	//��ʼ������
	m_iLevel = LOG_NONE;
	m_file = NULL;
}

//��������
CMyLog::~CMyLog()
{
	if (NULL != m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
	//���ٻ�����
	//pthread_mutex_destroy(&mutexLock);
}

void CMyLog::Init()
{
	m_iLevel = (LOG_LEVEL)_ttoi(g_Config.GetKeyValue("LogLevel","System"));
	//��ʼ��������
	//pthread_mutex_init(&mutexLock,NULL);
}

void CMyLog::WriteLog(LOG_LEVEL level, const char* format, ...)
{
	// ������
	if (NULL == format)
	{
		return;
	}
	char chTmp[20] = { 0 };            		//ʱ���ַ���
	char szMsg[MAX_LOG_LENGTH + 1];     	//��־��Ϣ�ַ���
	// �ȼ����ƣ�����ѡ����־����LEVEL_ERROR����ôWARN��Ϣ���������־
	if (level > m_iLevel)
	{
		return;
	}
	// ����ʱ��
	strcpy(szMsg, "[");
	struct tm *newtime;
	time_t long_time;
	time(&long_time);
	newtime = localtime(&long_time);
	sprintf(chTmp, "%04d%02d%02d %02d:%02d:%02d", newtime->tm_year + 1900,
			newtime->tm_mon + 1, newtime->tm_mday, newtime->tm_hour,
			newtime->tm_min, newtime->tm_sec);

	strcat(szMsg, chTmp);
	strcat(szMsg, "]");
	// �ȼ�
	strcat(szMsg, "[");
	strcat(szMsg, LOG_LEVELNAME[level]);
	strcat(szMsg, "]> ");

	// ����
	va_list argp;
	va_start(argp, format);
#ifdef WIN32
	_vsnprintf(&szMsg[strlen(szMsg)], MAX_LOG_LENGTH - strlen(szMsg), format, argp);
#else
	vsnprintf(&szMsg[strlen(szMsg)], MAX_LOG_LENGTH - strlen(szMsg), format,
			argp);
#endif
	va_end(argp);
	// �����־
	this->PrintLog(szMsg);
}

// ������־
void CMyLog::ClearLogfile(int day)
{
	char cmd[256] = {0};
	sprintf(cmd, "find ./*.log -mtime +%d -type f -exec rm -rf {} \\;", day);
	system(cmd);
}

void CMyLog::PrintLog(const char* szMsg)
{
	//��СʱΪ��λ������־�ļ�
	char logFileName[256] = { 0 };
	struct tm *newtime;
	time_t long_time;
	time(&long_time);
	newtime = localtime(&long_time);
	sprintf(logFileName, "%04d%02d%02d.log", newtime->tm_year + 1900,
			newtime->tm_mon + 1, newtime->tm_mday);
	// ���ļ����
	//pthread_mutex_lock(&mutexLock);
	critical_section.Lock();
	if (NULL == m_file)
	{
		m_file = fopen(logFileName, "a+");
	}
	else
	{
		fclose(m_file);
		m_file = NULL;
		m_file = fopen(logFileName, "a+");
	}
	if (NULL != m_file)
	{
		fprintf(m_file, "%s\n", szMsg);
		//������������������д�����
		fflush(m_file);
		//ͬʱ�������Ļ
		printf("%s\n", szMsg);
		fclose(m_file);
		m_file = NULL;
	}
	critical_section.Unlock();
	//pthread_mutex_unlock(&mutexLock);
}
