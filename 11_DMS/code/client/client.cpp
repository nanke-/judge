/*
 * Author: Kinga
 * Date: 2013-09-12
 * 实现客户机类
 */
#include "client.h"

// 客户机类构造器
Client::Client(LogReader& reader,
	LogSender& sender):
	m_reader(reader), m_sender(sender)
{
}

// 数据采集
void Client::dataMine(void)
	throw (ClientException)
{
	cout << "数据采集开始..." << endl;
	// more codes
	m_sender.sendLog(m_reader.readLog());
	cout << "数据采集完成。" << endl;
}
