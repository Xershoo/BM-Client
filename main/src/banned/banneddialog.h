#ifndef BANNEDDIALOG_H
#define BANNEDDIALOG_H

#include "control\c8popupwindow.h"
#include "ui_banneddialog.h"

class BannedDialog : public C8PopupWindow
{
	Q_OBJECT

public:
	BannedDialog(QWidget *parent = 0);
	~BannedDialog();
	void setClassNameInStyleSheet();

	void setChatAble(bool chatAble);
	void setHandsUpAble(bool handsUpAble);

	bool getChatAble()const;
	bool getHandsUpAble()const;

private:
	Ui::BannedDialog ui;

protected slots:
	void chatAbleClicked();
	void handsUpAbleClicked();
};

#endif // BANNEDDIALOG_H
