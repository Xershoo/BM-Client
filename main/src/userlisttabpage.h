#ifndef USERLISTTABPAGE_H
#define USERLISTTABPAGE_H

#include <QWidget>
#include "ui_userlisttabpage.h"
#include <vector>
#include "userlistitemwidget.h"
#include <QAbstractItemDelegate>
#include "userinfowindow.h"

class QStandardItemModel;

struct UserListItemSortObj
{
    QListWidgetItem *item;

    friend bool operator < (UserListItemSortObj &other1, UserListItemSortObj &other)
    {
        UserListItemWidget *widget1 = (UserListItemWidget *)other1.item->listWidget()->itemWidget(other1.item);
        UserListItemWidget *widget2 = (UserListItemWidget *)other.item->listWidget()->itemWidget(other.item);

        if (widget1->isTeacher() || widget2->isTeacher())
        {
            if (widget1->isTeacher())
                return true;
            return false;
        }

        if (widget1->isSelf() || widget2->isSelf())
        {
            if (widget1->isSelf())
                return true;
            return false;
        }

        if (widget1->isSpeaking() || widget2->isSpeaking())
        {
            if (widget1->isSpeaking())
                return true;
            return false;
        }
        
        if (widget1->isHandsUp() || widget2->isHandsUp())
        {
            if (widget1->isHandsUp())
                return true;
            return false;
        }

        if (widget1->hasCamera() || widget2->hasCamera())
        {
            if (widget1->hasCamera())
                return true;
            return false;
        }

        if (widget1->hasMic() || widget2->hasMic())
        {
            if (widget1->hasMic())
                return true;
            return false;
        }
        
        return false;
    }

    friend bool operator > (UserListItemSortObj &other1, UserListItemSortObj &other)
    {
        return !(other1 < other);
    }
};

class SearchResultItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:

    explicit SearchResultItemDelegate(QObject *parent = 0);
    virtual ~SearchResultItemDelegate();

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setItemModel(QStandardItemModel *model) {m_model = model;}
    void setPopup(QAbstractItemView *popup) {m_popup = popup;}
protected:
    QStandardItemModel *m_model;
    QAbstractItemView *m_popup;
};

class UserListTabPage : public QWidget
{
    Q_OBJECT

signals:
    void chatWithOther(QString otherUser);
	void sg_privateChatCreate(__int64 userID);

public:
    UserListTabPage(QWidget *parent = 0);
    ~UserListTabPage();

    void addItem(__int64 userID, QString userName, int loginDeviceType, int headIconType);
    void removeItem(__int64 userID);
    bool userInList(__int64 userID);
    void updateUserName(__int64 userID, QString userName);
    void updateUserHeadIcon(__int64 userID, QString headIconPath);
    QString userHeadIcon(__int64 userID);
    void updateUserSpeakState(__int64 userID, bool speak);
    void updateUserHandsUpState(__int64 userID, bool handsUp);
    void updateUserHasMic(__int64 userID, bool hasMic);
    void updateUserHasCamera(__int64 userID, bool hasMic);
    void clearAllSpeakState();
    void reSortItems();
    void showUserInfoWindow(__int64 userID);
protected:
    void bubble_sort();
    void refreshResultModel();
    void refreshUserCountLabel();
    void removeItemFromSortList(QListWidgetItem *item);
protected slots:
    void searchUserBtnClicked();
    void searchLineEditEnterKeyPressed();

    void onViewProfileActionTriggered(__int64 userID);
    void onPromotedToAssistant(__int64 userID);
    void onAskActionTriggered(__int64 userID);
    void onDisableSpeakActionTriggered(__int64 userID);
    void onKickoutActionTriggered(__int64 userID);
    void onChatWithActionTriggered(__int64 userID);
	void doPrivateChat(__int64 userID);

private:
    Ui::UserListTabPage ui;
    std::vector<UserListItemSortObj> m_widgets;
    QStandardItemModel *m_searchResultModel;
    QCompleter *m_completer;

protected:
    UserInfoWindow *m_userInfoWindow;

};

#endif // USERLISTTABPAGE_H
