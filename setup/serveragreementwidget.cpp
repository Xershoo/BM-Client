#include "serveragreementwidget.h"
#include "ui_serveragreementwidget.h"
#include <QDialog>
#include <QFile>
#include <QDebug>
#include <QTextCodec>
ServerAgreementWidget::ServerAgreementWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::ServerAgreementWidget)
{
    ui->setupUi(this);

	QFile file(":/lang/res/lang/readme.txt");
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
	}
	ui->agreement_label->adjustSize();
	ui->agreement_label->setWordWrap(true);
	ui->agreement_label->setAlignment(Qt::AlignTop);
	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui->agreement_label->setText(tr(file.readAll().data()));
	setMoveWidget(this->parentWidget());
}

ServerAgreementWidget::~ServerAgreementWidget()
{
    delete ui;
}

void ServerAgreementWidget::on_cancle_Btn_clicked()
{
    emit sg_cancelBtnClicked();
}

void ServerAgreementWidget::on_min_Btn_clicked()
{
    ((QDialog*)this->parentWidget())->showMinimized();
}

void ServerAgreementWidget::on_close_Btn_clicked()
{
    emit sg_cancelBtnClicked();
}

void ServerAgreementWidget::setTitleBarRect(){
	QPoint pt = ui->serverAgreement_titleBat_widget->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui->serverAgreement_titleBat_widget->size());
}

void ServerAgreementWidget::retranslateUI()
{
	ui->retranslateUi(this);
}