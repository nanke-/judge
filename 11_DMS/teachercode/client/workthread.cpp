#include "workthread.h"
#include "logreader.h"
#include "socketsender.h"
#include "client.h"

void WorkThread::run(void)
{
	try
	{
		LogReader reader("./wtmpx",
				"./logins.dat");
		SocketSender sender("127.0.0.1",
				8888, "./fail.dat", this);
		Client client(reader, sender);
		client.dataMine();
	}
	catch(ClientException& ex)
	{
		cout << ex.what() << endl;
	}
}

void WorkThread::update(const string& text)
{
	emit(update(QString(text.c_str())));
}
