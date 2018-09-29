#include "aboutdialog.h"
#include "serveragreementdlg.h"
#include "common/Env.h"

AboutDialog::AboutDialog(QWidget *parent)
	: C8CommonWindow(parent,SHADOW_AERO)
{
	ui.setupUi(this);
	ui.pushButton_link->adjustSize();
	connect(ui.pushButton_minSize, SIGNAL(clicked()), this, SLOT(showMinimized()));
	connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.pushButton_link, SIGNAL(clicked()), this, SLOT(showServerAgreementDlg()));
    setWindowRoundCorner(ui.widget_about_Wnd, 2, 2);
    QString version = QString("%1").arg(ui.label_version->text()).arg(Env::GetGTVersion());
    ui.label_version->setText(version);

	resetContentsMargin(ui.verticalLayout);
}


AboutDialog::~AboutDialog()
{

}

void AboutDialog::setTitleBarRect()
{
	QPoint pt = ui.widget_aboutTitleBar->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.widget_aboutTitleBar->size());
}

void AboutDialog::showMinimized()
{
	QDialog::showMinimized();
}

void AboutDialog::close()
{
	QDialog::close();
}

void AboutDialog::showServerAgreementDlg()
{
	ServerAgreementDlg().exec();
}