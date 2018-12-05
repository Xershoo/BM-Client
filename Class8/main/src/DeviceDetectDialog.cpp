#include "DeviceDetectDialog.h"
#include "./media/MediaPublishMgr.h"
#include "./userinfo/userinfomanager.h"
#include "./common/macros.h"
#include "./common/Util.h"
#include "C8MessageBox.h"
#include <QtCore\QTimerEvent>
#include <QtNetwork\QTcpSocket>
#include "./common/Config.h"
#include "./common/ping.h"
#include "DeviceDetectHelpDialog.h"

//////////////////////////////////////////////////////////////////////////
QNetworkDetectThread::QNetworkDetectThread():QThread(NULL),
	m_retDetect(NETWORK_STATE_OK),
	m_exit(false)
{

}

QNetworkDetectThread::~QNetworkDetectThread()
{

}

int QNetworkDetectThread::result()
{
	return m_retDetect;
}

void QNetworkDetectThread::run()
{
	int step = 0;
	QString qstrUrlSite;
	QString qstrClassServHost;
	qint16	nClassServPort;

	Config* appConfig = Config::getConfig();
	
	if(NULL!=appConfig){
		string strUrlSite = Config::getConfig()->m_urlWebSite;
		string strClassServHost = Config::getConfig()->m_loginServIp;
		string strClassServPort = Config::getConfig()->m_loginServPort;

		QString qstrTemp = QString::fromStdString(strUrlSite);
		qstrTemp = qstrTemp.right(qstrTemp.length() - QString("http://").length());

		qstrUrlSite = qstrTemp.left(qstrTemp.length()-1);
		qstrClassServHost = QString::fromStdString(strClassServHost);
		nClassServPort = QString::fromStdString(strClassServPort).toInt();
	}else{
		qstrUrlSite = QString("121.43.33.4");
		qstrClassServHost = QString("121.43.33.4");
		nClassServPort = 7070;
	}
	
	while(!m_exit&&step<=3){
		switch(step)
		{
		case 0:			// ping server
			{
				CPing servPing;
				if(!servPing.ping(qstrUrlSite.toStdString().c_str())){
					m_retDetect = SERVER_NOT_CONNECTED;
				}
			}
			break;
		case 1:			// connect class server
			if(!connectServer(qstrClassServHost,nClassServPort)){
				m_retDetect = CLASS_SERVER_FAILED;
			}
			break;
		case 2:			// connect media server
			if(!connectServer(qstrUrlSite,1935)){
				m_retDetect = MEDIA_SERVER_FAILED;
			}
			break;
		case 3:			// connect web server
			if(!connectServer(qstrUrlSite,80)){
				m_retDetect = WEB_SERVER_FAILED;
			}
			break;
		default:
			break;
		}

		if(m_retDetect != NETWORK_STATE_OK){
			break;
		}

		step++;

		msleep(500);
	}

	if(!m_exit){
		emit detectFinished(m_retDetect);
	}
}

bool QNetworkDetectThread::connectServer(const QString& servHost,qint16 servPort)
{
	if(servHost.length() <=0){
		return false;
	}

	bool ret = false;
	QTcpSocket tcpSocket(0);
	tcpSocket.connectToHost(servHost,servPort);

	if(tcpSocket.waitForConnected(3000)){
		ret = true;
	}

	tcpSocket.close();
	return ret;
}

void QNetworkDetectThread::stop()
{
	m_exit=true;

	this->wait();
	this->quit();
}
//////////////////////////////////////////////////////////////////////////
DeviceDetectDialog::DeviceDetectDialog(__int64 uid,bool detectManual/* =true */,QWidget * parent/* =NULL */) :C8CommonWindow(parent,SHADOW_NO),
	m_uid(uid),
	m_mgrUserInfo(NULL),
	m_detectAudioDevice(NULL),
	m_refreshMicVolumeTimer(0),
	m_refreshSpkVolumeTimer(0),
	m_detectManual(detectManual),
	m_gifNetworkDetect(NULL)
{
	ui.setupUi(this);

	connect(ui.pushButton_close,SIGNAL(clicked()),this,SLOT(doClose()));
	connect(ui.pushButton_jumpDeviceDetect,SIGNAL(clicked()),this,SLOT(endDeviceDetect()));
	connect(ui.pushButton_goDeviceDetect,SIGNAL(clicked()),this,SLOT(showSpeakerDetect()));
	connect(ui.pushButton_spkHear,SIGNAL(clicked()),this,SLOT(showCameraDetect()));
	connect(ui.pushButton_cameraPreview,SIGNAL(clicked()),this,SLOT(showMicDetect()));
	connect(ui.pushButton_micHear,SIGNAL(clicked()),this,SLOT(showNetworkDetect()));
	connect(ui.pushButton_endDetect,SIGNAL(clicked()),this,SLOT(endDeviceDetect()));
	connect(ui.pushButton_spkNoHear,SIGNAL(clicked()),this,SLOT(showHelpMsgDlg()));
	connect(ui.pushButton_micNoHear,SIGNAL(clicked()),this,SLOT(showHelpMsgDlg()));
	connect(ui.pushButton_cameraNoPreview,SIGNAL(clicked()),this,SLOT(showHelpMsgDlg()));
	
	ui.pushButton_jumpDeviceDetect->hide();
	ui.widget_pagesDeviceDetect->setCurrentIndex(0);
	ui.progressBar_spkVolume->setRange(0,100);
	ui.progressBar_micVolume->setRange(0,100);

	ui.progressBar_spkVolume->setValue(10);
	ui.progressBar_micVolume->setValue(10);
	
	m_gifNetworkDetect = new QMovie(QString(":/res/res/image/default/network_detect.gif"));
	m_gifNetworkDetect->setCacheMode(QMovie::CacheAll);
	ui.label_gifNetworkDetect->setMovie(m_gifNetworkDetect);
	
	m_mgrUserInfo = new UserInfoManager(uid);
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(mgrMedia){
		if(!mgrMedia->isInitMedia()){
			connect(mgrMedia,SIGNAL(initFinished()),this,SLOT(onMeidaInited()));
			ui.pushButton_goDeviceDetect->setEnabled(false);
		}
		else{
			initUIDeviceList();
		}
	}

	CMediaPublishMgr::getInstance()->setPlayVideo(true);
	C8CommonWindow::centerWindow();
}

DeviceDetectDialog::~DeviceDetectDialog()
{
	enableSpkDetect(false);
	enableCameraDetect(false);
	enableMicDetect(false);
	enableNetworkDetect(false);

	SAFE_DELETE(m_mgrUserInfo);
	SAFE_DELETE(m_gifNetworkDetect);
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(mgrMedia){
		disconnect(mgrMedia,NULL,this,NULL);
		mgrMedia->setPlayVideo(false);
	}
}

void DeviceDetectDialog::setTitleBarRect()
{
	QPoint pt = ui.widget_titleDeviceDetect->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.widget_titleDeviceDetect->size());
}

void DeviceDetectDialog::doClose()
{
	if(m_detectManual){
		saveDeviceDetect();
		C8CommonWindow::accept();
		return;
	}

	int curDetect = ui.widget_pagesDeviceDetect->currentIndex();
	if(curDetect==4){
		if(ui.pushButton_endDetect->isEnabled()){
			endDeviceDetect();
			return;
		}
	}
	
	if(curDetect==1){
		if(ui.comboBox_speaker->count()<=0){
			saveDeviceDetect();
			C8CommonWindow::reject();
			return;
		}	
	}
	
	C8MessageBox msgBox(C8MessageBox::Question, QString(tr("errorTip")), QString(tr("closeMsg")));
	if(msgBox.exec() == C8MessageBox::Accepted){
		saveDeviceDetect();
		C8CommonWindow::accept();
		return;
	}else{

	}

	return;
}

void DeviceDetectDialog::showSpeakerDetect()
{
	ui.widget_pagesDeviceDetect->setCurrentIndex(1);

	//没有找到对应的声音设备
	if(ui.comboBox_speaker->count()<=0){
		C8MessageBox msgBox(C8MessageBox::Question, QString(tr("errorTip")), QString(tr("noSpeakerFind")));
		if(msgBox.exec()==C8MessageBox::Accepted){
			C8CommonWindow::reject();
			return;
		}

		ui.label_msgSpeakerDetect->setText(tr("msgNoSpeaker"));
		ui.pushButton_spkHear->setEnabled(false);
		ui.pushButton_spkNoHear->setEnabled(false);
	}

	enableSpkDetect(true);
}

void DeviceDetectDialog::showCameraDetect()
{
	enableSpkDetect(false);
	saveSpeakerDeviceDetect();

	CMediaPublishMgr::getInstance()->setSelSpeaker(ui.comboBox_speaker->currentIndex());
	ui.widget_pagesDeviceDetect->setCurrentIndex(2);

	if(ui.comboBox_camera->count()<=0){
		ui.label_msgCameraDetect->setText(tr("msgNoCamera"));
		ui.pushButton_cameraPreview->setEnabled(false);
		ui.pushButton_cameraNoPreview->setEnabled(true);
	}

	enableCameraDetect(true);
}

void DeviceDetectDialog::showMicDetect()
{
	saveCameraDeviceDetect();
	enableCameraDetect(false);
	
	CMediaPublishMgr::getInstance()->setSelCamera(ui.comboBox_camera->currentIndex());

	ui.widget_pagesDeviceDetect->setCurrentIndex(3);
	
	if(ui.comboBox_microphone->count()<=0){
		ui.label_msgMicDetect->setText(tr("msgNoMicrophone"));
		ui.pushButton_micHear->setEnabled(false);
		ui.pushButton_micNoHear->setEnabled(true);
	}
	enableMicDetect(true);
}

void DeviceDetectDialog::showNetworkDetect()
{
	saveMicrophoneDeviceDetect();
	enableMicDetect(false);
	CMediaPublishMgr::getInstance()->setSelMic(ui.comboBox_microphone->currentIndex());

	ui.widget_pagesDeviceDetect->setCurrentIndex(4);
	enableNetworkDetect(true);

	ui.pushButton_endDetect->setEnabled(false);
}

void DeviceDetectDialog::endDeviceDetect()
{
	if(m_mgrUserInfo){
		m_mgrUserInfo->setDeviceDetect(true);
	}

	C8CommonWindow::accept();
}

void DeviceDetectDialog::showHelpMsgDlg()
{
	DeviceDetectHelpDialog helpDlg;
	int ret = helpDlg.exec();

	if(ret == QDialog::Rejected){
		saveDeviceDetect();
		C8CommonWindow::reject();
		return;
	}

	int curIndex = ui.widget_pagesDeviceDetect->currentIndex();
	switch(curIndex)
	{
	case 1:
		showCameraDetect();
		break;
	case 2:
		showMicDetect();
		break;
	case 3:
		showNetworkDetect();
		break;
	}
}

bool DeviceDetectDialog::isNeedDetect()
{
	if(!CMediaPublishMgr::getInstance()->isInitMedia()){
		return true;
	}

	if(NULL==m_mgrUserInfo){
		return true;
	}
 
 	bool br = m_mgrUserInfo->isDeviceDetect();
 	if(!br){
 		return true;
 	}

	if(isDeviceChange()){
		m_mgrUserInfo->setDeviceDetect(false);
		return true;
	}
	
	return false;
}

bool DeviceDetectDialog::isDeviceChange()
{	
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL==mgrMedia){
		return false;
	}

	if(NULL==m_mgrUserInfo){
		return true;
	}

	MediaDeviceList devList = mgrMedia->getMediaDeviceList();
	if(devList.size()<=0){
		return true;
	}

	QVector<QString> spkListLocal;
	QVector<QString> micListLocal;
	QVector<QString> cameraListLocal;

	for(int i=0;i<devList.size();i++){
		QString deviceName;
		Util::AnsiToQString(devList[i]->_name.c_str(),devList[i]->_name.length(),deviceName);

		switch(devList[i]->_type)
		{
		case SPEAKER_LOCAL:
			spkListLocal.push_back(deviceName);
			break;
		case CAMERA_LOCAL:
			cameraListLocal.push_back(deviceName);
			break;
		case MICROPHONE_LOCAL:
			micListLocal.push_back(deviceName);
			break;
		}
	}

	QVector<QString> spkListSave;
	QVector<QString> micListSave;
	QVector<QString> cameraListSave;
	
	m_mgrUserInfo->getSpeakerList(spkListSave);
	m_mgrUserInfo->getMicList(micListSave);
	m_mgrUserInfo->getCameraList(cameraListSave);

	if (compareDeviceList(spkListLocal,spkListSave)||
		compareDeviceList(micListLocal,micListSave)||
		compareDeviceList(cameraListLocal,cameraListSave)){
			return true;
	}

	return false;
}

bool DeviceDetectDialog::compareDeviceList(QVector<QString>& vecDeviceA,QVector<QString>& vecDeviceB)
{
	if(vecDeviceA.size() != vecDeviceB.size()){
		return 1;
	}

	int m=0,n=0;

	for(m=0;m<vecDeviceA.size();m++){
		QString deviceA=vecDeviceA.at(m);
		for(n=0;n<vecDeviceB.size();n++){
			QString deviceB=vecDeviceB.at(n);
			if(deviceA.compare(deviceB) == 0){
				break;
			}
		}

		if(n>=vecDeviceB.size()){
			return 1;
		}
	}

	return 0;
}

void DeviceDetectDialog::onMeidaInited()
{
	ui.pushButton_goDeviceDetect->setEnabled(true);

	if(!isNeedDetect()) {
		ui.pushButton_jumpDeviceDetect->show();
	}

	initUIDeviceList();
}

void DeviceDetectDialog::initUIDeviceList()
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL==mgrMedia){
		return;
	}

	MediaDeviceList devList = mgrMedia->getMediaDeviceList();
	if(devList.size()<=0){
		return;
	}

	int selSpk = 0, selCamera = 0, selMic = 0;
	QString selSpkName,selCameraName,selMicName;

	if(m_mgrUserInfo){
		selSpk = m_mgrUserInfo->getSelSpeaker();
		selCamera = m_mgrUserInfo->getSelCamera();
		selMic = m_mgrUserInfo->getSelMic();

		QVector<QString> spkListSave;
		QVector<QString> micListSave;
		QVector<QString> cameraListSave;

		m_mgrUserInfo->getSpeakerList(spkListSave);
		m_mgrUserInfo->getMicList(micListSave);
		m_mgrUserInfo->getCameraList(cameraListSave);

		if(selSpk>=0){
			selSpkName = spkListSave.at(selSpk);
		}
		if(selMic>=0) {
			selMicName = micListSave.at(selMic);
		}
		if(selCamera>=0){
			selCameraName = cameraListSave.at(selCamera);
		}

		selSpk = 0;
		selCamera = 0;
		selMic = 0;

	}else{
		selSpk = mgrMedia->getSelSpeaker();
		selCamera = mgrMedia->getSelCamera();
		selMic = mgrMedia->getSelMic();
	}
	
	for(int i=0;i<devList.size();i++){
		QString deviceName;
		Util::AnsiToQString(devList[i]->_name.c_str(),devList[i]->_name.length(),deviceName);

		switch(devList[i]->_type)
		{
		case SPEAKER_LOCAL:
			ui.comboBox_speaker->addItem(deviceName);
			if(deviceName.compare(selSpkName) == 0){
				selSpk = ui.comboBox_speaker->count()-1;
			}

			break;
		case CAMERA_LOCAL:
			ui.comboBox_camera->addItem(deviceName);
			if(deviceName.compare(selSpkName) == 0){
				selCamera = ui.comboBox_camera->count()-1;
			}
			break;
		case MICROPHONE_LOCAL:
			ui.comboBox_microphone->addItem(deviceName);
			if(deviceName.compare(selSpkName) == 0){
				selMic = ui.comboBox_microphone->count()-1;
			}
			break;
		}
	}

	ui.comboBox_speaker->setCurrentIndex(selSpk);
	ui.comboBox_camera->setCurrentIndex(selCamera);
	ui.comboBox_microphone->setCurrentIndex(selMic);

	connect(ui.comboBox_speaker,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(selSpkChanged(const QString&)));
	connect(ui.comboBox_camera,SIGNAL(currentIndexChanged(int)),this,SLOT(selCameraChanged(int)));
	connect(ui.comboBox_microphone,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(selMicChanged(const QString&)));
}

void DeviceDetectDialog::saveSpeakerDeviceDetect()
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL==m_mgrUserInfo){
		return;
	}

	MediaDeviceList devList = mgrMedia->getMediaDeviceList();
	if(devList.size()<=0){
		return ;
	}

	int curSel = ui.comboBox_speaker->currentIndex();
	QVector<QString> deviceNameList;
	
	for(int i=0;i<devList.size();i++){
		QString deviceName;
		Util::AnsiToQString(devList[i]->_name.c_str(),devList[i]->_name.length(),deviceName);

		switch(devList[i]->_type)
		{
		case SPEAKER_LOCAL:
			deviceNameList.push_back(deviceName);
			break;
		default:
			break;
		}
	}

	m_mgrUserInfo->updateSpeakerList(curSel,deviceNameList);
}

void DeviceDetectDialog::saveCameraDeviceDetect()
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL==m_mgrUserInfo){
		return;
	}

	MediaDeviceList devList = mgrMedia->getMediaDeviceList();
	if(devList.size()<=0){
		return ;
	}

	int curSel = ui.comboBox_camera->currentIndex();
	QVector<QString> deviceNameList;

	for(int i=0;i<devList.size();i++){
		QString deviceName;
		Util::AnsiToQString(devList[i]->_name.c_str(),devList[i]->_name.length(),deviceName);

		switch(devList[i]->_type)
		{
		case CAMERA_LOCAL:
			deviceNameList.push_back(deviceName);
			break;
		default:
			break;
		}
	}

	m_mgrUserInfo->updateCameraList(curSel,deviceNameList);
}

void DeviceDetectDialog::saveMicrophoneDeviceDetect()
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL==m_mgrUserInfo){
		return;
	}

	MediaDeviceList devList = mgrMedia->getMediaDeviceList();
	if(devList.size()<=0){
		return ;
	}

	int curSel = ui.comboBox_microphone->currentIndex();
	QVector<QString> deviceNameList;

	for(int i=0;i<devList.size();i++){
		QString deviceName;
		Util::AnsiToQString(devList[i]->_name.c_str(),devList[i]->_name.length(),deviceName);

		switch(devList[i]->_type)
		{
		case MICROPHONE_LOCAL:
			deviceNameList.push_back(deviceName);
			break;
		default:
			break;
		}
	}

	m_mgrUserInfo->updateMicList(curSel,deviceNameList);
}

void DeviceDetectDialog::saveDeviceDetect()
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL==mgrMedia){
		return;
	}

	if(NULL==m_mgrUserInfo){
		return;
	}

	MediaDeviceList devList = mgrMedia->getMediaDeviceList();
	if(devList.size()<=0){
		return;
	}

	QVector<QString> spkListLocal;
	QVector<QString> micListLocal;
	QVector<QString> cameraListLocal;

	for(int i=0;i<devList.size();i++){
		QString deviceName;
		Util::AnsiToQString(devList[i]->_name.c_str(),devList[i]->_name.length(),deviceName);

		switch(devList[i]->_type)
		{
		case SPEAKER_LOCAL:
			spkListLocal.push_back(deviceName);
			break;
		case CAMERA_LOCAL:
			cameraListLocal.push_back(deviceName);
			break;
		case MICROPHONE_LOCAL:
			micListLocal.push_back(deviceName);
			break;
		}
	}

	int selSpeaker = ui.comboBox_speaker->currentIndex();
	int selCamera = ui.comboBox_camera->currentIndex();
	int selMicrophone = ui.comboBox_microphone->currentIndex();

	m_mgrUserInfo->updateSpeakerList(selSpeaker,spkListLocal);
	m_mgrUserInfo->updateMicList(selMicrophone ,micListLocal);
	m_mgrUserInfo->updateCameraList(selCamera,	cameraListLocal);
}

void DeviceDetectDialog::timerEvent(QTimerEvent *event)
{
	if(NULL==event){
		return;
	}

	int tid = event->timerId();
	if(tid==m_refreshSpkVolumeTimer){
		int valume= m_detectAudioDevice.getAudioOutDeviceVolume();
		ui.progressBar_spkVolume->setValue(valume);
		return;
	}

	if(tid==m_refreshMicVolumeTimer){
		//int valume= CMediaPublishMgr::getInstance()->GetMicRTVolum();
		int valume= m_detectAudioDevice.getAudioInDeviceVolume();
		ui.progressBar_micVolume->setValue(valume);
		return;
	}
}

void DeviceDetectDialog::selSpkChanged(const QString& spkName)
{
	if(ui.widget_pagesDeviceDetect->currentIndex() != 1){
		return;
	}

	enableSpkDetect(true);
}

void DeviceDetectDialog::selCameraChanged(int curIndex)
{
	if(ui.widget_pagesDeviceDetect->currentIndex() != 2){
		return;
	}

	enableCameraDetect(true);
}

void DeviceDetectDialog::selMicChanged(const QString& devName)
{
	if(ui.widget_pagesDeviceDetect->currentIndex() != 3){
		return;
	}

	enableMicDetect(true);
}

void DeviceDetectDialog::enableMicDetect(bool enable)
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL == mgrMedia)
	{
		return ;
	}

	if(!mgrMedia->isInitMedia())
	{
		return ;
	}

	if(ui.comboBox_microphone->count()<=0){
		return;
	}

	int mic = ui.comboBox_microphone->currentIndex();
	if(mic < 0)
	{
		return;
	}

	/*
	mgrMedia->PreviewSound(mic,enable);
	if(enable)
	{
		if(m_refreshMicVolumeTimer <= 0)
		{
			m_refreshMicVolumeTimer = startTimer(200);
		}
	}
	else
	{
		killTimer(m_refreshMicVolumeTimer);
		m_refreshMicVolumeTimer = 0;
	}
	*/

	if(enable)
	{
		QString inAudioName = ui.comboBox_microphone->currentText();
		QString outAudioName = ui.comboBox_speaker->currentText();

		m_detectAudioDevice.startDetectAudioInDevice(inAudioName,outAudioName);

		if(m_refreshMicVolumeTimer <=0){
			m_refreshMicVolumeTimer = this->startTimer(250);
		}
	}
	else
	{
		m_detectAudioDevice.stopDetectAudioInDevice();

		if(m_refreshMicVolumeTimer>0){
			killTimer(m_refreshMicVolumeTimer);
			m_refreshMicVolumeTimer = 0;
		}
	}
}

void DeviceDetectDialog::enableCameraDetect(bool enable)
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL == mgrMedia)
	{
		return ;
	}

	if(!mgrMedia->isInitMedia())
	{
		return ;
	}

	if(ui.comboBox_camera->count()<=0){
		return;
	}


	if(enable){
		int mic = ui.comboBox_camera->currentIndex();
		if(mic < 0)
		{
			return;
		}

		ui.widget_cameraPreview->setVideoIndex(ui.comboBox_camera->currentIndex());
		ui.widget_cameraPreview->setDrawVideoRect(true);
	}
	else{
		ui.widget_cameraPreview->closeVideo();
	}
}

void DeviceDetectDialog::enableSpkDetect(bool enable)
{
	if(ui.comboBox_speaker->count()<=0){
		return;
	}

	int mic = ui.comboBox_speaker->currentIndex();
	if(mic < 0)
	{
		return;
	}

	if(enable)
	{
		QString outAudioName = ui.comboBox_speaker->currentText();
		m_detectAudioDevice.startDetectAudioOutDevice(outAudioName);

		if(m_refreshSpkVolumeTimer <=0){
			m_refreshSpkVolumeTimer = this->startTimer(250);
		}
	}
	else
	{
		m_detectAudioDevice.stopDetectAudioOutDevice();

		if(m_refreshSpkVolumeTimer>0){
			killTimer(m_refreshSpkVolumeTimer);
			m_refreshSpkVolumeTimer = 0;
		}
	}
}

void DeviceDetectDialog::enableNetworkDetect(bool enable)
{
	if(enable){
		if(m_threadNetworkDetect.isRunning()){
			m_threadNetworkDetect.stop();
		}

		m_threadNetworkDetect.start();
		if(m_gifNetworkDetect){
			m_gifNetworkDetect->setPaused(false);
		}

		connect(&m_threadNetworkDetect,SIGNAL(detectFinished(int)),this,SLOT(networkDetectFinished(int)));

	}else{
		if(m_threadNetworkDetect.isRunning()){
			m_threadNetworkDetect.stop();
		}

		if(m_gifNetworkDetect){
			m_gifNetworkDetect->setPaused(true);
		}

		disconnect(&m_threadNetworkDetect,NULL,this,NULL);
	}
}

void DeviceDetectDialog::networkDetectFinished(int ret)
{
	if(m_gifNetworkDetect){
		m_gifNetworkDetect->setPaused(true);
		m_gifNetworkDetect->jumpToFrame(m_gifNetworkDetect->frameCount()-1);
	}

	switch(ret)
	{
	case QNetworkDetectThread::NETWORK_STATE_OK:
		ui.pushButton_endDetect->setEnabled(true);
		ui.label_tipNetworkDetect->setText(tr("networkDetectOK"));
		break;
	case QNetworkDetectThread::SERVER_NOT_CONNECTED:
	case QNetworkDetectThread::CLASS_SERVER_FAILED:
	case QNetworkDetectThread::MEDIA_SERVER_FAILED:
	case QNetworkDetectThread::WEB_SERVER_FAILED:
		ui.pushButton_endDetect->setEnabled(true);
		ui.label_tipNetworkDetect->setText(tr("networkDetectNoOK"));
		break;
	}
}