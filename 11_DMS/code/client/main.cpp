/*
 * Author: Kinga
 * Date: 2013-09-12
 *
 */
#include <iostream>
using namespace std;
#include "logreader.h"
#include "consolesender.h"
#include "socketsender.h"
#include "client.h"


int main(void)
{
	try
	{
		LogReader reader("./wtmpx",
				"./login.dat");
	//	ConsoleSender sender;
		
		SocketSender sender("127.0.0.1",
			8888, "./fail.dat");
		 
		Client client(reader, sender);
		client.dataMine();
	}
	catch(ClientException& ex)
	{
		cout << ex.what() << endl;
	}
	return 0;
}
