#ifndef FINISHWIDGET_H
#define FINISHWIDGET_H

#include <QWidget>
#include "basewidget.h"

namespace Ui {
class FinishWidget;
}

class FinishWidget : public BaseWidget
{
    Q_OBJECT

public:

    explicit FinishWidget(QWidget *parent = 0);
    ~FinishWidget();
	void show();
	void hide();

	void retranslateUI();
protected:
    virtual void setTitleBarRect();

private slots:
    void on_min_Btn_clicked();
    void on_close_Btn_clicked();
    void on_startNow_Btn_clicked();
    void on_finish_Btn_clicked();
    void on_shortcut_checkBox_stateChanged(int arg1);

private:
    Ui::FinishWidget *ui;
    bool isCreateShortcut;
};

#endif // FINISHWIDGET_H
