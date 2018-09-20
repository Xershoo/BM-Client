#include "selectpathwidget.h"
#include "ui_selectpathwidget.h"
#include <QDialog>
#include <QFileDialog>
#include <env.h>

extern QString getMaxFreeSpaceDisk();

SelectPathWidget::SelectPathWidget(QWidget *parent) :
    BaseWidget(parent),isCreateShortCutToMenue(false),
    ui(new Ui::SelectPathWidget)
{
    ui->setupUi(this);
    ui->path_Edit->setReadOnly(true);
	ui->select_title_widget->addPic(":/image/res/image/title_0.png");
	ui->select_title_widget->addPic(":/image/res/image/title_1.png");
	ui->select_title_widget->addPic(":/image/res/image/title_2.png");
    
    QString diskPath = getMaxFreeSpaceDisk();
    if(diskPath.isEmpty())
    {
        diskPath = QString("C:\\");
    }

    QString defSetupPath = diskPath + QString(QObject::tr("Product"));
	ui->path_Edit->setText(defSetupPath);

	connect(ui->select_shortCutToMenue_checkBox, SIGNAL(stateChanged(int)), this,SLOT(on_shortcutToMenue_checkBox_stateChanged(int)));
	setMoveWidget(this->parentWidget());
}

SelectPathWidget::~SelectPathWidget()
{
    delete ui;
}

void SelectPathWidget::on_min_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->showMinimized();
}

void SelectPathWidget::on_close_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->close();
}

void SelectPathWidget::on_install_Btn_clicked()
{
	m_setup->setConfigPath(ui->path_Edit->text());
    m_setup->run();
	if(isCreateShortCutToMenue)
		m_setup->createShortCutToStartMenue();
    emit sg_startInstall();
}

void SelectPathWidget::on_return_Btn_clicked()
{
    emit sg_retureClicked();
}

void SelectPathWidget::on_browse_Btn_clicked()
{    
    QString dirPath = QFileDialog::getExistingDirectory(this,QString(tr("selectDir")),ui->path_Edit->text());
    if(!dirPath.isEmpty())
	{
        ui->path_Edit->setText(dirPath);
    }
}

void SelectPathWidget::setTitleBarRect()
{
    QPoint pt = ui->select_titleBat_widget->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui->select_titleBat_widget->size());
}
void SelectPathWidget::show(){
	BaseWidget::show();
	ui->select_title_widget->show();
}

void SelectPathWidget::hide(){
	ui->select_title_widget->hide();
	BaseWidget::hide();
}

void SelectPathWidget::on_shortcutToMenue_checkBox_stateChanged(int arg1)
{
	if(2 == arg1)
		isCreateShortCutToMenue = true;
	else
		isCreateShortCutToMenue = false;
}

void SelectPathWidget::retranslateUI()
{
	ui->retranslateUi(this);
}