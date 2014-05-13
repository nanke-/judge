/*
 * Author: Kinga
 * Date: 2013-09-13
 * 声明存储线程类
 */
#ifndef _STORETHREAD_H
#define _STORETHREAD_H

#include "logthread.h"
#include "logdao.h"

// 存储线程类
class StoreThread : public LogThread
{
	public:
		// 构造器
		StoreThread(LogDao& dao);
		//线程处理
		void run(void);

	private:
		LogDao&		m_dao;	//数据访问对象
};

#endif // _STORETHREAD_H
