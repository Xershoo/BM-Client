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

const int MAXMSGNUM = 10;	//每页显示的最多信息
const int CLEARMSGNUM = 5;	//删除最前的N条消息

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT
signals:

	void setPixmap(QPixmap pixmap); //设置pixmap信息
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
	
	void resetUI();
    
    bool eventFilter(QObject *o, QEvent *e);
    void loadChatStyle();
    bool isLoadFinished() {return m_isLoadFinished;}
public slots:

	void showBigPic(const QString &path);
	void showPersonalInfo(const QVariant &path);
    void screenShotBtnClicked();
    void open_closeSound();
    void open_closeMic();
    void incressSound();
    void decressSound();
    void loadChatStyleFinished(bool);
protected slots:

	void showFaceSelectWindowBtnClicked();	

	void micEnableBtnClicked();
	void micDisableBtnClicked();
	void soundEnableBtnClicked();
	void soundDisableBtnClicked();

	void cameraEnableBtnClicked();
	void cameraDisableBtnClicked();

	void insertFace(QString strFacePath);
	void recvMsg(QVariant var);

	void getScreenShotPix(const QPixmap& pix);
	void refresh();

	void micophoneSliderChange(int value);
	void soundSliderChange(int value);
    void addJavaScriptObject();

    QRect getWidgetScreenRect(QWidget* widget);
protected:
    void timerEvent(QTimerEvent * event);
private:

    Ui::ChatWidget *ui;
	fullScreenWidget *m_fullWidget;
	FaceSelectWindow m_faceSelectWindow;
	QPixmap m_fullPixmap;
	int m_msgNum;
    SliderWidget m_micophoneSliderWidget;
    SliderWidget m_soundSliderWidget;

    bool m_isLoadFinished;
	bool m_enableChat;

    int  m_timerCheckSliderWidget;
private:
	void changeShow(QPushButton *toShowBtn,QPushButton *toHideBtn,SliderWidget* sliderWidget);
	void clearAll();
	void clearMsg();

};

#endif // CHATWIDGET_H
