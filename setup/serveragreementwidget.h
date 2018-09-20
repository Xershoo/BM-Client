#ifndef SERVERAGREEMENTWIDGET_H
#define SERVERAGREEMENTWIDGET_H

#include <QWidget>
#include "basewidget.h"
namespace Ui {
class ServerAgreementWidget;
}

class ServerAgreementWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit ServerAgreementWidget(QWidget *parent = 0);
    ~ServerAgreementWidget();

	void retranslateUI();
protected:
    virtual void setTitleBarRect();
private slots:
    void on_close_Btn_clicked();

    void on_min_Btn_clicked();

    void on_cancle_Btn_clicked();

private:
    Ui::ServerAgreementWidget *ui;
signals:
    void sg_cancelBtnClicked();
};

#endif // SERVERAGREEMENTWIDGET_H
