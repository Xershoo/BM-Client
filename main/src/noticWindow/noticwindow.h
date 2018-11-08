#ifndef NOTICWINDOW_H
#define NOTICWINDOW_H

#include "src/control/c8commonwindow.h"
#include "control/c8popupwindow.h"
#include "ui_noticwindow.h"
#include "varname.h"
#include "BizInterface.h"
#include <QTimer>

class NoticWindow : public C8PopupWindow
{
    Q_OBJECT
public:
    static NoticWindow *getInstance();
	static void freeInstance();

    NoticWindow(QWidget *parent = 0);
    ~NoticWindow();

    int GetCount();

public slots:
    void onRecvLoginToken(LPCWSTR pwszToken);
    void updateMsg(MessageListInfo infolist);

protected slots:
    void closeBtnClicked();
    void doLkSeeAll(QString);

private:
    void ShowSelfHome(LPCWSTR wszToken);
    void releaseWidget();
private:
    Ui::NoticWindow ui;
    bool m_bISClickedAllMsg;
    
    static NoticWindow * m_instance;
};

#endif // SKYFILEDIALOG_H
