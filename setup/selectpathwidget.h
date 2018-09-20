#ifndef SELECTPATHWIDGET_H
#define SELECTPATHWIDGET_H

#include <QWidget>
#include "basewidget.h"
namespace Ui {
class SelectPathWidget;
}

class SelectPathWidget : public BaseWidget
{
    Q_OBJECT

signals:
	void sg_startInstall();
	void sg_retureClicked();

public:
    explicit SelectPathWidget(QWidget *parent = 0);
    ~SelectPathWidget();
	void show();
	void hide();

	void retranslateUI();
protected:
    virtual void setTitleBarRect();

private slots:
    void on_min_Btn_clicked();
    void on_close_Btn_clicked();
    void on_install_Btn_clicked();
    void on_return_Btn_clicked();
    void on_browse_Btn_clicked();
	void on_shortcutToMenue_checkBox_stateChanged(int arg1);

private:
    Ui::SelectPathWidget *ui;
	bool isCreateShortCutToMenue;
};

#endif // SELECTPATHWIDGET_H
