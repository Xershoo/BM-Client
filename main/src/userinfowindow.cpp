#include "userinfowindow.h"
#include <QPixmap>
#include <QDesktopServices>
#include <QImage>
#include "BizInterface.h"
#include "./token/LoginTokenMgr.h"
#include "common/Config.h"
#include "./session/classsession.h"
#include <QUrl>

UserInfoWindow::UserInfoWindow(QWidget *parent)
    : C8CommonWindow(parent)
{
    ui.setupUi(this);

    connect(ui.label_home_page, SIGNAL(linkActivated(QString)), this, SLOT(doShowHomePage(QString)));
    connect(ui.label_whisper, SIGNAL(linkActivated(QString)), this, SLOT(doPrivateChat(QString)));

    connect(CLoginTokenMgr::GetInstance(), SIGNAL(recvLoginToken(LPCWSTR)), this, SLOT(onRecvLoginToken(LPCWSTR)));

}

UserInfoWindow::~UserInfoWindow()
{

}

void UserInfoWindow::setTitleBarRect()
{
    return;
}

void UserInfoWindow::setHeadIconPath(QString headIconPath)
{
    ui.label_useInfoHeadIcon->clear();
    QPixmap pix(headIconPath);
    QSize size = ui.label_useInfoHeadIcon->sizeHint();
    pix = pix.scaled(size - QSize(6, 6), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_useInfoHeadIcon->setPixmap(pix);
}

void UserInfoWindow::show()
{
	if(!m_inWindow)
		m_closeTimerId = startTimer(USERINFO_CLOSE_TIME);
	QWidget::show();
}

void UserInfoWindow::timerEvent(QTimerEvent *event)
{
	if(event->timerId() == m_closeTimerId)
	{
		killTimer(m_closeTimerId);
		m_closeTimerId = 0;
		this->close();
	}
}

void UserInfoWindow::setUserInfo(__int64 userID)
{
    ui.label_home_page->clear();
    ui.label_whisper->clear();
    ui.label_teacher_flag->clear();
    ui.label_teacher_id->clear();
    ui.label_teacher_nickname->clear();
    ui.label_teacher_name->clear();
    ui.label_teacher_sex->clear();
    ui.label_useInfoTea->clear();

    m_inWindow = false;
    m_nUserID = 0;
    m_bISClickedHome = false;

    QString strContent;
    QString homepage = QString(tr("homePage"));
    strContent = "<a style='color: rgb(60, 60, 60); text-decoration:none;' href = " 
        + QString(Config::getConfig()->m_urlWebSite.c_str()) 
        + "> " + homepage + "</a>";

    ui.label_home_page->setText(strContent);

    QString whicher = QString(tr("privateChat"));
    strContent = "<a style='color: rgb(60, 60, 60); text-decoration:none;' href = " 
        + QString(Config::getConfig()->m_urlWebSite.c_str()) 
        + "> " + whicher + "</a>";

    ui.label_whisper->setText(strContent);
    SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
    
    QString flag = QString(tr("userinfo_Flag"));
    QString id = QString(tr("userinfo_ID"));
    QString nickName = QString(tr("userinfo_NickName"));
    QString name = QString(tr("userinfo_Name"));
    QString sex = QString(tr("userinfo_Sex"));
    QString call = "";

    if (NULL == userInfo.szRealName || NULL == userInfo.szRealName[0])
    {
    }
    else
    {
        call = QString::fromWCharArray(&(userInfo.szRealName[0]));
    }
    if (biz::UserAu_Teacher == userInfo.nUserAuthority)
    {
        ui.label_teacher_flag->setText(QString("%1%2").arg(flag).arg(tr("FlagTeacher")));
        call += QString("%1").arg(tr("FlagTeacher"));
    }
    else
    {
        ui.label_teacher_flag->setText(QString("%1%2").arg(flag).arg(tr("FlagStudent")));
        call += QString("%1").arg(tr("FlagStudent"));
    }
    ui.label_useInfoTea->setText(call);
    ui.label_teacher_id->setText(QString("%1%2").arg(id).arg(userInfo.nUserId));
    m_nUserID = userInfo.nUserId;
    ui.label_teacher_nickname->setText(QString("%1%2").arg(nickName).arg(QString::fromWCharArray(userInfo.szNickName)));
    ui.label_teacher_name->setText(QString("%1%2").arg(name).arg(QString::fromWCharArray(userInfo.szRealName)));
    if (biz::Gender_Male == userInfo.nGender)
    {
        ui.label_teacher_sex->setText(QString("%1%2").arg(sex).arg(tr("GenderMale")));
    }
    else
    {
        ui.label_teacher_sex->setText(QString("%1%2").arg(sex).arg(tr("GenderFemale")));
    }
    this->update();
}

bool UserInfoWindow::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == this)
	{
		if (event->type() == QEvent::Enter)
		{
			m_inWindow = true;
			if(m_closeTimerId != 0)
			{
				killTimer(m_closeTimerId);
				m_closeTimerId = startTimer(USERINFO_CLOSE_TIME);
			}
		}
		if (event->type() == QEvent::Leave)
		{
			if(m_closeTimerId == 0)
			{
				killTimer(m_closeTimerId);
				m_closeTimerId = startTimer(USERINFO_CLOSE_TIME);
			}
			m_inWindow = false;
		}

		if (event->type() == QEvent::MouseButtonPress)
		{
			if (!m_inWindow)
			{
				this->setVisible(false);
			}
		}
	}

	return QWidget::eventFilter(obj, event);
}

void UserInfoWindow::setGeometry(const QRect &rect)
{
    QWidget::setGeometry(rect);
}

void UserInfoWindow::doShowHomePage(QString url)
{
    m_bISClickedHome = true;
    WCHAR szToken[MAX_PATH] = {0};
    if (CLoginTokenMgr::GetInstance()->GetLoginToken(ClassSeeion::GetInst()->_nUserId, szToken))
    {
        showHomePage(szToken);
    }
}

void UserInfoWindow::doPrivateChat(QString url)
{
	emit sg_privateChatCreate(m_nUserID);
}

void UserInfoWindow::onRecvLoginToken(LPCWSTR wszToken)
{
    if (NULL == wszToken || NULL == wszToken[0])
    {
        return;
    }
    showHomePage(wszToken);
}

void UserInfoWindow::showHomePage(LPCWSTR wszToken)
{
    if (!m_bISClickedHome)
    {
        return;
    }
    m_bISClickedHome = false;
    if (NULL == wszToken || NULL == wszToken[0])
    {
        return;
    }

    QString classurl(Config::getConfig()->m_urlWebSite.c_str());
    QString token = QString::fromWCharArray(wszToken);
    QString url;

    //Õ‚Õ¯
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        url = QString("%1infocenter/teacher/%2").arg(classurl).arg(m_nUserID);
    }
    else
    {
        //Õ‚Õ¯
        url = QString("%1infocenter/student/%2").arg(classurl).arg(m_nUserID);
    }

    QDesktopServices::openUrl(QUrl(url));
}