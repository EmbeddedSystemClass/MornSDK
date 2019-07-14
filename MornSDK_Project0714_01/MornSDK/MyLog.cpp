/*
 * MyLog.cpp
 *
 *  Created on: 2018年12月16日
 *      Author: YWJ
 */
#include "General.h"

//构造函数
CMyLog::CMyLog()
{
	//初始化变量
	m_iLevel = LOG_NONE;
	m_file = NULL;
}

//析构函数
CMyLog::~CMyLog()
{
	if (NULL != m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
	//销毁互斥锁
	//pthread_mutex_destroy(&mutexLock);
}

void CMyLog::Init()
{
	m_iLevel = (LOG_LEVEL)_ttoi(g_Config.GetKeyValue("LogLevel","System"));
	//初始化互斥锁
	//pthread_mutex_init(&mutexLock,NULL);
}

void CMyLog::WriteLog(LOG_LEVEL level, const char* format, ...)
{
	// 输入检查
	if (NULL == format)
	{
		return;
	}
	char chTmp[20] = { 0 };            		//时间字符串
	char szMsg[MAX_LOG_LENGTH + 1];     	//日志信息字符串
	// 等级控制，例如选择日志级别LEVEL_ERROR，那么WARN信息不会输出日志
	if (level > m_iLevel)
	{
		return;
	}
	// 日期时间
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
	// 等级
	strcat(szMsg, "[");
	strcat(szMsg, LOG_LEVELNAME[level]);
	strcat(szMsg, "]> ");

	// 内容
	va_list argp;
	va_start(argp, format);
#ifdef WIN32
	_vsnprintf(&szMsg[strlen(szMsg)], MAX_LOG_LENGTH - strlen(szMsg), format, argp);
#else
	vsnprintf(&szMsg[strlen(szMsg)], MAX_LOG_LENGTH - strlen(szMsg), format,
			argp);
#endif
	va_end(argp);
	// 输出日志
	this->PrintLog(szMsg);
}

// 清理日志
void CMyLog::ClearLogfile(int day)
{
	char cmd[256] = {0};
	sprintf(cmd, "find ./*.log -mtime +%d -type f -exec rm -rf {} \\;", day);
	system(cmd);
}

void CMyLog::PrintLog(const char* szMsg)
{
	//以小时为单位生成日志文件
	char logFileName[256] = { 0 };
	struct tm *newtime;
	time_t long_time;
	time(&long_time);
	newtime = localtime(&long_time);
	sprintf(logFileName, "%04d%02d%02d.log", newtime->tm_year + 1900,
			newtime->tm_mon + 1, newtime->tm_mday);
	// 打开文件句柄
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
		//将缓存区的内容立即写入磁盘
		fflush(m_file);
		//同时输出到屏幕
		printf("%s\n", szMsg);
		fclose(m_file);
		m_file = NULL;
	}
	critical_section.Unlock();
	//pthread_mutex_unlock(&mutexLock);
}
