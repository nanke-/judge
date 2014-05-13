/*************************************************************************
	> File Name: mysqldao.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月28日 星期一 19时58分11秒
 ************************************************************************/

#include "mysqldao.h"

MysqlDao::MysqlDao(const char *host, const char *username, const char *psw, const char *db, 
	const unsigned int port)throw(DBException):m_host(host), m_username(username), m_psw(psw), m_db(db), m_port(port)
{
	if(NULL == mysql_init(&m_mysql))
	{
		throw DBException("mysql init fail");
	}
	m_connected = false;
}


MysqlDao::~MysqlDao(void)
{
	if(m_connected)
	{
		mysql_close(&m_mysql);
	}
}

void MysqlDao::connect(void) throw (DBException)
{
	if(!m_connected)
	{
		if(NULL == mysql_real_connect(&m_mysql, m_host.c_str(), m_username.c_str(),
				m_psw.c_str(), m_db.c_str(), m_port, NULL, 0))
		{
			throw DBException("mysql_real_connect fail");
		}
		m_connected = true;	
	}
}
MYSQL_RES* MysqlDao::query(const string comand) throw(DBException)
{
	try
	{
		connect();
	}
	catch(DBException &ex)
	{
		throw;
	}

	if(mysql_query(&m_mysql, comand.c_str()))
	{
		throw DBException(mysql_error(&m_mysql));
	}
	MYSQL_RES *result = mysql_store_result(&m_mysql);

	if(!result && mysql_errno(&m_mysql))
	{
		throw DBException(mysql_error(&m_mysql));
	}
	return result;
}

void MysqlDao::update(const string comand) throw (DBException)
{
	try
	{
		connect();
	}
	catch(DBException &ex)
	{
		throw;
	}

	if(mysql_query(&m_mysql, comand.c_str()))
	{
		throw DBException(mysql_error(&m_mysql));
	}
}

