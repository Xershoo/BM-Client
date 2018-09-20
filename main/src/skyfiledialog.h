#ifndef SKYFILEDIALOG_H
#define SKYFILEDIALOG_H

#include "src/control/c8commonwindow.h"
#include "ui_skyfiledialog.h"

class SkyFileDialog : public C8CommonWindow
{
    Q_OBJECT

public:
    SkyFileDialog(QWidget *parent = 0);
    ~SkyFileDialog();

    void addItem();

protected:
    virtual void setTitleBarRect() override;
    void addItem(QString strName, QString strUrl,bool sel);

protected slots:
    void closeBtnClicked();
    void cancelBtnClicked();
    void downLoadBtnClicked();
private:
    Ui::SkyFileDialog ui;
};

#endif // SKYFILEDIALOG_H
