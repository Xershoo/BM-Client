#include "recordSettingdialog.h"
#include "CL8_EnumDefs.h"
#include "common/Env.h"
#include <QFileDialog>
#include "util.h"

QString recordSettingdialog::m_strPath = QString("");

recordSettingdialog::recordSettingdialog(QString strPath,QWidget *parent /* = 0 */)
     : C8CommonWindow(parent)
{
    ui.setupUi(this);
    setWindowRoundCorner(ui.widget, 3, 3);
    connect(ui.pushButton_Cancel, SIGNAL(clicked()), this, SLOT(closeBtnClicked()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(closeBtnClicked()));
    connect(ui.pushButton_OK, SIGNAL(clicked()), this, SLOT(okBtnClicked()));
    connect(ui.pushButton_change, SIGNAL(clicked()), this, SLOT(changeClicked()));
	
	m_strPath = strPath;
	if(m_strPath.isEmpty())
	{
		m_strPath = getDefaultPath();
	}

	ui.lineEdit_whiteboardname->setText(m_strPath);
	
    QString iconPath = Env::currentThemeResPath();
}

recordSettingdialog::~recordSettingdialog()
{

}

int recordSettingdialog::exec()
{
    return QDialog::exec();
}

void recordSettingdialog::setTitleBarRect()
{
    m_titlRect = ui.horizontalLayout_titleBar->contentsRect();
}

void recordSettingdialog::closeBtnClicked()
{
    QDialog::reject();
}

void recordSettingdialog::okBtnClicked()
{
    QDialog::accept();
}

void recordSettingdialog::changeClicked()
{
    QString floder = QFileDialog::getExistingDirectory(this, tr(""), "D:\\");
	floder.replace('/','\\');

    ui.lineEdit_whiteboardname->setText(floder);
	qApp->processEvents();

	m_strPath = floder;
}

QString recordSettingdialog::getRecordPath()
{
	return m_strPath;
}

QString recordSettingdialog::getDefaultPath()
{
    QString diskPath = Util::getMaxFreeSpaceDisk();
    if(diskPath.isEmpty())
    {
        diskPath = QString("C:\\");
    }

	QString defaultPath = diskPath + QString("class8Record\\");
	defaultPath.replace("/","\\");

	QDir	dirRecord(defaultPath);
	if(!dirRecord.exists(defaultPath))
	{
		dirRecord.mkdir(defaultPath);
	};
	
	return defaultPath;
}