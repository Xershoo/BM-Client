#ifndef C8MESSAGEBOX_H
#define C8MESSAGEBOX_H

#include "control/c8commonwindow.h"
#include "ui_c8messagebox.h"

class C8MessageBox : public C8CommonWindow
{
    Q_OBJECT

public:
    enum IconType
    {
        NoIcon,
        Information,
        Warning,
        Critical,
        Question
    };

    explicit C8MessageBox(IconType icontype, QString title, QString detial);
    ~C8MessageBox();

    

    void setIconType(IconType type);
    IconType getIconType();

    void setTitle(QString title);
    QString getTitle();

    void setDetail(QString detail);
    QString getDetail();
public slots:
    virtual int exec();
protected:
    virtual void setTitleBarRect() override;

protected slots:
    void acceptBtnClicked();
    void rejectBtnClicked();
    
private:
    Ui::C8MessageBox ui;
    IconType m_iconType;
    QString m_title;
    QString m_detail;
};

#endif // C8MESSAGEBOX_H
