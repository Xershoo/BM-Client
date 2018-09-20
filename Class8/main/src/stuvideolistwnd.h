#ifndef STUVIDEOLISTWND_H
#define STUVIDEOLISTWND_H

#include <QWidget>
#include "ui_stuvideolistwnd.h"
#include "BizInterface.h"
#include <vector>

class StuVideoListWnd : public QWidget
{
    Q_OBJECT

public:
    StuVideoListWnd(QWidget *parent = 0);
    ~StuVideoListWnd();

    void showStuVideoListWnd();
    void hideStuVideoListWnd();

	StudentVideoWnd* getStudentVideoWnd(__int64 userId);
protected:
    void updateVideoList(std::vector<__int64>& vecUser);

	void enableAllStudentVideo(bool enable);
	void addVideoListUser(__int64 userId);
	void delVideoListUser(__int64 userId);

	bool setVideoPlayerUserId(CRTMPPlayer* rtmpPlayer,__int64 userId);
    void setVideoWidgetTooltip(QWidget* videoWidget,__int64 userId);

protected slots:
	void onUserEnter(__int64 nUserId);
	void onUserLeave(UserLeaveInfo info);
	void onTopVideoChange(TopVideoChangeInfo info);
	void onRecvStudentVideoList(StudentVideoListInfo info);
	void onUpdateUserInfo(__int64 nUserId);
	void onUserSpeakAction(UserSpeakActionInfo info);

	void onSelCameraChanged(int nCamera);

    void onUserSpeakChange(__int64 userId,bool speak);

protected:
	void setVideoWidget(StudentVideoWnd* videoWidget,__int64 userId);

    virtual bool eventFilter(QObject *, QEvent *);
private:
    Ui::StuVideoListWnd ui;

    std::vector<StudentVideoWnd* > m_videoList;

};

#endif // STUVIDEOLISTWND_H
