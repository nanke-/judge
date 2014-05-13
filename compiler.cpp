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
#include <errno.h>
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
#include "logger.h"
#include "except.h"


using std::cerr;
using std::cout;
using std::endl;


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
		if(NULL == fp)
		{
			FM_LOG_WARNING("open file fail: %s\n", pid_path);
			throw CompileException(string("open file fail: ") + string(pid_path));
		}
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
		//cerr << "Compiled successfully";
		m_result = "Compiled successfully";
	}
	else if(signal == RS_CE)
	{
		m_result = "Compile Error";
	}
	else
	{
		if(option == CE_LANG)
		{

			//cerr << "No such language";
			m_result = "No such language";
		}
		else if(option == CE_SIGNAL)
		{
			//cerr << "Compiler error";
			m_result = "Compiler error";
		}
		else if(option == CE_NOSRC)
		{
			//cerr << "Source file not exist";
			m_result = "Source file not exist";
		}
		else if(option == CE_NOBIN)
		{
			//cerr << "Binary file not exist";
			m_result = "Binary file not exist";
		}

	}

	FM_LOG_DEBUG("result: option = %d signal = %d\n", option, signal);

	//exit(0);
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

const string& Compiler::getResult(void) const
{
	return m_result;
}

void com_timer(int signo)
{
	com.addTimeUsed();

	//cout << com.getTimeUsed() << ' ' << com.getTimeLimit() << endl;

	if(com.getTimeUsed() > com.getTimeLimit())
	{
		killProcessTree(com.getCPid());
	}
	alarm(1);
}

void com_setTimer()
{
	if(signal(SIGALRM, com_timer) == SIG_ERR)
	{
		FM_LOG_WARNING("cannot set Timer\n");
		throw CompileException(string("cannot set Timer"));
		//exit(0);
	}
	FM_LOG_TRACE("compile setTimer!\n");
	alarm(1);
	com.setTimeUsed(0);
}

void Compiler::compile(void)
{
	com_setTimer();
	if((m_cpid = fork()) == 0)
	{
		log_add_info("start compile");
		
		struct rlimit rl_time_limit, rl_output_limit;

		rl_time_limit.rlim_cur = rl_time_limit.rlim_max = m_time_limit;

		if(setrlimit(RLIMIT_CPU, &rl_time_limit) < 0)
		{
			FM_LOG_WARNING("set CPU limit fail , %d %s\n",  errno, strerror(errno));
			throw CompileException(string("set CPU limit fail") + string(strerror(errno)));
		}

		rl_output_limit.rlim_max = rl_output_limit.rlim_cur = m_output_limit * 1024;
		
		if(setrlimit(RLIMIT_FSIZE, &rl_output_limit) < 0)
		{
			FM_LOG_WARNING("set output_limit fail \n");
			throw CompileException(string("set output_limit fail"));
		}

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

		if(WIFEXITED(cstat))
		{
			if(EXIT_SUCCESS == WEXITSTATUS(cstat))
			{
				FM_LOG_TRACE("Compile Succeed");
				result(RS_NML, 0);
				return;
			}
			else {
				FM_LOG_TRACE("Compile Error");
				result(RS_CE, 0);
				return;
			}
		}
		if((WIFSIGNALED(cstat) && WTERMSIG(cstat) != 0)
			|| (WIFSTOPPED(cstat) && WSTOPSIG(cstat) != 0))
		{
			FM_LOG_TRACE("Compile Error: Stopped by signal");
			result(RS_CE, CE_SIGNAL);
			return;
		}

		if(access(m_bin.c_str(), 0) == -1)
		{
			FM_LOG_TRACE("Compile Error: No binary file");
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

	FM_LOG_DEBUG("m_config_path = %s\n", m_config_path);
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
	cout << "init" << endl;
	getConfigFile();
	cout << "second call " << endl;
	read_config();

	if(m_languages.find(m_lang) != m_languages.end())
	{
		string command = m_languages[m_lang];
		string_replace(command, "{$source}", m_src);
		string_replace(command, "{$binary}", m_bin);
		string_replace(command, "{$message}", m_compile_msg);

		FM_LOG_TRACE("Compile Command: %s\n", command.c_str());

		strcpy(m_compile_command, command.c_str());
	}
	else result(RS_CE, CE_LANG);

	if(access(m_src.c_str(), 0) == -1)
		result(RS_CE, CE_NOSRC);
	if(access(m_bin.c_str(), 0) == 0)
		remove(m_bin.c_str());
}