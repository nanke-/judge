/*************************************************************************
	> File Name: compiler.h
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月08日 星期二 19时34分23秒
	> Description: 
 ************************************************************************/

#ifndef _COMPILER_H__
#define _COMPILER_H__

#include <string>
#include <map>
#include <vector>




using std::string;
using std::vector;
using std::map;

class Compiler
{
public:
	Compiler(){}
	Compiler(const string lang, const string src, const string bin):m_lang(lang),
				m_src(src), m_bin(bin), m_result("") {}

	bool operator<(const Compiler& other)
	{
		return m_cpid < other.m_cpid;
	}
	void compile(void);
	
	void set_option(const char *key, const char *value);
	int read_config(void);
	void getConfigFile();
	void init(void);
	void result(int signal, int option = 0);
	void addTimeUsed(void);
	void setTimeUsed(int);
	int getTimeUsed(void) const;
	int getTimeLimit(void) const;
	int getCPid(void) const;
	const string& getResult(void) const;

private:
	string m_lang;
	string m_src;
	string m_bin;
	string m_compile_msg;
	string m_result;

	map<string, string> m_languages;

	int m_time_limit, m_output_limit;
	int m_cpid, m_time_used;
	char m_config_path[1024];
	char m_compile_command[1024];
	enum  
	{
		RS_NML,
		RS_CE,
	};
	enum
	{
		CE_LANG,
		CE_SIGNAL,
		CE_NOSRC,
		CE_NOBIN,
	};
};

int filter(const struct dirent *dir);
vector<int> getChildPid(int pid);
void killProcessTree(int pid);
void string_replace(string &strBig, const string &strsrc, const string &strdst);

void com_timer(int signo);
void com_setTimer();
int stringToInt(const char *str);
extern Compiler com;

#endif
