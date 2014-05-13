/*
 */
#include "clientdlg.h"

ClientDlg::ClientDlg(void)
{
	resize(560, 439);
	m_browser = new QTextBrowser(this);
	m_browser->setGeometry(10, 10, 540, 383);
	m_start = new QPushButton("start", this);
	m_start->move(374, 404);
	connect(m_start, SIGNAL(clicked()), 
			this, SLOT(onStart()));
	m_close = new QPushButton("close", this);
	m_close->move(474, 404);
	connect(m_close, SIGNAL(clicked()), 
			this, SLOT(close()));
	connect(&m_work, SIGNAL(update(QString)),
			this, SLOT(onUpdate(QString)));
}

ClientDlg::~ClientDlg(void)
{
	delete m_close;
	delete m_start;
	delete m_browser;
}

void ClientDlg::onStart(void)
{
	m_work.start();
}

void ClientDlg::onUpdate(QString text)
{
	m_browser->append(text);
}
