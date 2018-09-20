#ifndef SETTING_H
#define SETTING_H

#include "control/c8commonwindow.h"
#include "UserPubliserVideoWnd.h"
#include "ui_settingdialog.h"

class SettingDialog : public C8CommonWindow
{
    Q_OBJECT

public:
    SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

protected:
    virtual void setTitleBarRect() override;
    bool eventFilter(QObject *, QEvent *) override;

    virtual void timerEvent(QTimerEvent *);
    void initStyleSheet();

protected slots:
    void minsizeBtnClicked();
    void closeBtnClicked();
    void videoSettingBtnClicked();
    void voiceSettingBtnClicked();
    void hotkeySettingBtnClicked();
    void micEnableBtnClicked();
    void micDisableBtnClicked();
    void soundEnableBtnClicked();
    void soundDisableBtnClicked();
    void hotkeySettingItemClicked(int,int);

	void cameraSelectChanged(int index);
	void micSelectChanged(int index);
	void speakerSelectChanged(int index);

	void micValumeChanged(int value);
	void spkValumeChanged(int value);

    void resetHotkeyBtnClicked();
    void faceBeautyLevelChanged(int level);

    void hotKeyInputEditChanged(QString);
    
    void videoFluentBtnClicked();
    void videoHighBtnClicked();
    void videoSuperBtnClicked();

protected:
	virtual void closeEvent(QCloseEvent *event);
	
private:
    void initHotkeySetting();
    void initHotkeyValue();
    bool initMediaDeviceShow();

	bool setSpkMute(bool mute);
	bool setMicMute(bool mute);
	void enableMicSound(bool enable);
private:
    Ui::SettingDialog ui;

    int m_currentSettingHotkeyItemRow;
    int m_currentSettingHotkeyItemColumn;

    int m_idLoadMediaDevTimer;
	int m_idGetMicValumeTimer;

    int m_selMicIndex;
};

#endif // SETTING_H
