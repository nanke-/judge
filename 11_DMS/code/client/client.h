/*
 * Author: Kinga
 * Date: 2013-09-12
 * 声明客户机类
 */
#ifndef _CLIENT_H
#define _CLIENT_H

#include "logreader.h"
#include "socketsender.h"

// 客户机类
class Client
{
	public:
		// 构造器
		Client(LogReader& reader,
				LogSender& sender);
		// 数据采集
		void dataMine(void)
			throw (ClientException);
	private:
		LogReader&	m_reader;	//日志读取器
		LogSender&	m_sender;	//日志发送器
};

#endif // _CLIENT_H