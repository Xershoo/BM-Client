#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "faceselectwindow.h"
#include <QPixmap>
#include "biz/BizInterface.h"
#include <QList>
#include <QString>
#include <QVariant>
#include <QWidget>
#include "src/soundMicophone/sliderwidget.h"

class fullScreenWidget;

enum ChatType{
	PRIVATE_CHAT = 1,
	CLASS_CHAT
};

namespace Ui {
class ChatWidget;
}

typedef struct _chat_msg 
{
	QString content;
	__int64 _nSendUserId;
	__int64 _nUserId;
}CHAT_MSG,*LPCHATMSG;

typedef QList<CHAT_MSG>   QChatMsgList;
class ChatWidget : public QWidget
{
    Q_OBJECT
signals:

	void setPixmap(QPixmap pixmap); //…Ë÷√pixmap–≈œ¢
	void sg_sendMsg(QString msg);
    void sg_loadChatStyleFinished();

public slots:
	void sendChatMsgBtnClicked();

public:

    explicit ChatWidget(QWidget *parent = 0);
    ~ChatWidget();
	void showSysMsg(const QString &msg);
	void setChatType(ChatType chatType);
    void setEnbleChat(bool bISEnble);
	void recvMsg(CHAT_MSG msg);
	void resetUI();
    
    bool eventFilter(QObject *o, QEvent *e);
    bool isLoadFinished() {return m_isLoadFinished;}
public slots:

	void showBigPic(const QString &path);
	void showPersonalInfo(const QVariant &path);
    void screenShotBtnClicked();
    void open_closeSound();
    void open_closeMic();
    void incressSound();
    void decressSound();
    void loadChatStyleFinished();
protected slots:

	void showFaceSelectWindowBtnClicked();	

	void micEnableBtnClicked();
	void micDisableBtnClicked();
	void soundEnableBtnClicked();
	void soundDisableBtnClicked();

	void cameraEnableBtnClicked();
	void cameraDisableBtnClicked();

	void insertFace(QString strFacePath);
	void getScreenShotPix(const QPixmap& pix);
	void micophoneSliderChange(int value);
	void soundSliderChange(int value);

    QRect getWidgetScreenRect(QWidget* widget);
protected:
    void timerEvent(QTimerEvent * event);
private:

    Ui::ChatWidget *ui;
	fullScreenWidget *m_fullWidget;
	FaceSelectWindow m_faceSelectWindow;
	QPixmap m_fullPixmap;
    SliderWidget m_micophoneSliderWidget;
    SliderWidget m_soundSliderWidget;

    bool m_isLoadFinished;
	bool m_enableChat;

    int  m_timerCheckSliderWidget;
private:
	void changeShow(QPushButton *toShowBtn,QPushButton *toHideBtn,SliderWidget* sliderWidget);
	void clearAll();
};

#endif // CHATWIDGET_H
