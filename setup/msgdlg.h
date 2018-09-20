#ifndef MSGDLG_H
#define MSGDLG_H

#include <QWidget>
#include <QDialog>
#include "ui_msgdlg.h"
#include "basewidget.h"

class MsgDlg : public BaseWidget
{
	Q_OBJECT

public:
	MsgDlg(QWidget *parent = 0);
	~MsgDlg();

	void retranslateUI()
	{
	};
	
protected:
	virtual void setTitleBarRect();
	void setWindowRoundCorner(QWidget *widget, int roundX, int roundY);
private slots:
	void on_close_Btn_clicked();
private:
	Ui::msgDlg ui;
};

#endif // MSGDLG_H
