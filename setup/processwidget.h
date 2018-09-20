#ifndef PROCESSWIDGET_H
#define PROCESSWIDGET_H

#include <QWidget>
#include "basewidget.h"

namespace Ui {
class ProcessWidget;
}

class ProcessWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit ProcessWidget(QWidget *parent = 0);
    ~ProcessWidget();
	void show();
	void hide();

	void retranslateUI();
protected:
    virtual void setTitleBarRect();
private slots:
    void on_min_Btn_clicked();

    void on_close_Btn_clicked();
    void uncompressProcess(int value);

private:
    Ui::ProcessWidget *ui;

signals:
    void sg_installSucess();
};

#endif // PROCESSWIDGET_H
