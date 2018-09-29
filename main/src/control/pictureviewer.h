#ifndef PICTUREVIEWER_H
#define PICTUREVIEWER_H

#include "src/control/c8commonwindow.h"
#include "ui_pictureviewer.h"

class PictureViewer : public C8CommonWindow
{
    Q_OBJECT

public:
    static PictureViewer* getInstance();
	static void freeInstance();
    
    ~PictureViewer();

    void openImage(QString path);
protected:
    PictureViewer(QWidget *parent = 0);
    virtual void setTitleBarRect() override;

protected slots:
    void closeBtnClicked();

protected:
	static PictureViewer* m_instance;

private:
    Ui::PictureViewer ui;
};

#endif // PICTUREVIEWER_H
