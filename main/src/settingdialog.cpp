#include "settingdialog.h"
#include <QDebug>
#include "userinfo/userinfomanager.h"
#include "MediaPublishMgr.h"
#include "util.h"
#include "common/Env.h"
#include <QPainterPath>
#include "session/classsession.h"
#include "classroomdialog.h"
#include "setdebugnew.h"

SettingDialog::SettingDialog(QWidget *parent)
	: C8CommonWindow(parent)
    , m_idLoadMediaDevTimer(0)
	, m_idGetMicValumeTimer(0)
    , m_selMicIndex(0)
{
    ui.setupUi(this);
    setWindowRoundCorner(this);
    initStyleSheet();

    ui.pushButton_minSize->hide();
	ui.spkVolumeSlider->setMinimum(0);
	ui.spkVolumeSlider->setMaximum(100);
	ui.spkVolumeSlider->setSingleStep(5);
    
	ui.micVolumeSlider->setMinimum(0);
	ui.micVolumeSlider->setMaximum(100);
	ui.micVolumeSlider->setSingleStep(5);

	ui.pushButton_micDisable->setVisible(false);
	ui.pushButton_micEnable->setVisible(true);
	ui.pushButton_soundDisable->setVisible(false);
	ui.pushButton_soundEnable->setVisible(true);

    ui.pushButton_micDisable->setFixedSize(16,16);
    ui.pushButton_micEnable->setFixedSize(16,16);
    ui.pushButton_soundDisable->setFixedSize(16,16);
    ui.pushButton_soundEnable->setFixedSize(16,16);

	videoSettingBtnClicked();
	initHotkeySetting();

	ui.lineEdit_inputHotkey->installEventFilter(this);
	this->installEventFilter(this);

    if(!initMediaDeviceShow())
    {
        m_idLoadMediaDevTimer = this->startTimer(1000);
    }

	connect(ui.comboBox_Camera,SIGNAL(currentIndexChanged(int)),this,SLOT(cameraSelectChanged(int)));
	connect(ui.comboBox_Mic,SIGNAL(currentIndexChanged(int)),this,SLOT(micSelectChanged(int)));
	connect(ui.comboBox_Speaker,SIGNAL(currentIndexChanged(int)),this,SLOT(speakerSelectChanged(int)));

	connect(ui.micVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(micValumeChanged(int)));
	connect(ui.spkVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(spkValumeChanged(int)));

    connect(ui.pushButton_faceBeauty, SIGNAL(levelChanged(int)), this, SLOT(faceBeautyLevelChanged(int)));
    connect(ui.lineEdit_inputHotkey, SIGNAL(inputTextChanged(QString)), this, SLOT(hotKeyInputEditChanged(QString)));


    connect(ui.pushButton_fluent,SIGNAL(clicked()),this,SLOT(videoFluentBtnClicked()));
    connect(ui.pushButton_highDefinition,SIGNAL(clicked()),this,SLOT(videoHighBtnClicked()));
    connect(ui.pushButton_superdefinition,SIGNAL(clicked()),this,SLOT(videoSuperBtnClicked()));

    ui.tableWidget_hotkeySetting->setFocusPolicy(Qt::NoFocus);
    QHeaderView* headerView = ui.tableWidget_hotkeySetting->horizontalHeader(); // 这个例子是列方向上的表头
    headerView->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	CMediaPublishMgr::getInstance()->setPlayVideo(true);
}

SettingDialog::~SettingDialog()
{

}

void SettingDialog::setTitleBarRect()
{
    QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void SettingDialog::initStyleSheet()
{
    QIcon videoSettingIcon;
    videoSettingIcon.addFile(Env::currentThemeResPath() + "videoSetting_normal.png", QSize(), QIcon::Normal, QIcon::On);
    videoSettingIcon.addFile(Env::currentThemeResPath() + "videoSetting_pressed.png", QSize(), QIcon::Disabled, QIcon::On);
    ui.pushButton_videoSetting->setIcon(videoSettingIcon);

    QIcon voiceSettingIcon;
    voiceSettingIcon.addFile(Env::currentThemeResPath() + "voiceSetting_normal.png", QSize(), QIcon::Normal, QIcon::On);
    voiceSettingIcon.addFile(Env::currentThemeResPath() + "voiceSetting_pressed.png", QSize(), QIcon::Disabled, QIcon::On);
    ui.pushButton_voiceSetting->setIcon(voiceSettingIcon);

    QIcon hotkeySetting;
    hotkeySetting.addFile(Env::currentThemeResPath() + "hotkeySetting_normal.png", QSize(), QIcon::Normal, QIcon::On);
    hotkeySetting.addFile(Env::currentThemeResPath() + "hotkeySetting_pressed.png", QSize(), QIcon::Disabled, QIcon::On);
    ui.pushButton_hotkeySetting->setIcon(hotkeySetting);
}

void SettingDialog::minsizeBtnClicked()
{
    C8CommonWindow::minimumSize();
}

void SettingDialog::closeBtnClicked()
{   
    UserInfoManager userinfoManager(ClassSeeion::GetInst()->_nUserId);
    userinfoManager.updateHotkey("Show_HideMainWnd", ui.tableWidget_hotkeySetting->item(0, 1)->text());
    userinfoManager.updateHotkey("CutScreen", ui.tableWidget_hotkeySetting->item(1, 1)->text());
    userinfoManager.updateHotkey("Open_CloseClassSound", ui.tableWidget_hotkeySetting->item(2, 1)->text());
    userinfoManager.updateHotkey("Open_CloseMic", ui.tableWidget_hotkeySetting->item(3, 1)->text());
    userinfoManager.updateHotkey("HandsUp_Down", ui.tableWidget_hotkeySetting->item(4, 1)->text());
    userinfoManager.updateHotkey("IncressClassroomSound", ui.tableWidget_hotkeySetting->item(5, 1)->text());
    userinfoManager.updateHotkey("DecressClassroomSound", ui.tableWidget_hotkeySetting->item(6, 1)->text());
    C8CommonWindow::close();
}

bool SettingDialog::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui.lineEdit_inputHotkey)
    {
        if (e->type() == QEvent::FocusOut)
        {
            qDebug() << "QEvent::FocusOut";
            
            //ui.tableWidget_hotkeySetting->removeCellWidget(m_currentSettingHotkeyItemRow, m_currentSettingHotkeyItemColumn);
            ui.lineEdit_inputHotkey->hide();
            ui.label_setHotKeyHint->hide();
            return QDialog::eventFilter(obj, e);
        }
    }
    if (obj != ui.lineEdit_inputHotkey)
    {
        if (e->type() == QEvent::MouseButtonPress)
        {
            ui.lineEdit_inputHotkey->hide();
            ui.label_setHotKeyHint->hide();
        }
    }
    return QDialog::eventFilter(obj, e);
}

void SettingDialog::hotKeyInputEditChanged(QString text)
{
    int currentRow = ui.tableWidget_hotkeySetting->currentRow();
    ui.tableWidget_hotkeySetting->item(currentRow, 1)->setText(text);

    if (ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->regHotkey(currentRow, text);
    }
}

void SettingDialog::initHotkeyValue()
{
    UserInfoManager userinfoManager(ClassSeeion::GetInst()->_nUserId);
    QString value = userinfoManager.getHotkeyValue("Show_HideMainWnd");
    ui.tableWidget_hotkeySetting->setItem(0, 0, new QTableWidgetItem(tr("show/hideMainWnd")));
    ui.tableWidget_hotkeySetting->setItem(0, 1, new QTableWidgetItem(value.isEmpty() ? "CTRL + SHIFT+ Z" : value));

    value = userinfoManager.getHotkeyValue("CutScreen");
    ui.tableWidget_hotkeySetting->setItem(1, 0, new QTableWidgetItem(tr("cutScreen")));
    ui.tableWidget_hotkeySetting->setItem(1, 1, new QTableWidgetItem(value.isEmpty() ? "CTRL + ALT + T" : value));

    value = userinfoManager.getHotkeyValue("Open_CloseClassSound");
    ui.tableWidget_hotkeySetting->setItem(2, 0, new QTableWidgetItem(tr("close/openClassroomSound")));
    ui.tableWidget_hotkeySetting->setItem(2, 1, new QTableWidgetItem(value.isEmpty() ? "CTRL + ALT + V" : value));

    value = userinfoManager.getHotkeyValue("Open_CloseMic");
    ui.tableWidget_hotkeySetting->setItem(3, 0, new QTableWidgetItem(tr("close/openMic")));
    ui.tableWidget_hotkeySetting->setItem(3, 1, new QTableWidgetItem(value.isEmpty() ? "CTRL + ALT + D" : value));

    value = userinfoManager.getHotkeyValue("HandsUp_Down");
    ui.tableWidget_hotkeySetting->setItem(4, 0, new QTableWidgetItem(tr("handsUp/Down")));
    ui.tableWidget_hotkeySetting->setItem(4, 1, new QTableWidgetItem(value.isEmpty() ? "CTRL + ALT + A" : value));

    value = userinfoManager.getHotkeyValue("IncressClassroomSound");
    ui.tableWidget_hotkeySetting->setItem(5, 0, new QTableWidgetItem(tr("decressClassroomVoice")));
    ui.tableWidget_hotkeySetting->setItem(5, 1, new QTableWidgetItem(value.isEmpty() ? "CTRL + ALT + X" : value));

    value = userinfoManager.getHotkeyValue("DecressClassroomSound");
    ui.tableWidget_hotkeySetting->setItem(6, 0, new QTableWidgetItem(tr("incressClassRoomVoice")));
    ui.tableWidget_hotkeySetting->setItem(6, 1, new QTableWidgetItem(value.isEmpty() ? "CTRL + ALT + C" : value));
}

void SettingDialog::initHotkeySetting()
{
    m_currentSettingHotkeyItemRow = -1;
    m_currentSettingHotkeyItemColumn = -1;
    ui.lineEdit_inputHotkey->setVisible(false);
    ui.label_setHotKeyHint->setVisible(false);
    QStringList headerList;
    headerList << tr("FunIntroduction") << tr("hotkeySetting");
    ui.tableWidget_hotkeySetting->setHorizontalHeaderLabels(headerList);
    ui.tableWidget_hotkeySetting->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableWidget_hotkeySetting->setSelectionMode(QAbstractItemView::SingleSelection);

    initHotkeyValue();
    
    ui.tableWidget_hotkeySetting->setColumnWidth(0, 130);
    ui.tableWidget_hotkeySetting->setColumnWidth(1, ui.tableWidget_hotkeySetting->width() - 130 - 4);
    ui.tableWidget_hotkeySetting->horizontalHeader()->setSectionResizeMode(QHeaderView::Custom);
    ui.tableWidget_hotkeySetting->horizontalHeader()->setSectionsClickable(false);
}

void SettingDialog::videoSettingBtnClicked()
{
    ui.pushButton_videoSetting->setEnabled(false);
    ui.pushButton_voiceSetting->setEnabled(true);
    ui.pushButton_hotkeySetting->setEnabled(true);
    ui.stackedWidget->setCurrentIndex(0);

	enableMicSound(false);
}

void SettingDialog::voiceSettingBtnClicked()
{
    ui.pushButton_videoSetting->setEnabled(true);
    ui.pushButton_voiceSetting->setEnabled(false);
    ui.pushButton_hotkeySetting->setEnabled(true);
    ui.stackedWidget->setCurrentIndex(1);

	enableMicSound(true);

}

void SettingDialog::hotkeySettingBtnClicked()
{
    ui.pushButton_videoSetting->setEnabled(true);
    ui.pushButton_voiceSetting->setEnabled(true);
    ui.pushButton_hotkeySetting->setEnabled(false);
    ui.stackedWidget->setCurrentIndex(2);

	enableMicSound(false);
}

void SettingDialog::micDisableBtnClicked()
{
	if(!setMicMute(true))
	{
		return;
	}
    ui.pushButton_micDisable->setVisible(false);
    ui.pushButton_micEnable->setVisible(true);
}

void SettingDialog::micEnableBtnClicked()
{
	if(!setMicMute(false))
	{
		return;
	}

    ui.pushButton_micEnable->setVisible(false);
    ui.pushButton_micDisable->setVisible(true);
}

void SettingDialog::soundEnableBtnClicked()
{
	if(!setSpkMute(true))
	{
		return;
	}

    ui.pushButton_soundEnable->setVisible(false);
    ui.pushButton_soundDisable->setVisible(true);
}

void SettingDialog::soundDisableBtnClicked()
{
	if(!setSpkMute(false))
	{
		return;
	}

    ui.pushButton_soundEnable->setVisible(true);
    ui.pushButton_soundDisable->setVisible(false);
}

void SettingDialog::hotkeySettingItemClicked(int row,int column)
{
    //ui.lineEdit_inputHotkey->hide();
    ui.label_setHotKeyHint->setVisible(false);
    qDebug() << "row: " << row << ", column: " << column;
    if (column <= 0)
    {
        return;
    }
    m_currentSettingHotkeyItemRow = row;
    m_currentSettingHotkeyItemColumn = column;

    ui.label_setHotKeyHint->setVisible(true);
    QTableWidgetItem *item = ui.tableWidget_hotkeySetting->item(row, column);
    QString text = item->text();
    ui.lineEdit_inputHotkey->setText(text);
    ui.lineEdit_inputHotkey->show();
    ui.lineEdit_inputHotkey->setFocus();
    
    int height = ui.tableWidget_hotkeySetting->horizontalHeader()->height();
    QRect rc = ui.tableWidget_hotkeySetting->visualItemRect(item);
    QPoint pt = ui.tableWidget_hotkeySetting->mapToParent(QPoint(0, 0));
    pt.setX(pt.x() + rc.left());
    pt.setY(pt.y() + height + rc.top());
    pt.setY(pt.y() + 6);
    rc.setWidth(135);
    rc.setHeight(20);
    ui.lineEdit_inputHotkey->setGeometry(QRect(pt, rc.size()));
    pt.setX(pt.x() + 136);
    
    ui.label_setHotKeyHint->setGeometry(QRect(pt, rc.size()));
    ui.label_setHotKeyHint->setVisible(true);
    ui.lineEdit_inputHotkey->show();
}

void SettingDialog::timerEvent(QTimerEvent *e)
{
	int tid = e->timerId();
    if(tid == m_idLoadMediaDevTimer)
    {
        if(initMediaDeviceShow())
        {
            this->killTimer(m_idLoadMediaDevTimer);
            m_idLoadMediaDevTimer = 0;
        }
    }
	else if(tid== m_idGetMicValumeTimer)
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

		int valume = mgrMedia->GetMicRTVolum();
		ui.micValumeBar->setValue(valume);
	}

}

bool SettingDialog::initMediaDeviceShow()
{
    CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
    if(NULL == mgrMedia)
    {
        return false;
    }
        
    if(!mgrMedia->isInitMedia())
    {
        return false;
    }

    MediaDeviceList& devList = mgrMedia->getMediaDeviceList();
    if(devList.size() <= 0)
    {
        return true;
    }

    for(int i=0;i<devList.size();i++)
    {
        LPMEDIADEVICEINFO devInfo = devList.at(i);
        if(NULL == devInfo)
        {
            continue;
        }

        QString strDevName;
        Util::AnsiToQString(devInfo->_name.c_str(),devInfo->_name.length(),strDevName);

        switch(devInfo->_type)
        {
        case CAMERA_LOCAL:
            {   
                this->ui.comboBox_Camera->addItem(strDevName);
            }
            break;
        case MICROPHONE_LOCAL:
            {
                this->ui.comboBox_Mic->addItem(strDevName);
            }
            break;
        case SPEAKER_LOCAL:
            {
                this->ui.comboBox_Speaker->addItem(strDevName);
            }
            break;
        }
    }

	this->ui.widget_videoPreview->setVideoIndex(mgrMedia->getSelCamera());
	this->ui.widget_videoPreview->setDrawVideoRect(true);
    this->ui.comboBox_Camera->setCurrentIndex(mgrMedia->getSelCamera());
    this->ui.comboBox_Mic->setCurrentIndex(mgrMedia->getSelMic());
    this->ui.comboBox_Speaker->setCurrentIndex(mgrMedia->getSelSpeaker());

    CRTMPPublisher * rtmpPublish = this->ui.widget_videoPreview->getPublisher();
    if(NULL != rtmpPublish)
    {
        MediaLevel ml = rtmpPublish->getMediaLevel();
        if(ml == SMOOTHLEVEL)
        {
            ui.pushButton_fluent->setEnabled(false);
        }
        else if(ml == STANDARRDLEVEL)
        {
            ui.pushButton_highDefinition->setEnabled(false);
        }
        else if(ml == HIGHLEVEL)
        {
            ui.pushButton_superdefinition->setEnabled(false);
        }
        else
        {

        }
    }

    m_selMicIndex = mgrMedia->getSelMic();

	ui.spkVolumeSlider->setValue(mgrMedia->getSpeakersVolume());
	ui.micVolumeSlider->setValue(mgrMedia->getMicVolume());
	
	if(ui.stackedWidget->currentIndex() == 1)
	{
		enableMicSound(true);
	}
	else
	{
		enableMicSound(false);
	}
    return true;
}

void SettingDialog::cameraSelectChanged(int index)
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

    int oldIndex = this->ui.widget_videoPreview->getVideoIndex();
    if(oldIndex == index)
    {
        return;
    }

	this->ui.widget_videoPreview->setVideoIndex(index);
	this->ui.widget_videoPreview->setDrawVideoRect(true);
}

void SettingDialog::micSelectChanged(int index)
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

	int oldIndex = m_selMicIndex;
	if(oldIndex == index)
	{
		return;
	}

    m_selMicIndex = index;	
	enableMicSound(true);
}

void SettingDialog::speakerSelectChanged(int index)
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

	int oldIndex = mgrMedia->getSelSpeaker();
	if(oldIndex == index)
	{
		return;
	}

	mgrMedia->setSelSpeaker(index);
}

void SettingDialog::micValumeChanged(int value)
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
	
	mgrMedia->setMicVolume(value);
}

void SettingDialog::spkValumeChanged(int value)
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

	mgrMedia->setSpeakersVolume(value);
}

bool SettingDialog::setMicMute(bool mute)
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL == mgrMedia)
	{
		return false;
	}

	if(!mgrMedia->isInitMedia())
	{
		return false;
	}

	return mgrMedia->setMicMute(mute);
}

bool SettingDialog::setSpkMute(bool mute)
{
	CMediaPublishMgr* mgrMedia = CMediaPublishMgr::getInstance();
	if(NULL == mgrMedia)
	{
		return false;
	}

	if(!mgrMedia->isInitMedia())
	{
		return false;
	}

	return mgrMedia->setSpeakersMute(mute);
}

void SettingDialog::enableMicSound(bool enable)
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

	int mic = ui.comboBox_Mic->currentIndex();
	if(mic < 0)
	{
		return;
	}

	mgrMedia->PreviewSound(mic,enable);
	if(enable)
	{
		if(m_idGetMicValumeTimer == 0)
		{
			m_idGetMicValumeTimer = startTimer(200);
		}
	}
	else
	{
		killTimer(m_idGetMicValumeTimer);
	}
}

void SettingDialog::resetHotkeyBtnClicked()
{
    ui.tableWidget_hotkeySetting->setItem(0, 0, new QTableWidgetItem(tr("show/hideMainWnd")));
    ui.tableWidget_hotkeySetting->setItem(0, 1, new QTableWidgetItem(tr("CTRL + SHIFT+ Z")));

    ui.tableWidget_hotkeySetting->setItem(1, 0, new QTableWidgetItem(tr("cutScreen")));
    ui.tableWidget_hotkeySetting->setItem(1, 1, new QTableWidgetItem(tr("CTRL + ALT+ T")));

    ui.tableWidget_hotkeySetting->setItem(2, 0, new QTableWidgetItem(tr("close/openClassroomSound")));
    ui.tableWidget_hotkeySetting->setItem(2, 1, new QTableWidgetItem(tr("CTRL + SHIFT+ V")));

    ui.tableWidget_hotkeySetting->setItem(3, 0, new QTableWidgetItem(tr("close/openMic")));
    ui.tableWidget_hotkeySetting->setItem(3, 1, new QTableWidgetItem(tr("CTRL + SHIFT+ D")));

    ui.tableWidget_hotkeySetting->setItem(4, 0, new QTableWidgetItem(tr("handsUp/Down")));
    ui.tableWidget_hotkeySetting->setItem(4, 1, new QTableWidgetItem(tr("CTRL + SHIFT+ A")));

    ui.tableWidget_hotkeySetting->setItem(5, 0, new QTableWidgetItem(tr("decressClassroomVoice")));
    ui.tableWidget_hotkeySetting->setItem(5, 1, new QTableWidgetItem(tr("CTRL + SHIFT+ X")));

    ui.tableWidget_hotkeySetting->setItem(6, 0, new QTableWidgetItem(tr("incressClassRoomVoice")));
    ui.tableWidget_hotkeySetting->setItem(6, 1, new QTableWidgetItem(tr("CTRL + SHIFT+ C")));
}

void SettingDialog::faceBeautyLevelChanged(int level)
{

}

void SettingDialog::closeEvent(QCloseEvent *event)
{
	this->ui.widget_videoPreview->closeVideo();
    int nSelCamera = this->ui.comboBox_Camera->currentIndex();
    int nSelMic = this->ui.comboBox_Mic->currentIndex();

    CMediaPublishMgr::getInstance()->setSelMic(nSelMic);
    CMediaPublishMgr::getInstance()->setSelCamera(nSelCamera);

	if(!ClassRoomDialog::isValid())
	{
		CMediaPublishMgr::getInstance()->setPlayVideo(false);
	}

    enableMicSound(false);
}

void SettingDialog::videoFluentBtnClicked()
{
    CRTMPPublisher * rtmpPublish = this->ui.widget_videoPreview->getPublisher();
    if(NULL == rtmpPublish)
    {
        return;
    }

    rtmpPublish->setMediaLevel(SMOOTHLEVEL);
    if(rtmpPublish->isStart())
    {
        rtmpPublish->change();
    }

    ui.pushButton_fluent->setEnabled(false);
    ui.pushButton_highDefinition->setEnabled(true);
    ui.pushButton_superdefinition->setEnabled(true);
}

void SettingDialog::videoHighBtnClicked()
{
    CRTMPPublisher * rtmpPublish = this->ui.widget_videoPreview->getPublisher();
    if(NULL == rtmpPublish)
    {
        return;
    }

    rtmpPublish->setMediaLevel(STANDARRDLEVEL);
    if(rtmpPublish->isStart())
    {
        rtmpPublish->change();
    }

    ui.pushButton_fluent->setEnabled(true);
    ui.pushButton_highDefinition->setEnabled(false);
    ui.pushButton_superdefinition->setEnabled(true);
}

void SettingDialog::videoSuperBtnClicked()
{
    CRTMPPublisher * rtmpPublish = this->ui.widget_videoPreview->getPublisher();
    if(NULL == rtmpPublish)
    {
        return;
    }

    rtmpPublish->setMediaLevel(HIGHLEVEL);
    if(rtmpPublish->isStart())
    {
        rtmpPublish->change();
    }

    ui.pushButton_fluent->setEnabled(true);
    ui.pushButton_highDefinition->setEnabled(true);
    ui.pushButton_superdefinition->setEnabled(false);
}