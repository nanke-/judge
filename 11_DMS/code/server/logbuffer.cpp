/*
 * Author: Kinga
 * Date: 2013-09-13
 * 实现日志缓冲类
 */

#include "logbuffer.h"

LogBuffer g_logBuffer;// 日志缓冲区，全局共享

//日志缓冲类构造器
LogBuffer::LogBuffer(void)
{
	// 初始化同步互斥量
	pthread_mutex_init(&m_mutex, 0);
}

// 压入日志记录
LogBuffer& LogBuffer::operator<<(const MLogRec& log)
{
	cout << "压入日志记录开始..." << endl;
	// more code
	pthread_mutex_lock(&m_mutex);
	m_logs.push_back(log);
	pthread_mutex_unlock(&m_mutex);
	cout << "压入日志记录完成" << endl;
	return *this;
}

// 弹出日志记录
LogBuffer& LogBuffer::operator>>(MLogRec& log)
{
	cout << "弹出日志记录开始..." << endl;
	// more code
	pthread_mutex_lock(&m_mutex);
	while(m_logs.empty())
	{
		cout << "等待日志记录..." << endl;
		// 等待日志记录产生
	}
	log = m_logs.front();
	m_logs.pop_front();
	pthread_mutex_unlock(&m_mutex);
	cout << "弹出日志记录完成" << endl;
	return *this;
}
