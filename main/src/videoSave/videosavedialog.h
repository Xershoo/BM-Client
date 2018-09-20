#ifndef VIDEOSAVEDIALOG_H
#define VIDEOSAVEDIALOG_H

#include "control/c8commonwindow.h"
#include "ui_videosavedialog.h"
#include <QString>

class VideoSaveDialog : public C8CommonWindow
{
	Q_OBJECT

public:

	VideoSaveDialog(QWidget *parent = 0);
	~VideoSaveDialog();

protected:
	virtual void setTitleBarRect() override;

private:
	Ui::VideoSaveDialog ui;
	QString m_saveFilePath;

protected slots:
	void close();
	void showFileDlg();
	void save();
};

#endif // VIDEOSAVEDIALOG_H
