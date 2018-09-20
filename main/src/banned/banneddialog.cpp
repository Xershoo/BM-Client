#include "banneddialog.h"

BannedDialog::BannedDialog(QWidget *parent)
	: C8PopupWindow(parent,"")
{
	ui.setupUi(this);

	connect(ui.chatAble_CBox, SIGNAL(clicked()), this, SLOT(chatAbleClicked()));
	connect(ui.handsUpAble_CBox, SIGNAL(clicked()), this, SLOT(handsUpAbleClicked()));
}

BannedDialog::~BannedDialog()
{

}
void BannedDialog::setClassNameInStyleSheet()
{
	setStyleSheet("QWidget{background-color:rgb(255,255,255);}");
}


void BannedDialog::setChatAble(bool chatAble)
{
	this->ui.chatAble_CBox->setChecked(chatAble);
}

void BannedDialog::setHandsUpAble(bool handsUpAble)
{
	this->ui.handsUpAble_CBox->setChecked(handsUpAble);
}

bool BannedDialog::getChatAble()const
{
	return this->ui.chatAble_CBox->isChecked();
}
bool BannedDialog::getHandsUpAble()const
{
	return this->ui.handsUpAble_CBox->isChecked();
}

void BannedDialog::chatAbleClicked()
{
	//处理，发送消息或者其他
}
void BannedDialog::handsUpAbleClicked()
{
	//处理
}