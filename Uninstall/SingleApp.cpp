//ʵ��Ӧ�ó���ʵ������

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
	//ȡӦ�ó�������Ϊlocalserver������
    QString qstrProduct(QObject::tr("product")); 
	m_serverName = qstrProduct + QString(" ") + QFileInfo(QCoreApplication::applicationFilePath()).fileName();

	_initLocalConnection();
}

//����Ƿ��ѽ���һ��ʵ��������
bool SingleApp::isRunning()
{
	return m_bISRunning;
}

//ͨ��socketͨѶʵ�ֳ���ʵ�����У��������µ�����ʱ�����ú���
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

//ͨ��socketͨѶʵ�ֳ���ʵ������
//��ʼ���������ӣ�������Ӳ���server���򴴽��������˳�
void SingleApp::_initLocalConnection()
{
	m_bISRunning = false;

	QLocalSocket socket;
	socket.connectToServer(m_serverName);
	if (socket.waitForConnected(TIME_OUT))
	{
		//Ӧ�ó����Ѿ�����
		qDebug() << QString("%1 already running/").arg(m_serverName);
		m_bISRunning = true;
		
		//��������

		return;
	}
	//���Ӳ��Ϸ��������ʹ���һ��
	 _newLocalServer();
}

//����LocalServer
void SingleApp::_newLocalServer()
{
	m_localServer = new QLocalServer(this);
	connect(m_localServer, SIGNAL(newConnection()), this, SLOT(_newLocalConnection()));
	if (!m_localServer->listen(m_serverName))
	{
		//��ʱ����ʧ�ܣ������ǳ������ʱ���������̷��������µģ����Ƴ�
		if (QAbstractSocket::AddressInUseError == m_localServer->serverError())
		{
			QLocalServer::removeServer(m_serverName);
			m_localServer->listen(m_serverName);	//�ٴμ���
		}
	}
}

//�����
void SingleApp::_activateWindow()
{
	if (m_widget)
	{
		m_widget->showNormal();
		m_widget->raise();
		m_widget->activateWindow();	//�����
	}
}