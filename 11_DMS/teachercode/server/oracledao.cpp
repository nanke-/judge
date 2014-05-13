// 实现Oracle数据访问对象类
#include <iostream>
using namespace std;
#include "oracledao.h"
// 构造器
OracleDao::OracleDao (const string& username,
	const string& password)
	throw (DBException) {
	cout << "打开数据库开始..." << endl;

	cout << "打开数据库完成。" << endl;
}
OracleDao::~OracleDao (void) {
	cout << "关闭数据库开始..." << endl;

	cout << "关闭数据库完成。" << endl;
}
// 插入
void OracleDao::insert (const MLogRec& log)
	throw (DBException) {
	cout << "插入数据库开始..." << endl;
	cout << log << endl;
	cout << "插入数据库完成。" << endl;
}
