#include "welcomewidget.h"
#include "ui_welcomewidget.h"
#include <QDialog>
#include "env.h"
#include "lang.h"

qint64 getDiskFreeSpace(const QString& strDisk)
{
    if(strDisk.isEmpty())
    {
        return 0;
    }

    LPCWSTR lpcwstrDriver=(LPCWSTR)strDisk.utf16(); 
    ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes; 
    if( !::GetDiskFreeSpaceEx( lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) ) 
    { 
        return 0;  
    }

    return (quint64) liTotalFreeBytes.QuadPart;  
}

QString getMaxFreeSpaceDisk()
{
    QFileInfoList drvList=QDir::drives();

    QString diskOne;
    QString diskTwo;		
    qint64  fistFree = 0;
    qint64  secdFree = 0;

    for(int i=0; i<drvList.count();i++)
    {
        QFileInfo drvInfo = drvList.at(i);
        QString  drvPath = drvInfo.filePath();

        qint64 freeSize = getDiskFreeSpace(drvPath);
        if(freeSize > fistFree)
        {
            diskOne = drvPath;
            continue;
        }

        if(freeSize > secdFree)
        {
            diskTwo = drvPath;
            continue;
        }
    }

    if (diskOne.indexOf(QString("c:\\"),0,Qt::CaseInsensitive) == -1 &&
        diskOne.indexOf(QString("c:/"),0,Qt::CaseInsensitive) == -1)
    {
        return diskOne;
    }

    if(diskTwo.isEmpty())
    {
        return diskOne;
    }

    return diskTwo;
}

WelcomeWidget::WelcomeWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);
    ui->selfStart_checkBox->hide();
    ui->fast_install_Btn->setEnabled(false);
    ui->customInstall_Btn->setEnabled(false);
	ui->accept_checkBox->setChecked(true);
	QPalette palette;
	palette.setBrush(ui->welcome_title_widget->backgroundRole(), QBrush(QImage(":/image/res/image/install_start.png")));
	setPalette(palette);
	connect(ui->link_Btn,SIGNAL(clicked()),this,SLOT(on_link_Btn_clicked()));
	connect(ui->langSelect_Btn,SIGNAL(clicked()),this,SLOT(on_LangSelectBtn_clicked()));

	setMoveWidget(this->parentWidget());

	int langId = LangSet::getSystemLang();
	if(langId == LangSet::LANG_ENG)
	{
		ui->accept_checkBox->setMinimumWidth(110);
        ui->langSelect_Btn->setText(QString(tr("langChinese")));
	}
	else
	{
		ui->accept_checkBox->setMinimumWidth(95);
        ui->langSelect_Btn->setText(QString(tr("langEnglish")));
	}

    QString regKey(QObject::tr("ProductName"));
    std::string strKey=regKey.toStdString();
    LangSet::saveLangSet(strKey.c_str(),langId);
    
}

void WelcomeWidget::show(){
	BaseWidget::show();
	ui->welcome_title_widget->show();
}

void WelcomeWidget::hide(){
	ui->welcome_title_widget->hide();
	BaseWidget::hide();
}

WelcomeWidget::~WelcomeWidget()
{
    delete ui;
}

void WelcomeWidget::on_fast_install_Btn_clicked()
{
    QString diskPath = getMaxFreeSpaceDisk();
    if(diskPath.isEmpty())
    {
        diskPath = QString("C:\\");
    }

    QString setupPath = diskPath + QString(QObject::tr("Product"));
    ENV::createFolder(setupPath);
    m_setup->setConfigPath(setupPath);
    m_setup->run();
    emit sg_fastInstallBtnClick();
}

void WelcomeWidget::on_accept_checkBox_stateChanged(int arg1)
{
    if(2 == arg1)
    {
        ui->fast_install_Btn->setEnabled(true);
        ui->customInstall_Btn->setEnabled(true);
    }
    else
    {
        ui->fast_install_Btn->setEnabled(false);
        ui->customInstall_Btn->setEnabled(false);
    }
}

void WelcomeWidget::on_customInstall_Btn_clicked()
{
    emit sg_customInstallBtnClick();
}

void WelcomeWidget::on_close_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->close();
}

void WelcomeWidget::on_min_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->showMinimized();
}

void WelcomeWidget::on_link_Btn_clicked()
{
	emit sg_linkBtnClick();
}

void WelcomeWidget::setTitleBarRect()
{
    QPoint pt = ui->welcome_titleBat_widget->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui->welcome_titleBat_widget->size());
}

extern void setAppTranslator(int langId);

void WelcomeWidget::on_LangSelectBtn_clicked()
{
	bool langSel = ui->langSelect_Btn->isChecked();
	int langId = LangSet::getSystemLang();
	
	if(langId == LangSet::LANG_ENG)
	{        
		if(langSel)
		{
			langId = LangSet::LANG_CHA;
		}		
	}
	else
	{
		if(langSel)
		{
			langId = LangSet::LANG_ENG;
		}		
	}
		
	if(langId == LangSet::LANG_ENG)
	{
		ui->accept_checkBox->setMinimumWidth(110);		
	}
	else
	{
		ui->accept_checkBox->setMinimumWidth(95);		
	}

	setAppTranslator(langId);
    QString regKey(QObject::tr("ProductName"));
    std::string strKey=regKey.toStdString();
    LangSet::saveLangSet(strKey.c_str(),langId);
    qApp->processEvents();
}

void WelcomeWidget::retranslateUI()
{
	ui->retranslateUi(this);

    int langId = LangSet::getSystemLang();
    if(langId == LangSet::LANG_ENG)
    {        
        ui->langSelect_Btn->setText(QString(tr("langChinese")));
    }
    else
    {     
        ui->langSelect_Btn->setText(QString(tr("langEnglish")));
    }
}
