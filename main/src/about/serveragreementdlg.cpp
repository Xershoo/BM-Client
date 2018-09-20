#include "serveragreementdlg.h"

ServerAgreementDlg::ServerAgreementDlg(QWidget *parent)
	: C8CommonWindow(parent)
{
	ui.setupUi(this);
	ui.server_agree_text->adjustSize();
	ui.server_agree_text->setWordWrap(true);
	ui.server_agree_text->setAlignment(Qt::AlignTop);
	ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	connect(ui.pushButton_minSize, SIGNAL(clicked()), this, SLOT(showMinimized()));
	connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
}

ServerAgreementDlg::~ServerAgreementDlg()
{

}


void ServerAgreementDlg::setTitleBarRect()
{
	QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void ServerAgreementDlg::showMinimized()
{
	QDialog::showMinimized();
}

void ServerAgreementDlg::close()
{
	QDialog::close();
}