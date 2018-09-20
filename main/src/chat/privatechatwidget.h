#ifndef PRIVATECHATWIDGET_H
#define PRIVATECHATWIDGET_H

#include <QWidget>
#include "ui_privatechatwidget.h"

#include "biz/BizInterface.h"
#include <QList>
#include <QString>
#include <QVariant>
#include "control/c8commonwindow.h"

class Chat;

class PrivateChatWidget : public C8CommonWindow
{
	Q_OBJECT

signals:
	void sg_destoryMyself(PrivateChatWidget*);
	void sg_destory();
public:
	PrivateChatWidget(QWidget *parent = 0);
	~PrivateChatWidget();
	ChatWidget* getChatWidget()const;
	void setTitleText(const QString &title);
private:
	Ui::PrivateChatWidget ui;

protected slots:
	void sl_click_close();
	void sl_click_min();
    void loadChatStyleFinished();

protected:
	virtual void setTitleBarRect();
};

#endif // PRIVATECHATWIDGET_H
