#include "setup.h"
#include "configfile.h"
#include "registryfile.h"
#include <QFile>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QProcess>

#include "env.h"
#include <qdebug.h>

SetUp::SetUp():m_cfgPath(""),m_isOver(false)
{
    m_pro = new QProcess;
    connect(m_pro,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(uncompressFinished()));
    connect(m_pro,SIGNAL(readyReadStandardOutput()),this,SLOT(uncompressSuccess()));
}

SetUp::~SetUp()
{
    if(m_pro){
        m_pro->close();
        delete m_pro;
    }
}

void SetUp::setConfigPath(const QString &configPath)
{
    m_cfgPath = configPath;
	m_cfgPath.replace(QChar('/'),QChar('\\'));
}

void SetUp::createRegeditData()const
{
    QString regKey = QString("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%1\\").arg(QObject::tr("ProductName"));
    ConfigFile *configFile = new RegistryFile(regKey);
	if(configFile->init())
	{
        
		QString strIcon = m_cfgPath;
        QString strUninstall = m_cfgPath;

		strIcon += QString("\\%1\\bin\\%2,0").arg(QObject::tr("ProductName")).arg(QObject::tr("ProductFile"));
        strUninstall += QString("\\%1\\bin\\Uninstall.exe").arg(QObject::tr("ProductName"));

		configFile->setValue("DisplayIcon",strIcon);
		configFile->setValue("DisplayName",QObject::tr("Product"));
		configFile->setValue("DisplayVersion",QObject::tr("VersionString")); 
		configFile->setValue("InstallLocation",m_cfgPath);
		configFile->setValue("Publisher",QObject::tr("Company"));
		configFile->setValue("UninstallString",strUninstall);
	}

	delete configFile;
}

void SetUp::createNetLogin()const
{
    QString regKey = QString("HKEY_CLASSES_ROOT\\%1\\").arg(QObject::tr("ProductName"));
    ConfigFile *settings = new RegistryFile(regKey);
    if (settings->init())
    {
        settings->setValue(".",QObject::tr("ProductName"));

        QString strPath = m_cfgPath;
        strPath += QString("\\%1\\update\\update.exe").arg(QObject::tr("ProductName"));
        strPath.replace("/","\\");
        settings->setValue("URL Protocol",strPath);
       
        QString strIcon = m_cfgPath;
        strIcon += QString("\\%1\\update\\update.exe,1").arg(QObject::tr("ProductName"));
        strIcon.replace("/","\\");
        settings->setValue("DefaultIcon/.",strIcon);

        settings->setValue("shell/.","");
        settings->setValue("shell/open/.","");

        QString shell = QString("\"%1\",").arg(strPath) + QString("\"\%1\"");
        shell.replace("/","\\");
        settings->setValue("shell/open/command/.", shell);
    }
    delete settings;
}

void SetUp::uncompressFinished(){
    m_pro->close();
    delete m_pro;
    m_pro = NULL;
	createRegeditData();
    createNetLogin();
    createShortCut();
    emit sendProcess(100);
}

void SetUp::createShortCut()
{
    createShortCutToDeskTop();
    createShortCutToStartMenue();
}

void SetUp::uncompressSuccess(){
    QString outPutStream = m_pro->readAllStandardOutput();
    if(!outPutStream.contains('%') || true == m_isOver)
        return;
    QStringList list = outPutStream.split('%');
    QString temp = list[0];
    QString count = "";
    count += temp[temp.size()-2];
    count += temp[temp.size()-1];
    emit sendProcess(count.toInt());
}

void SetUp::run(){
    ENV::createFolder(m_cfgPath);
    QString runPath = QCoreApplication::applicationDirPath();
    QString cmd = QString("\"%1/unrar\" x -y \"%2/%3.rar\" \"%4\"").arg(runPath).arg(runPath).arg(QObject::tr("ProductName")).arg(m_cfgPath);
    m_pro->start(cmd);
}

void SetUp::startExe(){
    QString filePath = QString("%1\\%2\\bin\\%3").arg(m_cfgPath).arg(QObject::tr("ProductName")).arg(QObject::tr("ProductFile"));
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<< "Open failed." << endl;
        return;
    }
    QTextStream txtInput(&file);
    QProcess::startDetached(filePath,QStringList());
    file.close();
}

void SetUp::createShortCutToDeskTop()
{
    QString filePath = QString("%1\\%2\\bin\\%3").arg(m_cfgPath).arg(QObject::tr("ProductName")).arg(QObject::tr("ProductFile"));
	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		qDebug()<< "Open failed." << endl;
		return;
	}
    QString nameProduct = QString("/") + QString(QObject::tr("Product"));
	ENV::createShortCutToDeskTop(filePath, nameProduct);
	file.close();
}

void SetUp::createShortCutToStartMenue()
{
    QString filePath = QString("%1\\%2\\bin\\%3").arg(m_cfgPath).arg(QObject::tr("ProductName")).arg(QObject::tr("ProductFile"));
    QString unisFilePath = QString("%1\\%2\\bin\\Uninstall.exe").arg(m_cfgPath).arg(QObject::tr("ProductName"));
	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		qDebug()<< "Open failed." << endl;
		return;
	}
	
	ENV::createShortCutToStartMenue(filePath, QString(QObject::tr("ProductName")), unisFilePath);
	file.close();
}

void SetUp::selfStarting()
{

}