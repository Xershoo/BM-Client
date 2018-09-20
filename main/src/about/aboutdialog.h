#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "ui_aboutdialog.h"
#include "src/control/c8commonwindow.h"

class AboutDialog : public C8CommonWindow
{
	Q_OBJECT

public:
	AboutDialog(QWidget *parent = 0);
	~AboutDialog();
protected:
	virtual void setTitleBarRect() override;

private:
	Ui::AboutDialog ui;

protected slots:

	void showMinimized();
	void close();
	void showServerAgreementDlg();
};

#endif // ABOUTDIALOG_H
