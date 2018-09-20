#include "MainDlg.h"
#include <QGraphicsDropShadowEffect>
#include <QtGui/QPalette>
#include <QtGui/QPainter>
#include <QtCore/QtMath>
#include <QtGui/QMouseEvent>
#include <QtGui/QMovie>
#include <QtWidgets/QMessageBox>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QTextCodec>
#include "Util.h"

//////////////////////////////////////////////////////////////////////////
//
extern char g_szInstallPath[1024];

UninstallThread::UninstallThread(QObject * parent /* = NULL */):QThread(parent)
{
    
}

UninstallThread::~UninstallThread()
{

}

void UninstallThread::run()
{
    msleep(500);

    emit progress(UNINSTALL_DEL_REGEDIT);
    delRegeditData();

    msleep(500);

    emit progress(UNINSTALL_DEL_SHORTCUT);
    delShortcut();

    msleep(500);

    emit progress(UNINSTALL_DEL_FILES);
    delProgramFile();

    msleep(500);

    emit progress(UNINSTALL_FINISHED);    
}

void UninstallThread::delRegeditData()
{
    QString qstrProduct(QObject::tr("product"));

    QSettings * regSetting = new QSettings(QString("HKEY_CLASSES_ROOT\\"),QSettings::NativeFormat);
    if(NULL != regSetting)
    {
        regSetting->remove(qstrProduct);
        
        delete regSetting;
        regSetting = NULL;
    }

    QString qstrKey("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
    qstrKey += qstrProduct;
    qstrKey += QString("\\");

    regSetting = new QSettings(qstrKey,QSettings::NativeFormat);
    if(NULL != regSetting)
    {
        m_dirInstall = regSetting->value(QString("InstallLocation"),QVariant()).toString();
        m_dirInstall += QString("\\");
        m_dirInstall += qstrProduct;

        regSetting->clear();

        delete regSetting;
        regSetting = NULL;
    }

    regSetting = new QSettings(QString("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\")
        ,QSettings::NativeFormat);
    if(NULL != regSetting)
    {
        regSetting->remove(qstrProduct);
        
        delete regSetting;
        regSetting = NULL;
    }

    return;
}

void UninstallThread::delShortcut()
{
    QString qstrProduct(QObject::tr("product"));

    //desktop shortcut
    QString deskTopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString deskTopFile = deskTopPath + QString("/")+QString(tr("nameProduct"))+QString(".lnk");
    QFile::remove(deskTopFile);
    
    deskTopFile = deskTopPath + QString("/")+qstrProduct+QString(".lnk");
    QFile::remove(deskTopFile);

    //startmenu shortcut
    QString startMenuePath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    QString startMenuDir = startMenuePath + QString("/")+QString(tr("nameProduct"));
    QString startMenuDLMusicLink = startMenuDir + QString("/") + qstrProduct + QString(".lnk");
    QString startMenuUninstallLink = startMenuDir + QString("/Uninstall.lnk");

    QFile::remove(startMenuDLMusicLink);
    QFile::remove(startMenuUninstallLink);

    QDir dir;
    if (dir.exists(startMenuDir))
    {  
        dir.remove(startMenuDLMusicLink);
        dir.remove(startMenuUninstallLink);
        dir.rmdir(startMenuDir);
    }
    
    startMenuDir = startMenuePath + QString("/")+qstrProduct;
    startMenuDLMusicLink = startMenuDir + QString("/") + qstrProduct + QString(".lnk");
    startMenuUninstallLink = startMenuDir + QString("/Uninstall.lnk");

    QFile::remove(startMenuDLMusicLink);
    QFile::remove(startMenuUninstallLink);

    if (dir.exists(startMenuDir))
    {  
        dir.remove(startMenuDLMusicLink);
        dir.remove(startMenuUninstallLink);
        dir.rmdir(startMenuDir);
    }
        
    startMenuDLMusicLink = startMenuePath + QString("/") + qstrProduct + QString(".lnk");
    QFile::remove(startMenuDLMusicLink);
      
    startMenuDLMusicLink = startMenuePath + QString("/")+QString(tr("nameProduct"))+QString(".lnk");
    QFile::remove(deskTopFile);
}

void UninstallThread::delProgramFile()
{    
    if(m_dirInstall.isEmpty())
    {
        return;
    }

    //清除目录文件
    removePath(m_dirInstall);

    QString strPath = m_dirInstall;
    strPath.replace('/','\\');
    QByteArray sArray = QTextCodec::codecForLocale()->fromUnicode(strPath);
    strcpy_s(g_szInstallPath,sArray.data());
}

bool UninstallThread::removePath(QString& pathName)
{
    QDir directory(pathName);
    if (!directory.exists())
    {
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(pathName);
    if (!srcPath.endsWith(QDir::separator()))
    {
        srcPath += QDir::separator();
    }

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;

    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath))
            {
                qDebug() << "remove file" << filePath << " faild!";
                error = true;
            }
        }
        else if (fileInfo.isDir())
        {
            if (!removePath(filePath))
            {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path())))
    {
        qDebug() << "remove dir" << directory.path() << " faild!";
        error = true;
    }

    return !error;
}


//////////////////////////////////////////////////////////////////////////
//
extern bool g_uninstallFinished;

MainDlg::MainDlg(QWidget *parent)
    : QDialog(parent)
	, m_moveable(false)
	, m_thrdUninstall(NULL)
{
    ui.setupUi(this);
	
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowIcon(QIcon(QString(":/Image/res/image/uninstall.ico")));

	this->setWindowRoundCorner(ui.widget_bottom,4,4);

	QGraphicsDropShadowEffect *wndShadow = new QGraphicsDropShadowEffect;
	wndShadow->setBlurRadius(12.0);
	wndShadow->setColor(QColor(0, 0, 0, 160));
	wndShadow->setOffset(0.0);
	this->setGraphicsEffect(wndShadow);	

	calcTitleRect();

	ui.labGif_uninstall->hide();
	ui.labTxt_uninstall->hide();
	connect(ui.pushButton_min,SIGNAL(clicked()),this,SLOT(minBtnClicked()));
	connect(ui.pushButton_close,SIGNAL(clicked()),this,SLOT(closeBtnClicked()));
    connect(ui.pushButton_uninstall,SIGNAL(clicked()),this,SLOT(uninstallBtnClicked()));	
    connect(&m_thrdUninstall,SIGNAL(progress(int)),this,SLOT(uninstallProgress(int)));
}

MainDlg::~MainDlg()
{

}

void MainDlg::setWindowRoundCorner(QWidget *widget, int roundX, int roundY)
{
	QPainterPath path;
	QRectF rect = widget->rect();
	QRectF rectTop = rect;
	QRectF rectBot = rect;

	rectTop.setHeight(rectTop.height() - 3*roundY);
	rectBot.setTop(rectTop.bottom());
	
	path.addRect(rectTop);
	path.addRoundRect(rectBot, roundX, roundY);
	QPolygon polygon= path.toFillPolygon().toPolygon();
	QRegion region(polygon);
	widget->setMask(region);
}

void MainDlg::calcTitleRect()
{
	int rTop = 0;

	QLayout * layout = this->layout();
	if(layout)
	{
		layout->getContentsMargins(NULL,&rTop,NULL,NULL);
	}

	m_titlRect = this->rect();	

	m_titlRect.setTop(rTop);
	m_titlRect.setHeight(ui.pushButton_min->rect().height());
}


void MainDlg::mousePressEvent(QMouseEvent *event)
{
	if (m_titlRect.isEmpty())
	{
		return;
	}

	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		if (m_titlRect.contains(event->pos()))
		{
			m_moveable = true;
		}
		event->accept();
	}
}

void MainDlg::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton && m_moveable)
	{
		move(event->globalPos() - m_dragPosition);
	}
	event->accept();
}

void MainDlg::mouseReleaseEvent(QMouseEvent *event)
{
	m_moveable = false;
	event->accept();
}

void MainDlg::closeBtnClicked()
{		
	QDialog::close();
}

void MainDlg::closeEvent(QCloseEvent * event)
{
 	if(!m_thrdUninstall.isRunning())
 	{
 		event->accept(); 		
 	}
    else
    {
        event->ignore();
    }

	return;
}

void MainDlg::minBtnClicked()
{
	QDialog::showMinimized();
}

void MainDlg::uninstallBtnClicked()
{
    //判断是否客户端是否运行
    QString qstrProduct(QObject::tr("productFile"));
    std::string strProduct=qstrProduct.toStdString();
    DWORD nProcId = FindProcess(strProduct.c_str());
    if(nProcId > 0)
    {        
        if(QMessageBox::question(this,QString(tr("msgBox_title")),QString(tr("msgText_clientrunning")),QMessageBox::Yes,QMessageBox::No) == QMessageBox::No)
        {
            QDialog::close();
            return;
        }

        KillProcess(nProcId);
    }
    
    ui.pushButton_uninstall->hide();
    ui.labGif_uninstall->show();
	ui.labTxt_uninstall->show();

    ui.labGif_uninstall->setContentsMargins(0,0,0,0);
    ui.labGif_uninstall->setStyleSheet(QString("background-color:transparent;"));
	ui.labTxt_uninstall->setStyleSheet(QString("background-color:transparent;"));	
	ui.labTxt_uninstall->setText(QString(tr("start_uninstall")));
    QMovie* gifUninstall = new QMovie(QString(":/Image/res/image/uninstall.gif"));
    ui.labGif_uninstall->setMovie(gifUninstall);
    gifUninstall->start();

	m_thrdUninstall.start();
}

void MainDlg::uninstallProgress(int state)
{
    switch(state)
    {
    case UNINSTALL_DEL_REGEDIT:
        {
			ui.labTxt_uninstall->setText(QString(tr("delReg_uninstall")));
        }
        break;
    case UNINSTALL_DEL_SHORTCUT:
        {
			ui.labTxt_uninstall->setText(QString(tr("delSht_uninstall")));
        }
        break;
    case UNINSTALL_DEL_FILES:
        {
			ui.labTxt_uninstall->setText(QString(tr("delFile_uninstall")));
        }
        break;
    case UNINSTALL_FINISHED:
        {
			ui.labTxt_uninstall->setText(QString(tr("finish_uninstall")));

            m_thrdUninstall.exit(0);
            QDialog::close();

            g_uninstallFinished = true;
        }
        break;
    }
}