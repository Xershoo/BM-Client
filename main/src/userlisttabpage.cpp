#include "userlisttabpage.h"
#include <QListWidgetItem>
#include "session/classsession.h"
#include "common/Util.h"
#include <QStandardItemModel>
#include <QCompleter>
#include <QPainter>
#include "control/searchuserlineedit.h"
#include "../c8messagebox.h"
#include "BizInterface.h"
#include "src/userinfowindow.h"
#include "common/macros.h"

SearchResultItemDelegate::SearchResultItemDelegate(QObject *parent /* = 0 */) : m_model(NULL), m_popup(NULL), QAbstractItemDelegate(parent)
{

}

SearchResultItemDelegate::~SearchResultItemDelegate()
{

}

QSize SearchResultItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(0, 40);
}

void SearchResultItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state & QStyle::State_MouseOver || option.state & QStyle::State_Selected || option.state & QStyle::State_Active)
    {
        painter->fillRect(option.rect, QColor(qRgb(218, 218, 218)));
    }
    else
    {
        painter->fillRect(option.rect, QColor(qRgb(255, 255, 255)));
    }

    if (m_model && m_popup)
    {
        int pos = m_popup->model()->data(index, 0x0100).toInt();
        QStandardItem *item = m_model->item(pos);
        QIcon icon = item->icon();
        painter->drawPixmap(QPoint(5, (index.row() * 40) + 5), QPixmap(icon.pixmap(QSize(32, 32))));
        painter->drawText(QPoint(45, (index.row() * 40) + 25), item->text());
    }
}

UserListTabPage::UserListTabPage(QWidget *parent)
    : QWidget(parent)
	, m_completer(NULL)
	, m_userInfoWindow(NULL)
{
    ui.setupUi(this);
    ui.lineEdit_searchUser->hide();
    ui.pushButton_searchUser->setChecked(true);

    connect(ui.lineEdit_searchUser, SIGNAL(enterKeyPressed()), this, SLOT(searchLineEditEnterKeyPressed()));

    connect(ui.listWidget_userList, SIGNAL(viewProfileActionTriggered(__int64)), this, SLOT(onViewProfileActionTriggered(__int64)));
    connect(ui.listWidget_userList, SIGNAL(askActionTriggered(__int64)), this, SLOT(onAskActionTriggered(__int64)));
    connect(ui.listWidget_userList, SIGNAL(disableSpeakActionTriggered(__int64)), this, SLOT(onDisableSpeakActionTriggered(__int64)));
    connect(ui.listWidget_userList, SIGNAL(kickoutActionTriggered(__int64)), this, SLOT(onKickoutActionTriggered(__int64)));
    connect(ui.listWidget_userList, SIGNAL(chatWithActionTriggered(__int64)), this, SLOT(onChatWithActionTriggered(__int64)));
    connect(ui.listWidget_userList, SIGNAL(promotedToAssistant(__int64)), this, SLOT(onPromotedToAssistant(__int64)));

    m_searchResultModel = new QStandardItemModel(this);
    m_completer = new QCompleter(m_searchResultModel, this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    QAbstractItemView *popup = m_completer->popup();
    if (popup)
    {
        connect(popup, SIGNAL(activated(const QModelIndex &)), popup, SLOT(pressed(const QModelIndex &)));
        SearchResultItemDelegate *itemDelegate = new SearchResultItemDelegate(popup);
        itemDelegate->setItemModel(m_searchResultModel);
        itemDelegate->setPopup(popup);
        popup->setItemDelegate(itemDelegate);
    }

    m_completer->setFilterMode(Qt::MatchContains);
    ui.lineEdit_searchUser->setCompleter(m_completer);
}

UserListTabPage::~UserListTabPage()
{
   SAFE_DELETE(m_userInfoWindow);
}

void UserListTabPage::refreshUserCountLabel()
{
    QString userCount = QString(tr("userCount")).arg(m_widgets.size());
    ui.label_userCount->setText(userCount);
}

void UserListTabPage::addItem(__int64 userID, QString userName, int loginDeviceType, int headIconType)
{
    QListWidgetItem *item = new QListWidgetItem();
    UserListItemWidget *itemWidget = new UserListItemWidget(ui.listWidget_userList, userName, loginDeviceType, headIconType);
    itemWidget->setMsgHandler(this);
    itemWidget->setUserID(userID);
    ui.listWidget_userList->addItem(item);
    ui.listWidget_userList->setItemWidget(item, itemWidget);
    UserListItemSortObj obj;
    obj.item = item;
    m_widgets.push_back(obj);

    refreshUserCountLabel();
    refreshResultModel();
}

void UserListTabPage::removeItemFromSortList(QListWidgetItem *item)
{
    for (auto it = m_widgets.begin(); it != m_widgets.end(); it++)
    {
        if ((*it).item == item)
        {
            m_widgets.erase(it);
            break;
        }
    }
}

void UserListTabPage::removeItem(__int64 userID)
{
    if (!userInList(userID))
        return;

    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        if (UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item)))
        {
            if (itemWidget->userID() == userID)
            {
                removeItemFromSortList(item);
                /*
                // ui.listWidget_userList->removeItemWidget(item); 
                // removeItemWidget called deleteLater();
                */
                delete ui.listWidget_userList->itemWidget(item);
                delete ui.listWidget_userList->takeItem(i);
                refreshResultModel();
                ui.listWidget_userList->update();
                break;
            }
        }        
    }

    refreshUserCountLabel();
}

void UserListTabPage::bubble_sort()
{
    int i, j;
    UserListItemSortObj temp;
    int n = m_widgets.size();
    for (j = 0; j < n - 1; j++)
    {
        for (i = 0; i < n - 1 - j; i++)
        {
            if(m_widgets[i] > m_widgets[i + 1])
            {
                temp = m_widgets[i];
                m_widgets[i] = m_widgets[i + 1];
                m_widgets[i + 1] = temp;
            }
        }
    }
}

void UserListTabPage::reSortItems()
{
    bubble_sort();
    for (size_t i = 0; i < m_widgets.size(); i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        if (m_widgets[i].item != item)
        {
            UserListItemWidget *widget = (UserListItemWidget*)ui.listWidget_userList->itemWidget(m_widgets[i].item);
            UserListItemWidget *newWidget = new UserListItemWidget(*widget);
            newWidget->setMsgHandler(this);
            int row = ui.listWidget_userList->row(m_widgets[i].item);
            QListWidgetItem *item = ui.listWidget_userList->takeItem(row);
            ui.listWidget_userList->insertItem(i, item);
            ui.listWidget_userList->setItemWidget(item, newWidget);
        }
    }
}

bool UserListTabPage::userInList(__int64 userID)
{
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            if (itemWidget->userID() == userID)
            {
                return true;
            }
        }
    }
    return false;
}

void UserListTabPage::updateUserHeadIcon(__int64 userID, QString headIconPath)
{
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            if (itemWidget->userID() == userID)
            {
                itemWidget->updateHeadIcon(headIconPath);
                refreshResultModel();
                break;
            }
        }
    }
}

QString UserListTabPage::userHeadIcon(__int64 userID)
{
    QString iconPath;
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            if (itemWidget->userID() == userID)
            {
                iconPath = itemWidget->headIconPath();
                break;
            }
        }
    }
    return iconPath;
}

void UserListTabPage::updateUserSpeakState(__int64 userID, bool speak)
{
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            if (itemWidget->userID() == userID)
            {
                if (itemWidget->isSpeaking() != speak)
                {
                    itemWidget->setSpeaking(speak);
                    break;
                }
            }
        }
    }
    reSortItems();
}

void UserListTabPage::clearAllSpeakState()
{
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            itemWidget->setSpeaking(false);
            itemWidget->setHandsUp(false);
        }
    }
    reSortItems();
}

void UserListTabPage::updateUserHandsUpState(__int64 userID, bool handsUp)
{
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            if (itemWidget->userID() == userID)
            {
                if (itemWidget->isHandsUp() != handsUp)
                {
                    itemWidget->setHandsUp(handsUp);
                    break;  
                }              
            }
        }
    }
    reSortItems();
}

void UserListTabPage::updateUserHasMic(__int64 userID, bool hasMic)
{
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            if (itemWidget->userID() == userID)
            {
                if (itemWidget->hasMic() != hasMic)
                {
                    itemWidget->setHasMic(hasMic);
                    break;  
                }              
            }
        }
    }
}

void UserListTabPage::updateUserHasCamera(__int64 userID, bool hasCamera)
{
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            if (itemWidget->userID() == userID)
            {
                if (itemWidget->hasCamera() != hasCamera)
                {
                    itemWidget->setHasCamera(hasCamera);
                    break;
                }
            }
        }
    }
}

void UserListTabPage::updateUserName(__int64 userID, QString userName)
{
    int itemCount = ui.listWidget_userList->count();
    for (int i = 0 ;i < itemCount; i++)
    {
        QListWidgetItem *item = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget *>(ui.listWidget_userList->itemWidget(item));
        if (itemWidget)
        {
            if (itemWidget->userID() == userID)
            {
                itemWidget->setUserName(userName);
                refreshResultModel();
                break;
            }
        }
    }
}

void UserListTabPage::searchUserBtnClicked()
{
    bool checkAble = ui.pushButton_searchUser->isCheckable();
    if (ui.pushButton_searchUser->isChecked())
    {
        ui.pushButton_searchUser->setChecked(true);
        ui.lineEdit_searchUser->hide();
        ui.label_userCount->show();
    }
    else
    {
        ui.pushButton_searchUser->setChecked(false);
        ui.lineEdit_searchUser->show();
        ui.lineEdit_searchUser->setFocus();
        ui.label_userCount->hide();
    }
}

void UserListTabPage::refreshResultModel()
{
    m_searchResultModel->clear();
    
    for (int i = 0; i != ui.listWidget_userList->count(); i++)
    {
        QStandardItem *item = new QStandardItem();
        QListWidgetItem *listItem = ui.listWidget_userList->item(i);
        UserListItemWidget *itemWidget = qobject_cast<UserListItemWidget*>(ui.listWidget_userList->itemWidget(listItem));
        item->setIcon(QIcon(itemWidget->headIconPath()));
        QString userNameAndID = QString("%1(%2)").arg(itemWidget->getUserName()).arg(itemWidget->userID());
        item->setText(userNameAndID);
        m_searchResultModel->appendRow(item);
        int row = m_searchResultModel->indexFromItem(item).row();
        item->setData(QVariant(row), 0x0100);
    }
}

void UserListTabPage::searchLineEditEnterKeyPressed()
{
    QAbstractItemView *popup = m_completer->popup();
    QString selectedUser;
    if (popup->isVisible())
    {
        selectedUser = m_completer->currentCompletion();
        ui.lineEdit_searchUser->clear();
        ui.lineEdit_searchUser->setText(selectedUser);
        popup->hide();
    }
	else
	{
		selectedUser = ui.lineEdit_searchUser->text();
	}

	QStringList list = selectedUser.split('(');
	if(list.size() > 1)
	{
		selectedUser = list[1].split(')')[0];
	}
	emit sg_privateChatCreate(selectedUser.toLongLong());
}

void UserListTabPage::onViewProfileActionTriggered(__int64 userID)
{
    // 查看资料
    /*
	if (m_userInfoWindow)
    {
        m_userInfoWindow->close();
        delete m_userInfoWindow;
        m_userInfoWindow = NULL;
    }
	*/
    if (NULL == m_userInfoWindow)
    {
        m_userInfoWindow = new UserInfoWindow;
		connect(m_userInfoWindow, SIGNAL(sg_privateChatCreate(__int64)), this, SLOT(doPrivateChat(__int64)));
    }

	if(m_userInfoWindow->isVisible() && m_userInfoWindow->getUserId() == userID)
	{
		return;
	}

    QPoint point = cursor().pos();
    QSize size = m_userInfoWindow->size();
    point.setY(point.y());
    point.setX(point.x() - size.width());
    
    m_userInfoWindow->setGeometry(QRect(point, size));
	m_userInfoWindow->setHeadIconPath(userHeadIcon(userID));
	m_userInfoWindow->setUserInfo(userID);
	m_userInfoWindow->show();
}

void UserListTabPage::onChatWithActionTriggered(__int64 userID)
{
    // 与Ta 聊天
	emit sg_privateChatCreate(userID);
}

void UserListTabPage::onPromotedToAssistant(__int64 userID)
{
	if(!ClassSeeion::GetInst()->IsTeacher())
	{
		return;
	}

    //提升为助教
    auto sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
    if (sInfo.nUserAuthority == biz::UserAu_Assistant)
    {
		biz::GetBizInterface()->SetStudentAsAssistant(ClassSeeion::GetInst()->_nClassRoomId, ClassSeeion::GetInst()->_nUserId, userID, true);
	}
    else
    {
		biz::GetBizInterface()->SetStudentAsAssistant(ClassSeeion::GetInst()->_nClassRoomId, ClassSeeion::GetInst()->_nUserId, userID, false);
	}
    
}

void UserListTabPage::onAskActionTriggered(__int64 userID)
{
    // 提问 允许发言 取消发言

    auto pSpeakUserInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId)->GetSpeakUserInfo();
    if(NULL != pSpeakUserInfo)
    {
        //如果选中的用户正在发言， 取消发言
        biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, pSpeakUserInfo->nUserId, biz::eUserspeekstate_clean_speak, ClassSeeion::GetInst()->_nUserId);
        if (pSpeakUserInfo->nUserId == userID)
            return;
    }

    auto sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);		
    biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, userID, biz::eUserspeekstate_allow_speak, ClassSeeion::GetInst()->_nUserId);
}

void UserListTabPage::onDisableSpeakActionTriggered(__int64 userID)
{
    // 禁言
    auto sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
    if (sInfo._nUserState & biz::eUserState_user_mute)
        biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, userID, biz::eClassAction_unmute, ClassSeeion::GetInst()->_nUserId);
    else
        biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, userID, biz::eClassAction_mute, ClassSeeion::GetInst()->_nUserId);
    
}

void UserListTabPage::onKickoutActionTriggered(__int64 userID)
{
    // 踢出课堂
    auto sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
    QString strName = QString::fromWCharArray(sInfo.szRealName);
    if(strName.isEmpty())
    {
        strName = QString::fromWCharArray(sInfo.szNickName);
    }

    QString qstrMsg = QString(tr("askKickStudent")).arg(strName);
    C8MessageBox msgBox(C8MessageBox::Question, QString(tr("info")), qstrMsg);
    if (msgBox.exec())
        biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, userID, biz::eClassAction_Kicout, ClassSeeion::GetInst()->_nUserId);
}

void UserListTabPage::doPrivateChat(__int64 userID)
{
	emit sg_privateChatCreate(userID); 
}

void UserListTabPage::showUserInfoWindow(__int64 userID)
{
    onViewProfileActionTriggered(userID);
}