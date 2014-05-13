/*************************************************************************
	> File Name: main.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月26日 星期六 17时01分11秒
 ************************************************************************/

#include <iostream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <mysql.h>

#include "mysqldao.h"

using namespace std;

int main(void)
{

	try
	{
		MysqlDao mysql("localhost", "root", "123456", "csp_ojtest", 3306);
		MYSQL_RES *result = mysql.query("select * from csp_ojtest.tbl_submitions");

		if(!result)
		{
			cout << "empty table" << endl;
			exit(1);
		}
		//int rowcount = mysql_num_rows(result);
		int fieldcount = mysql_num_fields(result);\

		MYSQL_ROW row;
		while((row = mysql_fetch_row(result)))
		{
			unsigned long *lengths;
			lengths = mysql_fetch_lengths(result);

			int fd = open(row[0], O_CREAT | O_WRONLY);
			if(-1 == fd)
			{
				throw JudgeException("open file fail");
			}
			write(fd, row[4], lengths[4]);
			close(fd);
		}

	}
	catch(JudgeException &ex)
	{
		cout << ex.what() << endl;
		exit(1);
	}


	return 0;
}
