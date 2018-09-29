#include "privatechatwidget.h"
#include "chatwidget.h"

PrivateChatWidget::PrivateChatWidget(QWidget *parent)
	: C8CommonWindow(parent,SHADOW_QT)
{
	ui.setupUi(this);
    if (!ui.private_innerChatWidget->isLoadFinished())
    {
        connect(ui.private_innerChatWidget, SIGNAL(sg_loadChatStyleFinished()), this, SLOT(loadChatStyleFinished()));
        ui.privateChat_send_pushBtn->setEnabled(false);	       
    }
    ui.private_innerChatWidget->setChatType(PRIVATE_CHAT); 
	connect(this->ui.privateChat_send_pushBtn, SIGNAL(clicked()), ui.private_innerChatWidget, SLOT(sendChatMsgBtnClicked()));
	connect(this->ui.pushButton_close, SIGNAL(clicked()), this, SLOT(sl_click_close()));
	connect(this->ui.privateChat_cancel_pushBtn, SIGNAL(clicked()), this, SLOT(sl_click_close()));
	connect(this->ui.pushButton_minSize, SIGNAL(clicked()), this, SLOT(sl_click_min()));

    resetContentsMargin(ui.verticalLayout_2);
}

PrivateChatWidget::~PrivateChatWidget()
{

}

void PrivateChatWidget::setTitleBarRect() 
{
	QPoint pt = ui.privateChat_title->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.privateChat_title->size());
}

void PrivateChatWidget::loadChatStyleFinished()
{
    ui.privateChat_send_pushBtn->setEnabled(true);
}

void PrivateChatWidget::sl_click_close()
{
	emit sg_destory();
	delete this;
}

void PrivateChatWidget::sl_click_min()
{
	showMinimized();
}

ChatWidget* PrivateChatWidget::getChatWidget()const
{
	return ui.private_innerChatWidget;
}


void PrivateChatWidget::setTitleText(const QString &title)
{
	ui.privateChat_title_label->setText(QString("%1").arg(title));
}