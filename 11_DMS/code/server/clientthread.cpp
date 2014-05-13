/*
 * Author: Kinga
 * Date: 2013-09-13
 * 实现客户线程类
 */

#include "clientthread.h"
#include "data.h"
#include "logbuffer.h"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;

//客户线程类构造器
ClientThread::ClientThread(int connfd):
	m_connfd(connfd)
{	
}

//线程处理
void ClientThread::run(void)
{
	MLogRec log = {};
	for(size_t i = 0; i != 1000; i++)
	{
		sleep(1);
		g_logBuffer << log;
	}
	delete this;
}

