#ifndef DEVICE_DETECT_HELP_DIALOG_H
#define DEVICE_DETECT_HELP_DIALOG_H

#include "./src/control/c8commonwindow.h"
#include "ui_DeviceDetectHelpDialog.h"

class DeviceDetectHelpDialog : public C8CommonWindow
{
	Q_OBJECT
public:
	DeviceDetectHelpDialog(QWidget* parent=NULL);

protected slots:
	void  skipDetect();
	void  exitDetect();

protected:
	virtual void setTitleBarRect();

protected:
	Ui::DeviceDetectHelpDialog ui;
};
#endif