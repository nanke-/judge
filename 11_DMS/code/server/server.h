/*
 * Author: Kinga
 * Date: 2013-09-13
 *
 */
#ifndef _SERVER_H
#define _SERVER_H

#include "storethread.h"
#include "serversocket.h"
#include <string>
using namespace std;

class Server
{
	public:
		// 构造器
		Server(LogDao& dao, const string& ip,
				short port) throw (ServerException);
		// 数据采集
		void dataMine(void) throw (ServerException);
	private:
		StoreThread		m_store;	// 存储线程
		ServerSocket	m_socket;	// 服务器套接字
};

#endif // _SERVER_H
