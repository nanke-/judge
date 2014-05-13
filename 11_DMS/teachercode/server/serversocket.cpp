// 实现服务器套接字类
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;
#include "serversocket.h"
#include "clientthread.h"
// 构造器
ServerSocket::ServerSocket (const string& ip,
	short port) throw (SocketException) {
	cout << "初始化服务器开始..." << endl;
	if((m_sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		throw SocketException("创建套接字失败");
	}
	int on = 1;
	if(setsockopt(m_sockfd, SOL_SOCKET, 
			SO_REUSEADDR, &on, sizeof(on)) == -1)
	{
		throw SocketException("设置重用端口失败");
	}
	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ip.empty() ? 
		INADDR_ANY : inet_addr(ip.c_str());
	if(bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		throw SocketException("绑定套接字失败");
	}
	if(listen(m_sockfd, 1024) == -1)
	{
		throw SocketException("监听套接字失败");
	}
	cout << "初始化服务器完成。" << endl;
}
// 等待客户机连接
void ServerSocket::acceptClient (void)
	throw (SocketException) {
	cout << "等待客户机连接..." << endl;
	// 循环阻塞
	for (;;) {
		struct sockaddr_in addrcli;
		socklen_t addrlen = sizeof(addrcli);
		// 每接受一个客户机的连接请求
		int connfd = accept(m_sockfd, 
				(struct sockaddr*)&addrcli, &addrlen);
		if(connfd == -1)
		{
			close(m_sockfd);
			throw SocketException("接受连接请求失败");
		}
		// 创建一个客户线程为其提供服务
		(new ClientThread (connfd))->start ();
		//pause ();
	}
}
