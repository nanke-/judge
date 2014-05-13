// 实现客户线程类
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
using namespace std;
#include "clientthread.h"
#include "logbuffer.h"
// 构造器
ClientThread::ClientThread (int connfd) :
	m_connfd (connfd) {}
// 线程处理
void ClientThread::run (void) {
	unsigned long long logs;
	MLogRec log;
	// 每收到一条日志记录
	for (logs = 0;; ++logs) {
		ssize_t rlen;
		for(size_t len = sizeof(log); len; len -= rlen)
		{
			if((rlen = recv(m_connfd, (char*)&log + sizeof(log) - len, 
					len, 0)) <= 0)
				goto out_of_loop;
		}
		log.pid = ntohl(log.pid);
		log.logintime = ntohl(log.logintime);
		log.logouttime = ntohl(log.logouttime);
		log.durations = ntohl(log.durations);
		g_logBuffer << log;
	}
out_of_loop:
	cout << "接收到" << logs << "条匹配日志记录" << endl;
	close(m_connfd);
	delete this;
}
