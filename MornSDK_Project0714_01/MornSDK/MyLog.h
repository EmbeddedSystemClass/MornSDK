/*
 * MyLog.h
 *
 *  Created on: 2018年12月16日
 *      Author: YWJ
 */

#ifndef MYLOG_H_
#define MYLOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>



const int MAX_LOG_LENGTH = 1024;

/* 日志报警等级 */
typedef enum
{
	LOG_NONE= 0,    	//关闭日志
	LOG_FTL = 1,      	//致命
	LOG_ERR = 2,    	//错误,导致一个无法修复的错误
	LOG_WRN = 3,    	//警告,这时进行一些修复性的工作,系统能够恢复,可以继续运行下去
	LOG_INF = 4,    	//信息,反馈系统的当前状态给最终用户的
	LOG_DBG = 5	    	//调试,在实际运行过程中不输出
} LOG_LEVEL;


//不同等级对应的字符串
const char LOG_LEVELNAME[][8] = {"NONE", "FTL", "ERR", "WRN", "INF", "DBG"};

class CMyLog
{
public:
    CMyLog();
    ~CMyLog();
    //初始化
    void Init();
    // 日志打印接口，参数可变
    void WriteLog(LOG_LEVEL level, const char* format,...);
    // 清理日志
    void ClearLogfile(int day);
protected:
    void PrintLog(const char* szMsg); // 打印输出信息
    void GetDateString(char* szDate); // 获得日期, 实现省略
    void GetTimeString(char* szTime); // 获得时间，实现省略
private:
    FILE *m_file; // 日志文件
    LOG_LEVEL m_iLevel;
    //临界区
	CCriticalSection critical_section;
    //pthread_mutex_t mutexLock;
};

#endif /* MYLOG_H_ */
