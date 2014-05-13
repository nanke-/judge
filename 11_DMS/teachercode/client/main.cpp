#include <QApplication>
#include <iostream>
#include <cstring>
using namespace std;
#include "logreader.h"
#include "consolesender.h"
#include "socketsender.h"
#include "client.h"
#include "clientdlg.h"

int main (int argc, char* argv[]) {
	if(argc < 2)
	{
		try {
			LogReader reader ("./wtmpx",
				"./logins.dat");
	//		ConsoleSender sender;
			SocketSender sender ("127.0.0.1",
				8888, "./fail.dat");
			Client client (reader, sender);
			client.dataMine ();
		}
		catch (ClientException& ex) {
			cout << ex.what () << endl;
			return -1;
		}
	}
	else if(! strcmp(argv[1], "-g"))
	{
		QApplication app(argc, argv);
		ClientDlg dlg;
		dlg.show();
		return app.exec();
	}
	else
	{
		cerr << "用法: " << argv[0]
			<< " [-g 图形界面]" << endl;
		return -1;
	}
	return 0;
}
