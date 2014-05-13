/*
 * Author: Kinga
 * Date: 2013-09-13
 * 实现文件数据访问对象类
 */

#include "filedao.h"

// 文件数据访问对象类构造器
FileDao::FileDao(const string& path)
	throw (DBException)
{
	cout << "打开数据库开始..." << endl;
	// more codes
	cout << "打开数据库完成。" << endl;
}

FileDao::~FileDao(void)
{
	cout << "关闭数据库开始..." << endl;
	// more codes
	cout << "关闭数据库完成。" << endl;
}
		
// 插入
void FileDao::insert(const MLogRec& log)
	throw (DBException)
{
	cout << "插入数据库开始..." << endl;
	// more codes
	cout << "插入数据库完成。" << endl;
}
