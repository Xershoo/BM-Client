#ifndef PRIVATECHATDLG_H
#define PRIVATECHATDLG_H

#include <QDialog>
#include "ui_privatechatdlg.h"

class PrivateChatDlg : public QDialog
{
	Q_OBJECT

public:
	PrivateChatDlg(QWidget *parent = 0);
	~PrivateChatDlg();

private:
	Ui::PrivateChatDlg ui;
};

#endif // PRIVATECHATDLG_H
