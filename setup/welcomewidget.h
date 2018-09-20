#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include <QWidget>
#include "basewidget.h"

namespace Ui {
class WelcomeWidget;
}

class WelcomeWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit WelcomeWidget(QWidget *parent = 0);
    ~WelcomeWidget();
	void show();
	void hide();

	void retranslateUI();
protected:
    virtual void setTitleBarRect();
private slots:

    void on_fast_install_Btn_clicked();
    void on_accept_checkBox_stateChanged(int arg1);
    void on_customInstall_Btn_clicked();
    void on_close_Btn_clicked();
    void on_min_Btn_clicked();
	void on_link_Btn_clicked();
	void on_LangSelectBtn_clicked();
private:
    Ui::WelcomeWidget *ui;

signals:
    void sg_fastInstallBtnClick();
    void sg_customInstallBtnClick();
	void sg_linkBtnClick();
};

#endif // WELCOMEWIDGET_H
