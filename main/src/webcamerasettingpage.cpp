#include "webcamerasettingpage.h"
#include "control/mulitcameralistitemwidget.h"
#include "media/MediaPublishMgr.h"
#include "session/classsession.h"
#include "util.h"
#include "common/quickmark/QR_Encode.h"

WebCameraSettingPage::WebCameraSettingPage(QWidget *parent)
    : C8CommonWindow(parent)
	, m_camIdUnset(-1)
	, m_camTypeUnset(-1)
	, m_seatIdUnset(-1)
{
    ui.setupUi(this);
    ui.editCameraCode->setEnabled(false);
	if(!updatePublishSeatInfo())
	{
		m_idUpdateTimer = startTimer(1000);

		ui.pushButton_pos1->setEnabled(false);
		ui.pushButton_pos2->setEnabled(false);
		ui.pushButton_pos3->setEnabled(false);
		ui.radioButton_4_3->setEnabled(false);
		ui.radioButton_16_9->setEnabled(false);
	}

	CBizCallBack * bizCallback = getBizCallBack();
	if(bizCallback)
	{
		connect(bizCallback,SIGNAL(RecvUpdateDevCodeMsg(__int64)),this,SLOT(onRecvDevCodeMsg(__int64)));
		connect(bizCallback,SIGNAL(RecvNetDevAccess(NetDevAccessInfo)),this,SLOT(onRecvNetCameraAccess(NetDevAccessInfo)));
		connect(bizCallback,SIGNAL(RecvMobileNotify(MobileNotifyInfo)),this,SLOT(onRecvNetCameraDropp(MobileNotifyInfo)));
		connect(bizCallback,SIGNAL(RecvChooseMobileResponse(ChooseMobileInfo)),this,SLOT(onRecvSelNetCameraResp(ChooseMobileInfo)));
	}
    connect(ui.label_codeIcon, SIGNAL(clicked()), this, SLOT(QRCodeClicked()));

	onRecvDevCodeMsg(ClassSeeion::GetInst()->_nClassRoomId);
}

WebCameraSettingPage::~WebCameraSettingPage()
{

}

void WebCameraSettingPage::setTitleBarRect()
{
    QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void WebCameraSettingPage::closeBtnClicked()
{
    C8CommonWindow::close();
}

void WebCameraSettingPage::posOneBtnClicked()
{
	selPublishSeat(0);
}

void WebCameraSettingPage::posTwoBtnClicked()
{
	selPublishSeat(1);
}

void WebCameraSettingPage::posThrBtnClicked()
{
	selPublishSeat(2);
}

void WebCameraSettingPage::vdsOneBtnClicked()
{
	changeSeatVideoScale(0);
}

void WebCameraSettingPage::vdsTwoBtnClicked()
{
	changeSeatVideoScale(1);
}

void WebCameraSettingPage::cameraListItemClicked(MulitCameraListItemWidget * itemCamera)
{
	if(NULL == itemCamera || itemCamera->getCameraSeat() >= 0)
	{
		return;
	}

	int camType = itemCamera->getCameraType();
	int camId = itemCamera->getCameraId();

	LPMEDIADEVICEINFO devMedia = CMediaPublishMgr::getInstance()->getMediaDevice(camId,camType);
	if(NULL == devMedia)
	{
		return;
	}

	LPPUBLISHSEATINFO seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(camType,camId);
	if(NULL != seatPublish)
	{
		return;
	}

	int seatId = ui.videoWidget->getSeatIndex();
	if(seatId < 0)
	{
		return;
	}

	if(m_seatIdUnset >= 0 )		//上次设置还未完成
	{
		return;
	}

	// 设置原来的摄像头的显示
	int nIndex = -1;
	MulitCameraListItemWidget * itemWidget = findListItem(seatId,nIndex);
	if(NULL != itemWidget)
	{
		itemWidget->setCameraSeat(-1);
	}

	seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(seatId);
	if(seatPublish->_ctype == CAMERA_LOCAL)
	{
		CMediaPublishMgr::getInstance()->setSeatVideoDevice(seatId,camType,camId);
		itemCamera->setCameraSeat(seatId);

		if(camType == CAMERA_NETWORK)
		{
			biz::GetBizInterface()->SelectMobileAsCamera(ClassSeeion::GetInst()->_nUserId,camId,true,seatPublish->_url.c_str());

			m_camIdUnset = camId;
			m_camTypeUnset = camType;
			m_seatIdUnset = seatId;
		}
	}
	else
	{
		biz::GetBizInterface()->SelectMobileAsCamera(ClassSeeion::GetInst()->_nUserId,seatPublish->_cid,false,seatPublish->_url.c_str());
		
		m_camIdUnset = camId;
		m_camTypeUnset = camType;
		m_seatIdUnset = seatId;
	}
	
}

void WebCameraSettingPage::timerEvent(QTimerEvent *event)
{
	if(NULL == event)
	{
		return;
	}

	if(event->timerId() == m_idUpdateTimer)
	{
		if(updatePublishSeatInfo())
		{
			killTimer(m_idUpdateTimer);
		}

		return;
	}
}

bool WebCameraSettingPage::updatePublishSeatInfo()
{
	if(!CMediaPublishMgr::getInstance()->isInitMedia())
	{
		return false;
	};
    
	if(CMediaPublishMgr::getInstance()->getCameraCount() <= 0 )
	{
		return false;
	}

	if(!selPublishSeat(0))
	{
		return false;
	}

	MediaDeviceList& devList = CMediaPublishMgr::getInstance()->getMediaDeviceList();
	for(int i=0;i<devList.size();i++)
	{
		LPMEDIADEVICEINFO pCDI = devList.at(i);
		if(NULL == pCDI)
		{
			continue;
		}

		if(pCDI->_type == CAMERA_LOCAL || pCDI->_type == CAMERA_NETWORK)
		{	
			addCameraItem(pCDI->_id,pCDI->_type,pCDI->_name);
		}
	}

	return true;
}

void WebCameraSettingPage::addCameraItem(int id,int type,const string& name)
{
	LPPUBLISHSEATINFO seatInfo = CMediaPublishMgr::getInstance()->getPublishSeatInfo(type,id);
	int nSeat = (seatInfo == NULL) ? -1 : seatInfo->_sid;
	QString camName;

	Util::AnsiToQString(name.c_str(),name.length(),camName);
	QListWidgetItem *item = new QListWidgetItem;
	MulitCameraListItemWidget *itemWidget = new MulitCameraListItemWidget(type,camName, nSeat,id,this);

	ui.listWidget_webCameraList->addItem(item);
	ui.listWidget_webCameraList->setItemWidget(item, itemWidget);

	connect(itemWidget,SIGNAL(itemClicked(MulitCameraListItemWidget *)),this,SLOT(cameraListItemClicked(MulitCameraListItemWidget *)));    
}

bool WebCameraSettingPage::selPublishSeat(int nIndex)
{
	LPPUBLISHSEATINFO seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(nIndex);
	if(NULL == seatPublish )
	{
		return false;
	}

	if(!ui.videoWidget->setSeatIndex(nIndex))
	{
		return false;
	}

	if(seatPublish->_ctype == CAMERA_LOCAL)
	{
		if(seatPublish->_scale == SHOWSCALE4A3)
		{
			ui.radioButton_4_3->setChecked(true);
			ui.radioButton_16_9->setChecked(false);
		}
		else
		{
			ui.radioButton_4_3->setChecked(false);
			ui.radioButton_16_9->setChecked(true);
		}

		ui.radioButton_4_3->setEnabled(true);
		ui.radioButton_16_9->setEnabled(true);
	}
	else
	{
		ui.radioButton_4_3->setChecked(false);
		ui.radioButton_16_9->setChecked(false);

		ui.radioButton_4_3->setEnabled(false);
		ui.radioButton_16_9->setEnabled(false);
	}

	QPushButton* btn[3] = { ui.pushButton_pos1,ui.pushButton_pos2,ui.pushButton_pos3};
	for(int i=0;i<3;i++)
	{
		bool enable = (i!=nIndex)?true:false;		
        btn[i]->setEnabled(enable);
	}
	
	return true;
}

void WebCameraSettingPage::changeSeatVideoScale(int scale)
{
	int nVideo = ui.videoWidget->getSeatIndex();
	if(nVideo < 0)
	{
		return;
	}

	LPPUBLISHSEATINFO seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(nVideo);
	if( NULL==seatPublish || seatPublish->_ctype != CAMERA_LOCAL)
	{
		return;
	}

	scale = scale == 0 ? SHOWSCALE4A3:SHOWSCALE16A9;
	if(scale == seatPublish->_scale)
	{
		return;
	}

	seatPublish->_scale = scale;
	CRTMPPublisher* rtmpPublisher = dynamic_cast<CRTMPPublisher*>(seatPublish->_rtmp);
	if(NULL == rtmpPublisher)
	{
		return;
	}

	rtmpPublisher->setShowScale((SHOWSCALE)scale);
	rtmpPublisher->change();
}

void WebCameraSettingPage::onRecvDevCodeMsg(__int64 roomId)
{
	if(roomId != ClassSeeion::GetInst()->_nClassRoomId)
	{
		return;
	}

	IClassRoomDataContainer* roomContainer = biz::GetBizInterface()->GetClassRoomDataContainer();
	if(NULL == roomContainer)
	{
		return;
	}

	IClassRoomInfo * roomInfo = roomContainer->GetClassRoomInterface(roomId);
	if(NULL == roomInfo)
	{
		return;
	}

	SLClassRoomBaseInfo& roomBaseInfo = roomInfo->GetRoomBaseInfo();	
	ui.editCameraCode->setText(QString::fromWCharArray(roomBaseInfo._szDevAccessCode));
    CQR_Encode qrCode;
    QImage img = qrCode.QRImage(0, 3, false, false, roomBaseInfo._szDevAccessCode);
    if (!img.isNull())
    {
        ui.label_codeIcon->clear();
        ui.label_codeIcon->setPixmap(QPixmap::fromImage(img));
    }
}

void WebCameraSettingPage::QRCodeClicked()
{

}

void WebCameraSettingPage::onRecvNetCameraAccess(NetDevAccessInfo info)
{
	if(info._nRoomId != ClassSeeion::GetInst()->_nClassRoomId)
	{
		return;
	}

	int nIndex = -1;
	MulitCameraListItemWidget * itemWidget = findListItem(info._nUserId,CAMERA_NETWORK,nIndex);
	if(NULL != itemWidget)
	{
		return;
	}
		
	//添加到列表中
	string strDevName;
	Util::QStringToString(QString::fromWCharArray(info._pwszDevName),strDevName);
	addCameraItem(info._nUserId,CAMERA_NETWORK,strDevName);

    CMediaPublishMgr::getInstance()->addNetworkCamera(info._nUserId,info._pwszDevName);
}

void WebCameraSettingPage::onRecvNetCameraDropp(MobileNotifyInfo info)
{
	if(info._nTeacherId != ClassSeeion::GetInst()->_nUserId)
	{
		return;
	}

	int nIndex = -1;
	MulitCameraListItemWidget * itemWidget = findListItem(info._nUserId,CAMERA_NETWORK,nIndex);
	if(NULL == itemWidget)
	{
		return;
	}

	
	delete itemWidget;
	delete ui.listWidget_webCameraList->takeItem(nIndex);;

    CMediaPublishMgr::getInstance()->delNetworkCamera(info._nUserId);

	return;
}

void WebCameraSettingPage::onRecvSelNetCameraResp(ChooseMobileInfo info)
{
	if(info._nTeacherId != ClassSeeion::GetInst()->_nUserId)
	{
		return;
	}
	
	if(info._nAct == 0)		//cancel choose
	{
		if(info._ret != 0)
		{
			//弹出消息提示框
			m_camIdUnset = -1;
			m_camTypeUnset = -1;
			m_seatIdUnset = -1;

			return;
		}

		LPPUBLISHSEATINFO seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(m_seatIdUnset);
		if(NULL == seatPublish)
		{
			return;
		}

		CMediaPublishMgr::getInstance()->setSeatVideoDevice(m_seatIdUnset,m_camTypeUnset,m_camIdUnset);
		
		int nIndex = -1;
		MulitCameraListItemWidget * itemWidget = findListItem(m_camIdUnset,m_camTypeUnset,nIndex);
		if(itemWidget)
		{
			itemWidget->setCameraSeat(m_seatIdUnset);
		}

		if(m_camTypeUnset == CAMERA_NETWORK)
		{
			biz::GetBizInterface()->SelectMobileAsCamera(ClassSeeion::GetInst()->_nUserId,m_camIdUnset,true,seatPublish->_url.c_str());
			return;
		}
	}
	else	//choose
	{	
		if(info._nUserId != m_camIdUnset)
		{
			return;
		}

		LPPUBLISHSEATINFO seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(CAMERA_NETWORK,info._nUserId);
		if(NULL == seatPublish)
		{
			m_camIdUnset = -1;
			m_camTypeUnset = -1;
			m_seatIdUnset = -1;

			return;
		}

		if(info._ret != 0)
		{
			CMediaPublishMgr::getInstance()->stopSeatVideo(seatPublish->_sid);
			seatPublish->_cid = -1;
			seatPublish->_ctype = -1;

			int nIndex = -1;
			MulitCameraListItemWidget * itemWidget = findListItem(m_camIdUnset,m_camTypeUnset,nIndex);
			if(itemWidget)
			{
				itemWidget->setCameraSeat(-1);
			}
		}
        else
        {
            CMediaPublishMgr::getInstance()->startPublishSeatAsync(seatPublish->_sid);
        }
	}

	m_camIdUnset = -1;
	m_camTypeUnset = -1;
	m_seatIdUnset = -1;

	return;
}

MulitCameraListItemWidget * WebCameraSettingPage::findListItem(int id, int type,int& nIndex)
{
	for(int i=0;i<ui.listWidget_webCameraList->count();i++)
	{
		QListWidgetItem *item = ui.listWidget_webCameraList->item(i);
		MulitCameraListItemWidget * itemWidget = (MulitCameraListItemWidget*)ui.listWidget_webCameraList->itemWidget(item);

		if(itemWidget->getCameraId() == id && itemWidget->getCameraType() == type)
		{
			nIndex = i;
			return itemWidget;
		}
	}

	nIndex = -1;
	return NULL;
}

MulitCameraListItemWidget * WebCameraSettingPage::findListItem(int seat,int& nIndex)
{
	for(int i=0;i<ui.listWidget_webCameraList->count();i++)
	{
		QListWidgetItem *item = ui.listWidget_webCameraList->item(i);
		MulitCameraListItemWidget * itemWidget = (MulitCameraListItemWidget*)ui.listWidget_webCameraList->itemWidget(item);

		if(itemWidget->getCameraSeat() == seat)
		{
			nIndex = i;
			return itemWidget;
		}
	}

	nIndex = -1;
	return NULL;
}