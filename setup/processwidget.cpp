#include "processwidget.h"
#include "ui_processwidget.h"
#include <QDialog>
ProcessWidget::ProcessWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::ProcessWidget)
{
    ui->setupUi(this);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    ui->close_Btn->setEnabled(false);
	ui->process_title_widget->addPic(":/image/res/image/title_0.png");
	ui->process_title_widget->addPic(":/image/res/image/title_1.png");
	ui->process_title_widget->addPic(":/image/res/image/title_2.png");
	setMoveWidget(this->parentWidget());
}

ProcessWidget::~ProcessWidget()
{
    delete ui;
}

void ProcessWidget::on_min_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->showMinimized();
}

void ProcessWidget::on_close_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->close();
}

void ProcessWidget::uncompressProcess(int value)
{
    ui->progressBar->setValue(value);
    if(100 == value)
    {
        ui->progressBar->setValue(ui->progressBar->maximum());
        emit sg_installSucess();
    }
}

void ProcessWidget::setTitleBarRect()
{
    QPoint pt = ui->process_titleBat_widget->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui->process_titleBat_widget->size());
}

void ProcessWidget::show(){
	BaseWidget::show();
	ui->process_title_widget->show();
}

void ProcessWidget::hide(){
	ui->process_title_widget->hide();
	BaseWidget::hide();
}

void ProcessWidget::retranslateUI()
{
	ui->retranslateUi(this);
}