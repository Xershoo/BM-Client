//ʵ��Ӧ�ó���ʵ������
#pragma once

#include <QObject>
#include <QApplication>
#include <QtNetwork/QLocalServer>
#include <QWidget>


class SingleApp : public QApplication
{
	Q_OBJECT
public:
	SingleApp(int &argc, char **argv);

	bool isRunning();	//�Ƿ��Ѿ���ʵ��������

private slots:
	//������ʱ����
	void _newLocalConnection();

private:
	//��ʼ����������
	void _initLocalConnection();
	
	//����������
	void _newLocalServer();

	//�����
	void _activateWindow();

public:
	QWidget		*m_widget;

private:
	bool			m_bISRunning;	//�Ƿ��Ѿ���ʵ������
	QLocalServer	*m_localServer;	//����socket server
	QString			m_serverName;	//��������

};

