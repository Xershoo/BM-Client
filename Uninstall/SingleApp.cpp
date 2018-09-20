//实现应用程序单实例运行

#include "SingleApp.h"
#include <QtNetwork/QLocalSocket>
#include <QFileInfo>
#include <QDebug>


#define TIME_OUT	(500)	//500ms


SingleApp::SingleApp(int &argc, char **argv)
	:QApplication(argc, argv)
	,m_widget(NULL)
	,m_bISRunning(false)
	,m_localServer(NULL)
{
	//取应用程序名作为localserver的名字
    QString qstrProduct(QObject::tr("product")); 
	m_serverName = qstrProduct + QString(" ") + QFileInfo(QCoreApplication::applicationFilePath()).fileName();

	_initLocalConnection();
}

//检查是否已經有一个实例在运行
bool SingleApp::isRunning()
{
	return m_bISRunning;
}

//通过socket通讯实现程序单实例运行，监听到新的连接时触发该函数
void SingleApp::_newLocalConnection()
{
	QLocalSocket *socket = m_localServer->nextPendingConnection();
	if (socket)
	{
		socket->waitForReadyRead(2*TIME_OUT);
		delete socket;

		_activateWindow();
	}
}

//通过socket通讯实现程序单实例运行
//初始化本地连接，如果连接不上server，则创建，否则退出
void SingleApp::_initLocalConnection()
{
	m_bISRunning = false;

	QLocalSocket socket;
	socket.connectToServer(m_serverName);
	if (socket.waitForConnected(TIME_OUT))
	{
		//应用程序已经启动
		qDebug() << QString("%1 already running/").arg(m_serverName);
		m_bISRunning = true;
		
		//其他处理

		return;
	}
	//链接不上服务器，就创建一个
	 _newLocalServer();
}

//创建LocalServer
void SingleApp::_newLocalServer()
{
	m_localServer = new QLocalServer(this);
	connect(m_localServer, SIGNAL(newConnection()), this, SLOT(_newLocalConnection()));
	if (!m_localServer->listen(m_serverName))
	{
		//此时监听失败，可能是程序崩溃时，残留进程服务器导致的，须移除
		if (QAbstractSocket::AddressInUseError == m_localServer->serverError())
		{
			QLocalServer::removeServer(m_serverName);
			m_localServer->listen(m_serverName);	//再次监听
		}
	}
}

//激活窗口
void SingleApp::_activateWindow()
{
	if (m_widget)
	{
		m_widget->showNormal();
		m_widget->raise();
		m_widget->activateWindow();	//激活窗口
	}
}