#ifndef WEBCAMERASETTINGPAGE_H
#define WEBCAMERASETTINGPAGE_H

#include <QWidget>
#include "ui_webcamerasettingpage.h"
#include "control/c8commonwindow.h"
#include "biz/BizInterface.h"

class MulitCameraListItemWidget;

class WebCameraSettingPage : public C8CommonWindow
{
    Q_OBJECT

public:
    WebCameraSettingPage(QWidget *parent = 0);
    ~WebCameraSettingPage();
    
protected:
    virtual void setTitleBarRect() override;

protected slots:
    void closeBtnClicked();
	void posOneBtnClicked();
	void posTwoBtnClicked();
	void posThrBtnClicked();

	void vdsOneBtnClicked();
	void vdsTwoBtnClicked();

	void cameraListItemClicked(MulitCameraListItemWidget * itemCamera);
    void QRCodeClicked();

	void onRecvDevCodeMsg(__int64 roomId);
	void onRecvNetCameraAccess(NetDevAccessInfo info);
	void onRecvNetCameraDropp(MobileNotifyInfo info);
	void onRecvSelNetCameraResp(ChooseMobileInfo info);

protected:
	bool updatePublishSeatInfo();
	void addCameraItem(int id,int type,const string& name);
	bool selPublishSeat(int nIndex);
	void changeSeatVideoScale(int scale);

	MulitCameraListItemWidget *findListItem(int id, int type,int& nIndex);
	MulitCameraListItemWidget *findListItem(int seat,int& nIndex);

	virtual void timerEvent(QTimerEvent *event);
private:
    Ui::WebCameraSettingPage ui;

	bool  m_updateSeatInfo;
	int   m_idUpdateTimer;

	int   m_camIdUnset;
	int	  m_camTypeUnset;
	int   m_seatIdUnset;
};

#endif // WEBCAMERASETTINGPAGE_H
