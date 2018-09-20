#include "chatwidget.h"
#include "ui_chatwidget.h"
#include <QTextDocumentFragment>
#include <QImageReader>
#include <QMovie>
#include <QTime>
#include <QWebView>
#include <QWebElementCollection>
#include <QWebPage>
#include <QWebFrame>
#include <QXmlStreamReader>
#include <time.h>
#include <QByteArray>

#include "common/HttpSessionMgr.h"
#include "define/chatwindowtextblockuserdata.h"
#include "chatwindowoutputedit.h"
#include "control/fullccreenwidget.h"
#include "common/Util.h"
#include "biz/BizInterface.h"
#include "session/classsession.h"
#include "control/chatwindowoutputedit.h"
#include "common/Env.h"
#include "common/Config.h"
#include "token/UploadTokenMgr.h"
#include <QFile>
#include "src/chat/chatmanager.h"
#include "src/chat/chat.h"
#include "src/chat/chatupdownserver.h"
#include <QDebug>
#include "MediaPublishMgr.h"
#include "pictureviewer.h"

ChatWidget::ChatWidget(QWidget *parent) : QWidget(parent)
	,m_msgNum(0)
	,m_isLoadFinished(false)
	,m_enableChat(true)
	,ui(new Ui::ChatWidget)
    ,m_timerCheckSliderWidget(0)
{
    ui->setupUi(this);
	ui->textEdit_input->setReadOnly(false);
	ui->soundDisable_pushBtn->hide();
	ui->micDisable_pushBtn->hide();
	ui->cameraDisabel_pushBtn->hide();

	ui->textEdit_input->setText(tr("loading chat style..."));
	ui->textEdit_input->setEnabled(false);
	ui->showFaceSelectWindow_pushBtn->setEnabled(false);
	ui->screenShot_pushBtn->setEnabled(false);
	ui->send_pushBtn->setEnabled(false);

	ui->webView->setContextMenuPolicy (Qt::NoContextMenu);
	ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("ChatWidget",this);
    ui->webView->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    ui->webView->page()->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavaScriptObject()));
	m_fullWidget = new fullScreenWidget;
	m_soundSliderWidget.setRange(0,100);
	m_micophoneSliderWidget.setRange(0,100);

	connect(&m_faceSelectWindow, SIGNAL(sg_selectface(QString)), this, SLOT(insertFace(QString)));
	connect(this, SIGNAL(setPixmap(QPixmap)), m_fullWidget, SLOT(loadBackgroundPixmap(QPixmap)));
	connect(m_fullWidget, SIGNAL(finishPixmap(QPixmap)), this, SLOT(getScreenShotPix(QPixmap)));

	connect(this->ui->send_pushBtn, SIGNAL(clicked()), this, SLOT(sendChatMsgBtnClicked()));
	connect(this->ui->showFaceSelectWindow_pushBtn, SIGNAL(clicked()), this, SLOT(showFaceSelectWindowBtnClicked()));
	connect(this->ui->screenShot_pushBtn, SIGNAL(clicked()), this, SLOT(screenShotBtnClicked()));

	connect(ui->camera_pushBtn,SIGNAL(clicked()),this,SLOT(cameraEnableBtnClicked()));
	connect(ui->cameraDisabel_pushBtn,SIGNAL(clicked()),this,SLOT(cameraDisableBtnClicked()));

	connect(ui->micEnable_pushBtn, SIGNAL(clicked()), this, SLOT(micEnableBtnClicked()));
	connect(ui->micDisable_pushBtn, SIGNAL(clicked()), this, SLOT(micDisableBtnClicked()));
	connect(&m_micophoneSliderWidget,SIGNAL(sliderValueChanged(int)),this,SLOT(micophoneSliderChange(int)));

	connect(ui->soundEnable_pushBtn, SIGNAL(clicked()), this, SLOT(soundEnableBtnClicked()));
	connect(ui->soundDisable_pushBtn, SIGNAL(clicked()), this, SLOT(soundDisableBtnClicked()));
	connect(&m_soundSliderWidget,SIGNAL(sliderValueChanged(int)),this,SLOT(soundSliderChange(int)));
	
	connect(ui->webView->page()->mainFrame(), SIGNAL(loadFinished(bool)), this, SLOT(loadChatStyleFinished(bool)));
	loadChatStyle(); 

    ui->textEdit_input->installEventFilter(this);
    ui->soundEnable_pushBtn->installEventFilter(this);
    ui->micEnable_pushBtn->installEventFilter(this);
}

void ChatWidget::loadChatStyle()
{
    QFile source(":/chatstyle/res/chatstyle/test.html");
    if (source.open(QIODevice::ReadOnly))
    {
        ui->webView->setHtml(QString::fromUtf8(source.readAll().constData()));
    }
    source.close();
}

void ChatWidget::loadChatStyleFinished(bool finished)
{
    if (finished)
    {
        if (!m_isLoadFinished)
        {
			ui->textEdit_input->setText("");
		}

        ui->textEdit_input->setEnabled(true);
        ui->send_pushBtn->setEnabled(m_enableChat);
        ui->showFaceSelectWindow_pushBtn->setEnabled(true);
        ui->screenShot_pushBtn->setEnabled(true);
        update();
        m_isLoadFinished = true;
        emit sg_loadChatStyleFinished();
    }
}

bool ChatWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->textEdit_input)
    {
        if (e->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
            int keyPress = keyEvent->key();            
            if ((keyPress == Qt::Key_Enter || keyPress == Qt::Key_Return) && (keyEvent->modifiers() == Qt::NoModifier))
            {
                sendChatMsgBtnClicked(); //发送消息的槽
                return true;
            }
        }
        return false;
    }

    if (o==ui->micEnable_pushBtn)
    {
        if(e->type() == QEvent::Enter)
        {
            QSize size = m_micophoneSliderWidget.size();
            QPoint ptGlobal = ui->micEnable_pushBtn->mapToGlobal(QPoint(0, 0));
            ptGlobal.setY(ptGlobal.y() - size.height()-10);
            ptGlobal.setX(ptGlobal.x() - (size.width()- ui->micEnable_pushBtn->size().width()) / 2);

            m_micophoneSliderWidget.setSliderValue(CMediaPublishMgr::getInstance()->getMicVolume());
            m_micophoneSliderWidget.setGeometry(QRect(ptGlobal, size));
            m_micophoneSliderWidget.show();
            m_micophoneSliderWidget.raise();

            m_timerCheckSliderWidget = startTimer(400);
        }
        else if(e->type() == QEvent::Leave)
        {
            
        }

        return false;
    }

    if (o==ui->soundEnable_pushBtn)
    {
        if(e->type() == QEvent::Enter)
        {
            QSize size = m_soundSliderWidget.size();
            QPoint ptGlobal = ui->soundEnable_pushBtn->mapToGlobal(QPoint(0, 0));
            ptGlobal.setY(ptGlobal.y() - size.height()-10);
            ptGlobal.setX(ptGlobal.x() - (size.width() - ui->soundEnable_pushBtn->size().width()) / 2);

            m_soundSliderWidget.setSliderValue(CMediaPublishMgr::getInstance()->getSpeakersVolume());
            m_soundSliderWidget.setGeometry(QRect(ptGlobal, size));
            m_soundSliderWidget.show();
            m_soundSliderWidget.raise(); 

            m_timerCheckSliderWidget = startTimer(400);
        }
        else if(e->type() == QEvent::Leave)
        {
            
        }

        return false;
    }

    return QWidget::eventFilter(o, e);
}

void ChatWidget::addJavaScriptObject()
{
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("ChatWidget",this);
}

void ChatWidget::setChatType(ChatType chatType)
{
	switch (chatType)
	{
	case PRIVATE_CHAT:
		ui->send_pushBtn->hide();
		ui->micDisable_pushBtn->hide();
		ui->micEnable_pushBtn->hide();
		ui->soundDisable_pushBtn->hide();
		ui->soundEnable_pushBtn->hide();
		ui->camera_pushBtn->hide();

		ui->textEdit_input->setObjectName("private_textEdit_input");
		ui->webView->setObjectName("private_webView");
		ui->chat_webView_background_widget->setObjectName("privateChat_webView_background_widget");
		ui->webView->setFixedHeight(220);
		ui->textEdit_input->setFixedHeight(86);

		break;
	case CLASS_CHAT:
		break;
	}
}

ChatWidget::~ChatWidget()
{
    if (m_fullWidget)
    {
        delete m_fullWidget;
        m_fullWidget = NULL;
    }
    delete ui;
}

void ChatWidget::sendChatMsgBtnClicked()
{
	if(!m_enableChat){
		return;
	}

    if (ui->textEdit_input->toPlainText().isEmpty())
    {
        showSysMsg(tr("can not input null"));
        return;
    }

    QString strHtml = ui->textEdit_input->toHtml();
    ui->textEdit_input->clear();
	emit sg_sendMsg(strHtml);
}

void ChatWidget::insertFace(QString strFacePath)
{
	m_faceSelectWindow.hide();
	if (strFacePath.isEmpty())
	{
		return;
	}
	QString strUrl = QString("<img src='%1'/>").arg(strFacePath);
	ui->textEdit_input->insertHtml(strUrl);
}

void ChatWidget::recvMsg(QVariant var)
{
    if (!m_isLoadFinished)
    {
        return;
    }
	Msg msg = var.value<Msg>();
	++m_msgNum;
	if(m_msgNum >= MAXMSGNUM)
	{
		clearMsg();
	}

	bool isSelf = false;
	if(msg._nSendUserId == msg._nUserId)
		isSelf = true;

	QString headPic = "qrc" + Env::currentThemeResPath();
	SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(msg._nSendUserId);
	if (userInfo.nUserAuthority == biz::UserAu_Teacher)
	{
		if (userInfo.nGender == biz::Gender_Male)
		{
			headPic =  headPic + QString("%1").arg("userListHeadIcon_0.png"); 
		}
		else
		{
			headPic =  headPic + QString("%1").arg("userListHeadIcon_2.png"); 
		}
	}
	else
	{
		if (userInfo.nGender == biz::Gender_Male)
		{
			headPic =  headPic + QString("%1").arg("userListHeadIcon_1.png"); 
		}
		else
		{
			headPic =  headPic + QString("%1").arg("userListHeadIcon_3.png"); 
		}
	}

	QString name;
	if(userInfo.szRealName == NULL || NULL == userInfo.szRealName[0])
	{
		name = QString::fromWCharArray(userInfo.szNickName);
	}
	else
	{
		name = QString::fromWCharArray(userInfo.szRealName);
	}

	if(!isSelf)
	{
		QString insertLeftMsg = QString("insertLeft('%1','%2','%3');").arg(name).arg(headPic).arg(msg.content);
		ui->webView->page()->mainFrame()->evaluateJavaScript(insertLeftMsg);
	}
	else
	{
		QString insertRightMsg = QString("insertRight('%1','%2','%3');").arg(name).arg(headPic).arg(msg.content);
		ui->webView->page()->mainFrame()->evaluateJavaScript(insertRightMsg);
	}
	refresh();
}

void ChatWidget::screenShotBtnClicked()
{
	QPixmap pixmap = m_fullWidget->getFullScreenPixmap();
	m_fullWidget->show();
	emit setPixmap(pixmap); //发送信号，使用当前的屏幕的图片作背景图片
}

void ChatWidget::showFaceSelectWindowBtnClicked()
{
	QSize size = m_faceSelectWindow.size();
	QPoint ptGlobal = ui->showFaceSelectWindow_pushBtn->mapToGlobal(QPoint(0, 0));
	ptGlobal.setY(ptGlobal.y() - size.height());
	ptGlobal.setX(ptGlobal.x() - (size.width() / 2 )+ ui->showFaceSelectWindow_pushBtn->size().width() / 2);
	m_faceSelectWindow.setGeometry(QRect(ptGlobal, size));
	m_faceSelectWindow.show();
	m_faceSelectWindow.selectFaceWindowBtnClicked();
	m_faceSelectWindow.activateWindow();
}

void ChatWidget::micEnableBtnClicked()
{	
	if(!ClassSeeion::GetInst()->_bBeginedClass)
	{
		return;
	}

	//m_micophoneSliderWidget.setSliderValue(CMediaPublishMgr::getInstance()->getMicVolume());
	changeShow(ui->micDisable_pushBtn,ui->micEnable_pushBtn,&m_micophoneSliderWidget);

	CMediaPublishMgr::getInstance()->setMicMute(true);
	biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::eClassAction_Close_voice,0);
}

void ChatWidget::micDisableBtnClicked()
{
	if(!ClassSeeion::GetInst()->_bBeginedClass)
	{
		return;
	}

	changeShow(ui->micEnable_pushBtn,ui->micDisable_pushBtn,&m_micophoneSliderWidget);
	
	CMediaPublishMgr::getInstance()->setMicMute(false);
	biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::eClassAction_Open_voice,0);	
}

void ChatWidget::soundEnableBtnClicked()
{
	if(!ClassSeeion::GetInst()->_bBeginedClass)
	{
		return;
	}

	//m_soundSliderWidget.setSliderValue(CMediaPublishMgr::getInstance()->getSpeakersVolume());
	changeShow(ui->soundDisable_pushBtn,ui->soundEnable_pushBtn,&m_soundSliderWidget);

	CMediaPublishMgr::getInstance()->setSpeakersMute(true);
}

void ChatWidget::soundDisableBtnClicked()
{
	if(!ClassSeeion::GetInst()->_bBeginedClass)
	{
		return;
	}

	changeShow(ui->soundEnable_pushBtn,ui->soundDisable_pushBtn,&m_soundSliderWidget);
	CMediaPublishMgr::getInstance()->setSpeakersMute(false);
}

void ChatWidget::cameraEnableBtnClicked()
{
	if(!ClassSeeion::GetInst()->_bBeginedClass)
	{
		return;
	}

	if(ClassSeeion::GetInst()->IsTeacher())
	{
		PublishSeatList& listPublish = CMediaPublishMgr::getInstance()->getPublishSeatList();
		for(int i=0;i<listPublish.size();i++)
		{
			LPPUBLISHSEATINFO seatPublish = listPublish.at(i);
            if(NULL == seatPublish || NULL == seatPublish->_rtmp)
            {
                continue;
            }

			if(seatPublish->_ctype != CAMERA_LOCAL)
			{
				continue;
			}
            
			int sourceType = ((CRTMPPublisher*)(seatPublish->_rtmp))->getSourceType();
			sourceType &=~SOURCECAMERA;
			((CRTMPPublisher*)(seatPublish->_rtmp))->setSourceType(sourceType);
			seatPublish->_rtmp->change();
		}
	}
	else
	{
        LPPUBLISHSEATINFO seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(0);
        if(seatPublish && seatPublish->_rtmp)
        {
            CRTMPPublisher* rtmpPublish = dynamic_cast<CRTMPPublisher*>(seatPublish->_rtmp);
            if(rtmpPublish)
            {
				int sourceType = rtmpPublish->getSourceType();
				sourceType &=~SOURCECAMERA;
				rtmpPublish->setSourceType(sourceType);
                rtmpPublish->change();
            }
        }
	}

	biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::eClassAction_Close_video,0);
	ui->cameraDisabel_pushBtn->show();
	ui->camera_pushBtn->hide();
}

void ChatWidget::cameraDisableBtnClicked()
{
	if(!ClassSeeion::GetInst()->_bBeginedClass)
	{
		return;
	}

	if(ClassSeeion::GetInst()->IsTeacher())
	{
		PublishSeatList& listPublish = CMediaPublishMgr::getInstance()->getPublishSeatList();
		for(int i=0;i<listPublish.size();i++)
		{
			LPPUBLISHSEATINFO seatPublish = listPublish.at(i);
            if(NULL == seatPublish || NULL == seatPublish->_rtmp)
            {
                continue;
            }

			if(seatPublish->_ctype != CAMERA_LOCAL)
			{
				continue;
			}

			int sourceType = ((CRTMPPublisher*)(seatPublish->_rtmp))->getSourceType();
			sourceType |= SOURCECAMERA;
			((CRTMPPublisher*)(seatPublish->_rtmp))->setSourceType(sourceType);
			seatPublish->_rtmp->change();
		}
	}
	else
	{
        LPPUBLISHSEATINFO seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(0);
        if(seatPublish && seatPublish->_rtmp)
        {
            CRTMPPublisher* rtmpPublish = dynamic_cast<CRTMPPublisher*>(seatPublish->_rtmp);
            if(rtmpPublish)
            {
				int sourceType = rtmpPublish->getSourceType();
				sourceType |= SOURCECAMERA;
				rtmpPublish->setSourceType(sourceType);
                rtmpPublish->change();
            }
        }
	}

	biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::eClassAction_Open_video,0);
	
	ui->camera_pushBtn->show();
	ui->cameraDisabel_pushBtn->hide();
}


void ChatWidget::changeShow(QPushButton *toShowBtn,QPushButton *toHideBtn,SliderWidget* sliderWidget)
{
	toHideBtn->hide();
	toShowBtn->show();
    sliderWidget->hide();

    this->killTimer(m_timerCheckSliderWidget);    
    m_timerCheckSliderWidget = 0;
/*
    QSize size = sliderWidget->size();
	QPoint ptGlobal = toShowBtn->mapToGlobal(QPoint(0, 0));
	ptGlobal.setY(ptGlobal.y() - size.height()-10);
	ptGlobal.setX(ptGlobal.x() - (size.width() / 2 )+ toShowBtn->size().width() / 2);
	sliderWidget->setGeometry(QRect(ptGlobal, size));
	sliderWidget->show();
	sliderWidget->raise();
*/
}

void ChatWidget::clearAll()
{
	QString JS = "clearAll()";
	ui->webView->page()->mainFrame()->evaluateJavaScript(JS);
}

void ChatWidget::clearMsg()
{
	QString JS = QString("clearMsg(%1)").arg(CLEARMSGNUM);
	m_msgNum = MAXMSGNUM - CLEARMSGNUM;
	ui->webView->page()->mainFrame()->evaluateJavaScript(JS);
}

void ChatWidget::getScreenShotPix(const QPixmap& pix)
{
	QTextDocumentFragment fragment; 
	QString picCachePath = Env::GetUserPicCachePath(ClassSeeion::GetInst()->_nUserId)+"tmp1.jpg";
	pix.save(picCachePath, "JPG", 100);
	QFile file(picCachePath);
	QString md5Path = ChatUpDownServer::getMd5Path(picCachePath);
	file.rename(md5Path);
	QString strUrl = QString("<img src=\"%1\"'/>").arg(md5Path);
	ui->textEdit_input->insertHtml(strUrl);
	ui->textEdit_input->activateWindow();
}

void ChatWidget::refresh()
{
	ui->webView->page()->mainFrame()->setHtml(ui->webView->page()->mainFrame()->toHtml());
	ui->webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical,ui->webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}

void ChatWidget::showBigPic(const QString &path)
{
    QString filePath = path;
    filePath = filePath.right(filePath.length() - QString("file:///").length());
    
	PictureViewer::getInstance()->openImage(filePath);
}

void ChatWidget::showPersonalInfo(const QVariant &path)
{
	qDebug()<<"showPersonalInfo"<<path.toString();
}

void ChatWidget::showSysMsg(const QString &msg)
{
	QString JS = QString("insertSysMsg('%1')").arg(msg);
	ui->webView->page()->mainFrame()->evaluateJavaScript(JS);
}

void ChatWidget::micophoneSliderChange(int value)
{
	ui->micEnable_pushBtn->show();
	ui->micDisable_pushBtn->hide();

	CMediaPublishMgr::getInstance()->setMicVolume(value);
}

void ChatWidget::soundSliderChange(int value)
{
	ui->soundEnable_pushBtn->show();
	ui->soundDisable_pushBtn->hide();

	CMediaPublishMgr::getInstance()->setSpeakersVolume(value);
}

void ChatWidget::open_closeSound()
{
	if(!ui->soundEnable_pushBtn->isHidden())
		soundEnableBtnClicked();
	else
		soundDisableBtnClicked();
}

void ChatWidget::open_closeMic()
{
	if(!ui->micEnable_pushBtn->isHidden())
		micEnableBtnClicked();
	else
		micDisableBtnClicked();
}

void ChatWidget::incressSound()
{
	soundSliderChange(m_soundSliderWidget.getSliderValue()+5);
}

void ChatWidget::decressSound()
{
	soundSliderChange(m_soundSliderWidget.getSliderValue()-5);
}

void ChatWidget::setEnbleChat(bool bISEnble)
{
	m_enableChat = bISEnble;
	if(m_isLoadFinished)
    {
		ui->send_pushBtn->setEnabled(bISEnble);
	}
}

void ChatWidget::resetUI()
{
	bool enable = ClassSeeion::GetInst()->_bBeginedClass;

	ui->camera_pushBtn->setEnabled(enable);
	ui->cameraDisabel_pushBtn->setEnabled(enable);
	ui->micEnable_pushBtn->setEnabled(enable);
	ui->micDisable_pushBtn->setEnabled(enable);
	ui->soundEnable_pushBtn->setEnabled(enable);
	ui->soundDisable_pushBtn->setEnabled(enable);
}

void ChatWidget::timerEvent(QTimerEvent * event)
{
    if(NULL==event)
    {
        return;
    }

    int timerId = event->timerId();
    if(timerId == m_timerCheckSliderWidget)
    {
        QPoint posCursor = QCursor::pos();

        if(m_soundSliderWidget.isVisible())
        {
            QRect rectSlider = getWidgetScreenRect(&m_soundSliderWidget);
            QRect rectBtn = getWidgetScreenRect(this->ui->soundEnable_pushBtn);

            bool ptIn = rectSlider.contains(posCursor) || rectBtn.contains(posCursor);
            if(!ptIn)
            {
                m_soundSliderWidget.hide();
                this->killTimer(m_timerCheckSliderWidget);
                this->activateWindow();
                m_timerCheckSliderWidget = 0;
            }

            return;
        }

        if(m_micophoneSliderWidget.isVisible())
        {
            QRect rectSlider = getWidgetScreenRect(&m_micophoneSliderWidget);
            QRect rectBtn = getWidgetScreenRect(this->ui->micEnable_pushBtn);
            
            bool ptIn = rectSlider.contains(posCursor) || rectBtn.contains(posCursor);
            if(!ptIn)
            {
                m_micophoneSliderWidget.hide();
                this->killTimer(m_timerCheckSliderWidget);
                this->activateWindow();
                m_timerCheckSliderWidget = 0;
            }
            return;
        }

        this->killTimer(m_timerCheckSliderWidget);
        m_timerCheckSliderWidget = 0;
    }
}

QRect ChatWidget::getWidgetScreenRect(QWidget* widget)
{
    QRect rectWidget;
    if(NULL == widget)
    {
        return rectWidget;
    }

    rectWidget = widget->rect();
    QPoint ptPos(rectWidget.x(),rectWidget.y());
    ptPos = widget->mapToGlobal(ptPos);

    rectWidget.moveTo(ptPos);
    return rectWidget;
}