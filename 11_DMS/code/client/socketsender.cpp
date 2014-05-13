/*
 * Author: Kinga
 * Date: 2013-09-12
 * 实现网络日志发送器
 */

#include "socketsender.h"
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 网络日志发送器构造器
SocketSender::SocketSender(const string& ip,
	short port, const string& failFile):
	m_ip(ip), m_port(port), m_failFile(failFile)
{
}
// 发送日志
void SocketSender::sendLog(list<MLogRec>& logs)
	throw (SendException)
{
	cout << "发送日志开始..." << endl;
	// mode codes
	try
	{
		try
		{
			readFailFile(logs);
		}
		catch(ReadException& ex)
		{
			cout << "无发送失败文件！" << endl;
		}
		//连接服务器
		connectServer();
		// 发送数据
		sendData(logs);
	}
	catch(SocketException& ex)
	{
		// 保存发送失败文件
		saveFailFile(logs);
		throw SendException("连接错误");
	}
	catch(SendException& ex)
	{
		// 保存发送失败文件
		saveFailFile(logs);
		throw SendException("发送错误");
	}
	cout << "发送日志完成。" << endl;
}
// 连接服务器
void SocketSender::connectServer(void)
	throw (SocketException)
{
	cout << "连接服务器开始..." << endl;
	// more codes
	if((m_sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		throw SocketException();
	}
	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(m_port);
	addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
	if(connect(m_sockfd, (sockaddr*)&addr, sizeof(addr)) == -1)
	{
		throw SocketException();
	}
	cout << "连接服务器完成。" << endl;
}
// 读取发送失败文件
void SocketSender::readFailFile(list<MLogRec>& logs)
	throw (ReadException)
{
	cout << "读取发送失败文件开始..." << endl;
	// more codes
	ifstream ifs(m_failFile.c_str(), ios::binary);
	if(! ifs)
	{
		throw ReadException();
	}
	MLogRec log;
	while(ifs.read((char*)&log, sizeof(log)))
	{
		logs.push_front(log);
	}
	if(! ifs.eof())
	{
		throw ReadException();
	}
	ifs.close();
	unlink(m_failFile.c_str());
	cout << "读取发送失败文件完成。" << endl;
}
// 发送数据
void SocketSender::sendData(list<MLogRec>& logs)
	throw (SendException)
{
	cout << "发送数据开始..." << endl;
	// more codes
	while(! logs.empty())
	{
		logs.front();
		MLogRec log = logs.front();
		log.pid = htonl(log.pid);
		log.logintime = htonl(log.logintime);
		log.logouttime = htonl(log.logouttime);
		log.durations = htonl(log.durations);
		if(send(m_sockfd, &log, sizeof(log), 0) == -1)
		{
			close(m_sockfd);
			throw SendException();
		}
		// 删除发送成功的匹配日志记录，
		// 剩余的记录存入发送失败文件
		logs.pop_front();
	}
	close(m_sockfd);
	cout << "发送数据完成。" << endl;
}
// 保存发送失败文件
void SocketSender::saveFailFile(list<MLogRec>& logs)
	throw (SaveException)
{
	cout << "保存发送失败文件开始..." << endl;
	// more codes
	if(! logs.empty())
	{
		ofstream ofs(m_failFile.c_str(), ios::binary);
		if(! ofs)
		{
			throw SaveException();
		}
		while(! logs.empty())
		{
			ofs.write((const char*)&logs.front(), 
					sizeof(MLogRec));
			logs.pop_front();
		}
	}
	cout << "保存发送失败文件完成。" << endl;
}
