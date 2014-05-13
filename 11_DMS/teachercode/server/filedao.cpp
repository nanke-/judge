// 实现文件数据访问对象类
#include <iostream>
using namespace std;
#include "filedao.h"
// 构造器
FileDao::FileDao (const string& path)
	throw (DBException) {
	cout << "打开数据库开始..." << endl;
	m_ofs.open(path.c_str(), ios::app);
	if(! m_ofs)
	{
		throw DBException("数据库文件无法打开");
	}
	cout << "打开数据库完成。" << endl;
}
FileDao::~FileDao (void) {
	cout << "关闭数据库开始..." << endl;
	m_ofs.close();
	cout << "关闭数据库完成。" << endl;
}
// 插入
void FileDao::insert (const MLogRec& log)
	throw (DBException) {
	cout << "插入数据库开始..." << endl;
	m_ofs << log << endl;
	cout << "插入数据库完成。" << endl;
}
