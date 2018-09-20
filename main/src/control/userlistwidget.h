#ifndef USERLISTWIDGET_H
#define USERLISTWIDGET_H

#include <QListWidget>
#include "src/userlistmenue.h"

class UserListWidget : public QListWidget
{
    Q_OBJECT

public:
    UserListWidget(QWidget *parent);
    ~UserListWidget();
    
signals:
    void viewProfileActionTriggered(__int64 userID);
    void promotedToAssistant(__int64 userID);
    void askActionTriggered(__int64 userID);
    void disableSpeakActionTriggered(__int64 userID);
    void kickoutActionTriggered(__int64 userID);
    void chatWithActionTriggered(__int64 userID);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *e) override;
    bool isTriggeredSelf(const QPoint &pt);
    bool isTriggeredTea(const QPoint &pt);
    bool isTriggeredStu(const QPoint &pt);
    bool isTriggeredItemSpeaking(const QPoint &pt);
    bool isTriggeredItemHandsup(const QPoint &pt);
    bool isTriggeredItemDisableSpeaking(const QPoint &pt);
    bool isTriggeredItemAssistant(const QPoint &pt);
    __int64 triggeredItemUserID(const QPoint &pt);

protected slots:
	void sl_recvClickObj(int id);

private:
	UserListMenue *m_userListMenue;
};

#endif // USERLISTWIDGET_H
