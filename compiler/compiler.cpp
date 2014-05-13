/*************************************************************************
	> File Name: compiler.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月09日 星期三 10时15分37秒
	> Description: 
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include <unistd.h>
#include <dirent.h>
#include <sys/user.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "compiler.h"

using std::cerr;
using std::cout;
using std::endl;


int stringToInt(const char *str)
{
	int num;
	sscanf(str, "%d", &num);
	return num;
}

int filter(const struct dirent *dir)
{
	int n = strlen(dir->d_name);
	for(int i = 0; i < n; ++i)
	{
		if(!isdigit(dir->d_name[i]))
			return 0;
	}
	return 1;
}
vector<int> getChildPid(int pid)
{
	struct dirent **namelist;
	char pid_path[128];
	vector<int> result;

	int total = scandir("/proc", &namelist, filter, alphasort);
	for(int i = 0; i < total; ++i)
	{
		int ppid, tmp;
		char tmp2[128];
		strcpy(pid_path, "/proc/");
		strcat(pid_path, namelist[i]->d_name);
		strcat(pid_path, "/stat");

		FILE *fp = fopen(pid_path, "r");
		fscanf(fp, "%d %s %s %d", &tmp, tmp2, tmp2, &ppid);
		fclose(fp);
		if(ppid == pid)
		{
			sscanf(namelist[i]->d_name, "%d", &tmp);
			result.push_back(tmp);
		}
	}
	return result;
}


void killProcessTree(int pid)
{
	vector<int> children = getChildPid(pid);
	for(vector<int>::iterator i = children.begin(); i != children.end(); ++i)
	{
		int cpid = *i;
		killProcessTree(cpid);
	}
	kill(pid, SIGKILL);
}

void Compiler::result(int signal, int option)
{
	if(signal == RS_NML)
	{
		cerr << "Compiled successfully";
	}
	else
	{
		if(option == CE_LANG)
			cerr << "No such language";
		else if(option == CE_SIGNAL)
			cerr << "Compiler error";
		else if(option == CE_NOSRC)
			cerr << "Source file not exist";
		else if(option == CE_NOBIN)
			cerr << "Binary file not exist";

	}

	cerr << endl;
	cout << signal << endl << option << endl;

	exit(0);
}

void Compiler::addTimeUsed(void)
{
	++m_time_used;
}

void Compiler::setTimeUsed(int time_used)
{
	m_time_used = time_used;
}

int Compiler::getTimeUsed(void) const
{
	return m_time_used;
}

int Compiler::getTimeLimit(void) const
{
	return m_time_limit;
}

int Compiler::getCPid(void) const
{
	return m_cpid;
}


void timer(int signo)
{
	com.addTimeUsed();

	//cout << com.getTimeUsed() << ' ' << com.getTimeLimit() << endl;

	if(com.getTimeUsed() > com.getTimeLimit())
	{
		killProcessTree(com.getCPid());
	}
	alarm(1);
}

void setTimer()
{
	if(signal(SIGALRM, timer) == SIG_ERR)
	{
		perror("cannot set timer");
		exit(0);
	}
	cout << "Set Timer.." << endl;
	alarm(1);
	com.setTimeUsed(0);
}

void Compiler::compile(void)
{
	setTimer();
	if((m_cpid = fork()) == 0)
	{
		struct rlimit rl_time_limit, rl_output_limit;

		rl_time_limit.rlim_cur = m_time_limit;

		setrlimit(RLIMIT_CPU, &rl_time_limit);

		rl_output_limit.rlim_max = rl_output_limit.rlim_cur = m_output_limit * 1024;
		setrlimit(RLIMIT_FSIZE, &rl_output_limit);

		freopen("compile_stdin.null", "r", stdin);
		freopen("compile_stdout.null", "w", stdout);
		freopen("compile_stderr.null", "w", stderr);

		execl("/bin/sh", "sh", "-c", m_compile_command, NULL);

		exit(0);
	}
	else 
	{
		int cstat;
		waitpid(m_cpid, &cstat, NULL);
		
		if((WIFSIGNALED(cstat) && WTERMSIG(cstat) != 0)
			|| (WIFSTOPPED(cstat) && WSTOPSIG(cstat) != 0))
		{
			result(RS_CE, CE_SIGNAL);
		}

		if(access(m_bin.c_str(), 0) == -1)
		{
			cout << m_bin.c_str() << endl;
			result(RS_CE, CE_NOBIN);
		}

		result(RS_NML);
	}

}

void Compiler::set_option(const char *key, const char *value)
{
	if(strcmp(key, "time_limit") == 0)
	{
		m_time_limit = stringToInt(value);
		m_time_limit = (m_time_limit / 1000) + (m_time_limit % 1000 ? 1 : 0);

	}
	else if(strcmp(key, "output_limit") == 0)
	{
		m_output_limit = stringToInt(value);
	}
	else if(strcmp(key, "messagefile") == 0)
	{
		m_compile_msg = value;
	}
	else 
	{
		m_languages[key] = value;
	}
}

int Compiler::read_config(void)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *value;

	doc = xmlParseFile(m_config_path);
	if(doc == NULL)
		return 1;

	cur = xmlDocGetRootElement(doc);

	if(xmlStrcmp(cur->name, (const xmlChar *)"document") != 0)
	{
		xmlFreeDoc(doc);
		return 1;
	}

	for(cur = cur->xmlChildrenNode; cur != NULL; cur = cur->next)
	{
		if(cur->type == 1)
		{
			value = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			set_option((const char*)cur->name, (const char *)value);
			xmlFree(value);

		}
	}
	xmlFreeDoc(doc);
	return 0;
}

void Compiler::getConfigFile()
{
	sprintf(m_config_path, "/proc/%d/exe", getpid());
	m_config_path[readlink(m_config_path, m_config_path, 1023)] = '\0';
	strrchr(m_config_path, '/')[1] = 0;
	sprintf(&m_config_path[strlen(m_config_path)], "compiler.xml");
}

void string_replace(string &strBig, const string &strsrc, const string &strdst)
{
	string::size_type pos = 0;
	string::size_type srclen = strsrc.size();
	string::size_type dstlen = strdst.size();
	while((pos = strBig.find(strsrc, pos)) != string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}

void Compiler::init(void)
{
	getConfigFile();
	read_config();

	if(m_languages.find(m_lang) != m_languages.end())
	{
		string command = m_languages[m_lang];
		string_replace(command, "{$source}", m_src);
		string_replace(command, "{$binary}", m_bin);
		string_replace(command, "{$message}", m_compile_msg);

		cerr << "Command: " << command << endl;
		strcpy(m_compile_command, command.c_str());
	}
	else result(RS_CE, CE_LANG);

	if(access(m_src.c_str(), 0) == -1)
		result(RS_CE, CE_NOSRC);
	if(access(m_bin.c_str(), 0) == 0)
		remove(m_bin.c_str());
}