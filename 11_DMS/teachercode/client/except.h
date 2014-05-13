// 异常类
#ifndef _EXCEPT_H
#define _EXCEPT_H
#include <string>
using namespace std;
// 客户机异常
class ClientException : public exception {
public:
	ClientException (void) :
		m_msg ("客户机异常！") {}
	ClientException (const string& msg) :
		m_msg ("客户机异常：") {
		m_msg += msg;
		m_msg += "！";
	}
	~ClientException (void) throw () {}
	const char* what (void) const throw () {
		return m_msg.c_str ();
	}
private:
	string m_msg;
};
// 备份异常
class BackupException :
	public ClientException {
public:
	BackupException (void) :
		ClientException ("备份错误") {}
	BackupException (const string& msg) :
		ClientException (msg) {}
};
// 读取异常
class ReadException :
	public ClientException {
public:
	ReadException (void) :
		ClientException ("读取错误") {}
	ReadException (const string& msg) :
		ClientException (msg) {}
};
// 存储异常
class SaveException :
	public ClientException {
public:
	SaveException (void) :
		ClientException ("存储错误") {}
	SaveException (const string& msg) :
		ClientException (msg) {}
};
// 网络异常
class SocketException :
	public ClientException {
public:
	SocketException (void) :
		ClientException ("网络错误") {}
	SocketException (const string& msg) :
		ClientException (msg) {}
};
// 发送异常
class SendException :
	public ClientException {
public:
	SendException (void) :
		ClientException ("发送错误") {}
	SendException (const string& msg) :
		ClientException (msg) {}
};
#endif // _EXCEPT_H
