#include "finishwidget.h"
#include "ui_finishwidget.h"
#include <QDialog>
FinishWidget::FinishWidget(QWidget *parent) :
    BaseWidget(parent),isCreateShortcut(false),
    ui(new Ui::FinishWidget)
{
    ui->setupUi(this);
    ui->shortcut_checkBox->setChecked(true);
	setMoveWidget(this->parentWidget());
}

FinishWidget::~FinishWidget()
{
    delete ui;
}

void FinishWidget::on_min_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->showMinimized();
}

void FinishWidget::on_close_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->close();
}

void FinishWidget::on_startNow_Btn_clicked()
{
    m_setup->startExe();
    on_close_Btn_clicked();
}

void FinishWidget::on_finish_Btn_clicked()
{
    if(isCreateShortcut){
        m_setup->createShortCut();
    }
    on_close_Btn_clicked();
}

void FinishWidget::on_shortcut_checkBox_stateChanged(int arg1)
{
    if(2 == arg1)
        isCreateShortcut = true;
    else
        isCreateShortcut = false;
}

void FinishWidget::setTitleBarRect()
{
    QPoint pt = ui->finfish_titleBat_widget->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui->finfish_titleBat_widget->size());
}
void FinishWidget::show()
{
	BaseWidget::show();
	ui->finish_title_widget->show();
}

void FinishWidget::hide()
{
	ui->finish_title_widget->hide();
	BaseWidget::hide();
}

void FinishWidget::retranslateUI()
{
	ui->retranslateUi(this);
}