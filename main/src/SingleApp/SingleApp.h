//实现应用程序单实例运行
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

	bool isRunning();	//是否已经有实例在运行

private slots:
	//有链接时触发
	void _newLocalConnection();

private:
	//初始化本地链接
	void _initLocalConnection();
	
	//创建服务器
	void _newLocalServer();

	//激活窗口
	void _activateWindow();

public:
	QWidget		*m_widget;

private:
	bool			m_bISRunning;	//是否已经有实例运行
	QLocalServer	*m_localServer;	//本地socket server
	QString			m_serverName;	//服务名称

};

