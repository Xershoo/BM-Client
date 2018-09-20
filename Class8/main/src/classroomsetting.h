#ifndef CLASSROOMSETTING_H
#define CLASSROOMSETTING_H

#include "control/c8commonwindow.h"
#include "ui_classroomsetting.h"

class ClassroomSetting : public C8CommonWindow
{
    Q_OBJECT

public:
    ClassroomSetting(QWidget *parent = 0);
    ~ClassroomSetting();

protected:
    virtual void setTitleBarRect() override;

    void initStyleSheet();
protected slots:
    void closeBtnClicked();
    void minsizeBtnClicked();
    void classroomSettingBtnClicked();
    void privilegeSettingBtnClicked();
    void changeVideoRecordPathBtnClicked();
private:
    Ui::ClassroomSetting ui;
};

#endif // CLASSROOMSETTING_H
