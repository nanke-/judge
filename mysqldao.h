/*************************************************************************
	> File Name: mysqldao.h
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月28日 星期一 19时58分04秒
 ************************************************************************/

#ifndef _MYSQLDAO_H__
#define _MYSQLDAO_H__

#include <mysql/mysql.h>
#include <string>

#include "except.h"
using std::string;

class MysqlDao
{
public:
	MysqlDao(const char *host, const char *username, const char *psw, const char *db, const unsigned int port) throw (DBException);
	~MysqlDao(void);

	MYSQL_RES* query(const string comand) throw (DBException);
	void update(const string conmand) throw (DBException);

private:
	string m_host;
	string m_username;
	string m_psw;
	string m_db;
	unsigned int m_port;

	bool m_connected;

	MYSQL m_mysql;

	void connect(void) throw (DBException);

};

#endif





