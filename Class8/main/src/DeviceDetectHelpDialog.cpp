#include "DeviceDetectHelpDialog.h"

DeviceDetectHelpDialog::DeviceDetectHelpDialog(QWidget* parent/* =NULL */):C8CommonWindow(parent,SHADOW_AERO)
{
	ui.setupUi(this);
	
	connect(ui.pushButton_close,SIGNAL(clicked()),this,SLOT(exitDetect()));
	connect(ui.pushButton_exitDetect,SIGNAL(clicked()),this,SLOT(exitDetect()));
	connect(ui.pushButton_skipDetect,SIGNAL(clicked()),this,SLOT(skipDetect()));

	this->centerWindow();
}

void DeviceDetectHelpDialog::setTitleBarRect()
{
	QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void DeviceDetectHelpDialog::exitDetect()
{
	C8CommonWindow::reject();
}

void DeviceDetectHelpDialog::skipDetect()
{
	C8CommonWindow::accept();
}