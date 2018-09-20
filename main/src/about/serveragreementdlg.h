#ifndef SERVERAGREEMENTDLG_H
#define SERVERAGREEMENTDLG_H

#include <QDialog>
#include "ui_serveragreementdlg.h"

#include "src/control/c8commonwindow.h"

class ServerAgreementDlg : public C8CommonWindow
{
	Q_OBJECT

public:
	ServerAgreementDlg(QWidget *parent = 0);
	~ServerAgreementDlg();

protected:
	virtual void setTitleBarRect() override;

protected slots:
	void showMinimized();
	void close();

private:
	Ui::ServerAgreementDlg ui;
};

#endif // SERVERAGREEMENTDLG_H
