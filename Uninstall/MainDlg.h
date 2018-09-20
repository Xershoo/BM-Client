#ifndef MAINDLG_H
#define MAINDLG_H

#include <QtWidgets/QDialog>
#include <QtCore/QThread>
#include "ui_MainDlg.h"

enum
{
    UNINSTALL_DEL_REGEDIT = 0,
    UNINSTALL_DEL_SHORTCUT,
    UNINSTALL_DEL_FILES,
    UNINSTALL_FINISHED,
};

class UninstallThread : public QThread
{
	Q_OBJECT
public:
	UninstallThread(QObject * parent = NULL);
	virtual ~UninstallThread();

protected:
	void run();

protected:
    void delRegeditData();
    void delShortcut();
    void delProgramFile();

    bool removePath(QString& pathName);

signals:
    void progress(int state);

private:
    QString m_dirInstall;
};


class MainDlg : public QDialog
{
    Q_OBJECT

public:
    MainDlg(QWidget *parent = 0);
    ~MainDlg();

protected:
	void setWindowRoundCorner(QWidget *widget, int roundX, int roundY);

protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);	
	virtual void closeEvent(QCloseEvent * event);

protected slots:
	void minBtnClicked();
	void closeBtnClicked();
    void uninstallBtnClicked();
	
    void uninstallProgress(int state);

protected:
	void calcTitleRect();

protected:
	QRect	m_titlRect;
	bool	m_moveable;
	QPoint	m_dragPosition;

	UninstallThread   m_thrdUninstall;	
private:
    Ui::MainDlg ui;
};

#endif // MAINDLG_H
