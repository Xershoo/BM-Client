#include "classroomdialog.h"
#include "./session/classsession.h"
#include "./media/MediaPublishMgr.h"

//xiewb 2018.10.17
void ClassRoomDialog::showClassChatWidget()
{
	ui.toolButton_userList->setEnabled(true);
	ui.toolButton_chatClass->setEnabled(false);
	ui.tabWidget_classroom->setCurrentIndex(0);
}

void ClassRoomDialog::showClassUserList()
{
	ui.toolButton_chatClass->setEnabled(true);
	ui.toolButton_userList->setEnabled(false);
	ui.tabWidget_classroom->setCurrentIndex(1);
}

//xiewb 2018.10.18
void ClassRoomDialog::enableCameraClicked()
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

	ui.pushButton_cameraEnable->setVisible(true);
	ui.pushButton_cameraDisable->setVisible(false);
}

void ClassRoomDialog::disableCameraClicked()
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
	ui.pushButton_cameraEnable->setVisible(false);
	ui.pushButton_cameraDisable->setVisible(true);
}

void ClassRoomDialog::disableMicrophoneClicked()
{
	ui.pushButton_micEnable->setVisible(false);
	ui.pushButton_micDisable->setVisible(true);

	ui.slider_micVolume->setEnabled(false);
	CMediaPublishMgr::getInstance()->setMicMute(true);
}

void ClassRoomDialog::enableMicrophoneClicked()
{
	ui.pushButton_micEnable->setVisible(true);
	ui.pushButton_micDisable->setVisible(false);

	ui.slider_micVolume->setEnabled(true);
	CMediaPublishMgr::getInstance()->setMicMute(false);
}

void ClassRoomDialog::disableSpeakerClicked()
{
	ui.pushButton_spkEnable->setVisible(false);
	ui.pushButton_spkDisable->setVisible(true);

	ui.slider_spkVolume->setEnabled(false);
	CMediaPublishMgr::getInstance()->setSpeakersMute(true);
}

void ClassRoomDialog::enableSpeakerClicked()
{
	ui.pushButton_spkEnable->setVisible(true);
	ui.pushButton_spkDisable->setVisible(false);

	ui.slider_spkVolume->setEnabled(true);
	CMediaPublishMgr::getInstance()->setSpeakersMute(false);
}

void ClassRoomDialog::micVolumeSliderChange(int nVolume)
{
	CMediaPublishMgr::getInstance()->setMicVolume(nVolume);
}

void ClassRoomDialog::spkVolumeSliderChange(int nVolume)
{
	CMediaPublishMgr::getInstance()->setSpeakersVolume(nVolume);
}