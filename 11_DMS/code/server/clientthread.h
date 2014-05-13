/*
 * Author: Kinga
 * Date: 2013-09-13
 * 声明客户线程类
 */
#ifndef _CLIENTTHREAD_H
#define _CLIENTTHREAD_H

#include "logthread.h"

//客户线程类
class ClientThread : public LogThread
{
	public:
		// 构造器
		ClientThread(int connfd);
		//线程处理
		void run(void);

	private:
		int		m_connfd;	//通信套接字
};

#endif // _CLIENTTHREAD_H
