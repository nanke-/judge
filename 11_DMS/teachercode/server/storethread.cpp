// 实现存储线程类
#include <iostream>
using namespace std;
#include "storethread.h"
#include "logbuffer.h"
// 构造器
StoreThread::StoreThread (LogDao& dao) :
	m_dao (dao) {}
// 线程处理
void StoreThread::run (void) {
	MLogRec log;
	for (;;) {
		// 每弹出一条日志记录
		g_logBuffer >> log;
		// 插入数据库
		try
		{
			m_dao.insert (log);
		}
		catch(DBException& ex)
		{
			cout << ex.what() << endl;
		}
		//sleep (1);
	}
}
