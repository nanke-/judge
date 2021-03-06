/*************************************************************************
	> File Name: checker.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月24日 星期四 15时13分16秒
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <sys/mman.h>
#include <fcntl.h>


#include "checker.h"

bool Checker::equalStr(const char *s, const char *s2) const
{
	const char *c1 = s, *c2 = s2;
	while(*c1 && c2 && (*c1++ == *c2++))
	{
		NULL;
	}

	return 0 == (*c1 | *c2);
}

int Checker::checkDiff(void)
{
	int ans_fd = open(m_ans_file.c_str(), O_RDONLY);
	if(-1 == ans_fd)
	{
		perror("ans_file not exist");
		exit(1);
	}
	int user_out_fd = open(m_user_out_file.c_str(), O_RDONLY);
	if(-1 == user_out_fd)
	{
		perror("user_out_file not exist");
		exit(1);
	}

	char *user_out, *ans;
	off_t user_out_len= lseek(user_out_fd, 0, SEEK_END);
	off_t ans_len = lseek(ans_fd, 0, SEEK_END);

	if(-1 == user_out_len || -1 == ans_len)
	{
		perror("lseek fail");
		exit(1);
	}

	// test output limit
	// if(user_out_len >= MAX_OUTPUT)
	// {
	// 	return OLE;
	// }

	lseek(user_out_fd, 0, SEEK_SET);
	lseek(ans_fd, 0, SEEK_SET);

	if(0 == (user_out_len && ans_len))
	{
		if(user_out_len || ans_len)
		{
			return WA;
		}
		return AC;
	}

	if((user_out = (char *)mmap(NULL, user_out_len, PROT_READ | PROT_WRITE, MAP_PRIVATE, user_out_fd, 0)) == MAP_FAILED)
	{
		perror("mmap user_out fail");
		exit(1);
	}

	if((ans = (char *)mmap(NULL, ans_len, PROT_READ | PROT_WRITE, MAP_PRIVATE, ans_fd, 0)) == MAP_FAILED)
	{
		munmap(user_out, user_out_len);
		perror("mmap ans fail");
		exit(1);
	}

	if((user_out_len == ans_len) && equalStr(user_out, ans))
	{
		munmap(user_out, user_out_len);
		munmap(ans, ans_len);
		return AC;
	}

	const char *cuser = user_out, *cans = ans;
	const char *end_user = user_out + user_out_len;
	const char *end_ans = ans + ans_len;

	while((cuser < end_user) && (cans < end_ans))
	{
		while((cuser < end_user)
			&& (*cuser == ' ' || *cuser == '\n' || *cuser == '\r' || *cuser == '\t'))
			++cuser;
		while((cans < end_ans)
			&& (*cans == ' ' || *cans == '\n' || *cans == '\r' || *cans == '\t'))
			++cans;
		if(cuser == end_user || cans == end_ans)
			break;
		if(*cuser != *cans)
			break;
		++cuser;
		++cans;
	}

	while((cuser < end_user)
		&& (*cuser == ' ' || *cuser == '\n' || *cuser == '\r' 
			|| *cuser == '\t'))
	{
		++cuser;
	}
	while((cans < end_ans)
		&& (*cans == ' ' || *cans == '\n' || *cans == '\r' 
			|| *cans == '\t'))
	{
		++cans;
	}

	if(cuser == end_user && cans == end_ans)
	{
		munmap(user_out, user_out_len);
		munmap(ans, ans_len);
		return PE;
	}

	munmap(user_out, user_out_len);
	munmap(ans, ans_len);
	return WA;
	
}
