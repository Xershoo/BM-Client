#ifndef USERLISTITEMWIDGET_H
#define USERLISTITEMWIDGET_H

#include <QWidget>
#include "ui_userlistitemwidget.h"

class UserListItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserListItemWidget(QWidget *parent, QString userName, int loginType, int headIconType);
    UserListItemWidget(const UserListItemWidget &other);
    ~UserListItemWidget();
    
    bool eventFilter(QObject *o, QEvent *e);

    void setMsgHandler(QWidget *handler) {m_msgHandler = handler;}
    QWidget* getMsgHandler() {return m_msgHandler;}

    void setUserName(QString userName);
    QString getUserName() const {return m_userName;}

    void updateHeadIcon(QString path);
    QString headIconPath() {return m_userHeadIconPath;}

    void setSpeaking(bool speaking);
    bool isSpeaking() const {return m_isSpeaking;}

    void setHandsUp(bool handsUp) {ui.label_handsUp->setVisible(handsUp); m_handsUp = handsUp;}
    bool isHandsUp() const {return m_handsUp;}
    
    void setHasMic(bool hasMic) {ui.label_mic->setVisible(hasMic); m_hasMic = hasMic; }
    bool hasMic() const {return m_hasMic;}

    void setHasCamera(bool hasCamera) {ui.label_camera->setVisible(hasCamera); m_hasCamera = hasCamera;}
    bool hasCamera() const {return m_hasCamera;}

    void setHeadIconType(int iconType) {m_headIconType = iconType;}
    int getHeadIconType() const {return m_headIconType;}
    
    void setUserID(__int64 userID) {m_userID = userID;}
    __int64 userID() const {return m_userID;}

    bool isTeacher();
    bool isSelf();

protected:
    void clearMovie();
private:
    Ui::UserListItemWidget ui;
    QWidget *m_msgHandler;
    int m_loginType;             //0:移动端,其他有需要在加
    int m_headIconType;         // 0:老师男， 1：学生男， 2:老师女， 3:学生女
    bool m_hasMic;
    bool m_hasCamera;
    bool m_handsUp;
    bool m_hasUserHeadIcon;
    bool m_isSpeaking;
    __int64 m_userID;
    QString m_userHeadIconPath;
    QString m_userName;
};

#endif // USERLISTITEMWIDGET_H
