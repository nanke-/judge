/*
 * Author: Kinga
 * Date: 2013-09-13
 * 声明日志缓冲类
 */
#ifndef _LOGBUFFER_H
#define _LOGBUFFER_H

#include "data.h"
#include <pthread.h>
#include <list>

//日志缓冲类
class LogBuffer
{
	public:
		// 构造器
		LogBuffer(void);
		// 压入日志记录
		LogBuffer& operator<<(const MLogRec& log);
		// 弹出日志记录
		LogBuffer& operator>>(MLogRec& log);
	private:
		list<MLogRec>	m_logs;		// 日志记录集
		pthread_mutex_t	m_mutex;	//同步互斥量
};

extern LogBuffer g_logBuffer;
#endif //_LOGBUFFER_H
