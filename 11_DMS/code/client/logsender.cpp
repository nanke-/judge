/*
 * Author: Kinga
 * Date: 2013-09-12
 * 实现网络日志发送器
 */
#include "socketsender.h"
using namespace std;

// 网络日志发送器
void sendLog(list<MLogRec> logs,
	const string& ip, const string& port,
	const string& failFile, int sockid);
// 连接服务器
void connectServer();
// 读取发送失败文件
void readFailFile();
// 发送数据
void sendData();
// 保存发送失败文件
void saveFailFile();
