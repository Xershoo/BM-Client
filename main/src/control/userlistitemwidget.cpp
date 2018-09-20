#include "userlistitemwidget.h"
#include <QPixmap>
#include <QMovie>
#include "../session/classsession.h"
#include "../userlisttabpage.h"
#include "Env.h"
#include <qDebug>

UserListItemWidget::UserListItemWidget(QWidget *parent, QString userName, int loginType, int headIconType)
    : m_msgHandler(NULL), m_userName(userName), m_loginType(loginType), m_headIconType(headIconType), m_hasUserHeadIcon(false), QWidget(parent)
{
    ui.setupUi(this);
    if (isTeacher())
    {
        ui.label_userNameStu->hide();
        ui.label_userNameTea->show();
        ui.label_userNameTea->setText(m_userName);
    }
    else
    {
        ui.label_userNameTea->hide();
        ui.label_userNameStu->show();
        ui.label_userNameStu->setText(m_userName);
    }

    if (m_loginType == 0)
    {
        ui.label_userLoginType->hide();
    }
    else
    {
        ui.label_userLoginType->show();
    }
    ui.label_headIcon->clear();
    QString headIconPath = QString("userListHeadIcon_%1.png").arg(m_headIconType);
    headIconPath = Env::currentThemeResPath() + headIconPath;
    QPixmap pix(headIconPath);
    QSize size = ui.label_headIcon->sizeHint();
    pix = pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_headIcon->setPixmap(pix);
    m_userHeadIconPath = headIconPath;
    setHandsUp(false);
    setSpeaking(false);
    ui.label_headIcon->installEventFilter(this);
}

UserListItemWidget::UserListItemWidget(const UserListItemWidget &other)
{
    ui.setupUi(this);
    m_headIconType = other.m_headIconType;
    m_hasUserHeadIcon = other.m_hasUserHeadIcon;
    m_userHeadIconPath = other.m_userHeadIconPath;
    m_loginType = other.m_loginType;
    if (m_loginType == 0)
    {
        ui.label_userLoginType->hide();
    }
    else
    {
        ui.label_userLoginType->show();
    }
    QPixmap pix(m_userHeadIconPath);
    QSize size = ui.label_headIcon->sizeHint();
    pix = pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_headIcon->setPixmap(pix);

    this->setHasMic(other.hasMic());
    this->setUserName(other.getUserName());
    this->setHasCamera(other.hasCamera());
    this->setHandsUp(other.isHandsUp());
    this->setSpeaking(other.isSpeaking());
    this->setUserID(other.userID());

    if (m_hasUserHeadIcon)
    {
        updateHeadIcon(m_userHeadIconPath);
    }
    ui.label_headIcon->installEventFilter(this);
}

UserListItemWidget::~UserListItemWidget()
{
    clearMovie();
}

bool UserListItemWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui.label_headIcon)
    {
        if (e->type() == QEvent::Enter)
        {
            if (UserListTabPage *page = qobject_cast<UserListTabPage*>(m_msgHandler))
                page->showUserInfoWindow(m_userID);
        }
    }
    return QWidget::eventFilter(o, e);
}

void UserListItemWidget::clearMovie()
{
    QMovie *movie = ui.label_speakState->movie();
    if (movie)
    {
        movie->stop();
        delete movie;
        ui.label_speakState->clear();
    }
}

void UserListItemWidget::setSpeaking(bool speaking)
{
    m_isSpeaking = speaking;
    clearMovie();
    if (speaking)
    {
        QString gifPath = Env::currentThemeResPath() + "label_speakState.gif";
        QMovie *moive = new QMovie(gifPath);
        ui.label_speakState->setMovie(moive);
        moive->start();
        ui.label_speakState->setVisible(true);
    }
    else
    {
        ui.label_speakState->setVisible(false);
    }
    
}

void UserListItemWidget::setUserName(QString userName)
{
    m_userName = userName;
    if (isTeacher())
    {
        ui.label_userNameStu->hide();
        ui.label_userNameTea->show();
        ui.label_userNameTea->setText(m_userName);
    }
    else
    {
        ui.label_userNameTea->hide();
        ui.label_userNameStu->show();
        ui.label_userNameStu->setText(m_userName);
    }
}

void UserListItemWidget::updateHeadIcon(QString path)
{
    ui.label_headIcon->clear();
    m_hasUserHeadIcon = true;
    m_userHeadIconPath = path;
    QPixmap pix(m_userHeadIconPath);
    QSize size = ui.label_headIcon->sizeHint();
    pix = pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_headIcon->setPixmap(pix);
}

bool UserListItemWidget::isTeacher()
{
    if (m_headIconType == 0 || m_headIconType == 2)
        return true;
    return false;
}

bool UserListItemWidget::isSelf()
{
    if (ClassSeeion::GetInst()->_nUserId == m_userID)
        return true;
    return false;
}