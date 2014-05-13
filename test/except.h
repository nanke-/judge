/*************************************************************************
	> File Name: except.h
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月25日 星期五 12时02分35秒
 ************************************************************************/

#ifndef _EXCEPT_H___
#define _EXCEPT_H___

#include <string>
#include <exception>


using namespace std;

class JudgeException : public exception
{
public:
	JudgeException(void): m_msg("Judge System Error!"){}
	JudgeException(const string& msg):
		m_msg("Judge System Error: ")
	{
		m_msg += msg;
		m_msg += "!";
	}
	~JudgeException(void) throw(){}
	const char* what(void) const throw()
	{
		return m_msg.c_str();
	}
private:
	string m_msg;
};


class CompileException: public JudgeException
{
public:
	CompileException(void):
		JudgeException("Compile Error!"){}
	CompileException(const string& msg):
		JudgeException(msg){}
};

class ExecutorException: public JudgeException
{
public:
	ExecutorException(void):
		JudgeException("Executor Error!"){}
	ExecutorException(const string &msg):
		JudgeException(msg){}

};

class CheckerException: public JudgeException 
{
public:
	CheckerException(void):
		JudgeException("Checker Error!"){}
	CheckerException(const string &msg):
		JudgeException(msg){}

};

class DBException: public JudgeException
{
public:
	DBException(void):
		JudgeException("DataBase Error!"){}
	DBException(const string &msg):
		JudgeException(msg){}
	
};
#endif //_EXCEPT_H__

