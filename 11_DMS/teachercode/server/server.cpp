// 实现服务器类
#include "server.h"
// 构造器
Server::Server (LogDao& dao, const string& ip,
	short port) throw (ServerException) :
	m_store (dao), m_socket (ip, port) {}
// 数据采集
void Server::dataMine (void)
	throw (ServerException) {
	try {
		// 启动存储线程...
		m_store.start ();
		// 等待客户机连接...
		m_socket.acceptClient ();
	}
	catch (ServerException& ex) {
		throw;
	}
}
