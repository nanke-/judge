/*
 */
#include "filedao.h"
#include "oracledao.h"
#include "server.h"
#include <iostream>
using namespace std;

int main(void)
{
	try
	{
		FileDao dao("./dms.db");
		//OracleDao dao("openlab", "open123");
		Server server(dao, "", 8888);
		server.dataMine();
	}
	catch(ServerException& ex)
	{
		cout << ex.what() << endl;
		return -1;
	}
	return 0;
}
