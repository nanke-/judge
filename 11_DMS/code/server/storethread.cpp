/*
 * Author: Kinga
 * Date: 2013-09-13
 * 实现客户线程类
 */

#include "storethread.h"
#include "logbuffer.h"

//客户线程类构造器
StoreThread::StoreThread(LogDao& dao):
	m_dao(dao)
{	
}

//线程处理
void StoreThread::run(void)
{
	MLogRec log;
	for(;;)
	{
		// 每弹出一条日志记录
		g_logBuffer >> log;
		// 插入数据库
		m_dao.insert(log);
	}
}

