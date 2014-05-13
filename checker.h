/*************************************************************************
	> File Name: checker.h
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月24日 星期四 15时13分07秒
 ************************************************************************/

#ifndef _CHECKER_H__
#define _CHECKER_H__

#include <string>



using std::string;

class Checker
{
public:
	Checker(string ans_file, string user_out_file):m_ans_file(ans_file), m_user_out_file(user_out_file){}
	int checkDiff(void);
private:
	string  m_ans_file;
	string m_user_out_file;
	enum  
	{
		AC,		//Accepted
		PE,		//Presentation Error
		WA,		//Wrong Answer
	};
	bool equalStr(const char *s, const char *s2) const;
};
#endif

