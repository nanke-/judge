/*
 * 实现服务器套接字类
 *
 * Author: Kinga
 * Date: 2013-09-13
 */
#include "serversocket.h"
#include "clientthread.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

//服务器套接字类构造器
ServerSocket::ServerSocket(const string& ip, short port)
	throw (SocketException)
{
	cout << "初始化服务器开始..." << endl;
	// more codes
	cout << "初始化服务器完成。" << endl;
}

// 等待客户机连接
void ServerSocket::acceptClient(void) throw (SocketException)
{
	cout << "等待客户机连接..." << endl;
	// more codes
	for(;;)
	{
		sleep(1);// 模拟accept阻塞
		int connfd = -1;// 从accept返回
		// 创建一个客户线程为客户提供服务
		(new ClientThread(connfd))->start();
	}
}
