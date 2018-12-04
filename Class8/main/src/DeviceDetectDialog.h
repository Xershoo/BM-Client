#ifndef DEVICE_DETECT_DIALOG_H
#define DEVICE_DETECT_DIALOG_H

#include "./src/control/c8commonwindow.h"
#include "ui_DeviceDetectDialog.h"
#include "./userinfo/userinfomanager.h"
#include "./multimedia/AudioDeviceDetect.h"
#include <QtCore/QThread>
#include <QtGui\QMovie>

class QNetworkDetectThread : public QThread
{
	Q_OBJECT

public:
	enum{
		NETWORK_STATE_OK = 0,
		SERVER_NOT_CONNECTED,
		CLASS_SERVER_FAILED,
		MEDIA_SERVER_FAILED,
		WEB_SERVER_FAILED
	};

public:
	QNetworkDetectThread();
	virtual ~QNetworkDetectThread();

	int result();
	void stop();

protected:
	virtual void run();

	bool connectServer(const QString& servHost,qint16 servPort);
signals:
	void detectFinished(int);
protected:
	int		m_retDetect;
	bool	m_exit;
};

class DeviceDetectDialog : public C8CommonWindow
{
	Q_OBJECT
public:
	DeviceDetectDialog(__int64 uid,bool detectManual=true,QWidget * parent=NULL);
	~DeviceDetectDialog();

public:
	bool isNeedDetect();
	
	void saveSpeakerDeviceDetect();
	void saveCameraDeviceDetect();
	void saveMicrophoneDeviceDetect();

	void saveDeviceDetect();
protected:
	void setTitleBarRect();
	bool isDeviceChange();
	bool compareDeviceList(QVector<QString>&,QVector<QString>&);
	void initUIDeviceList();

	void enableMicDetect(bool);
	void enableSpkDetect(bool);
	void enableCameraDetect(bool);
	void enableNetworkDetect(bool);
protected:
	virtual void timerEvent(QTimerEvent *event);

protected slots:
	void showSpeakerDetect();
	void showCameraDetect();
	void showMicDetect();
	void showNetworkDetect();
	void endDeviceDetect();
	void showHelpMsgDlg();

	void doClose();

	void onMeidaInited();

	void selSpkChanged(const QString&);
	void selCameraChanged(int);
	void selMicChanged(const QString&);

	void networkDetectFinished(int);
protected:
	Ui::DeviceDetectDialog ui;

	__int64		m_uid;
	UserInfoManager* m_mgrUserInfo;
	AudioDeviceDetect m_detectAudioDevice;

	int   m_refreshSpkVolumeTimer;
	int	  m_refreshMicVolumeTimer;

	bool  m_detectManual;
	
	QNetworkDetectThread	m_threadNetworkDetect;
	QMovie*		m_gifNetworkDetect;
};

#endif

