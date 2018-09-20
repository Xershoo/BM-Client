#ifndef WAITDLG_H
#define WAITDLG_H

#include "control/c8commonwindow.h"
#include "ui_WaitDlg.h"

class QWaitDlg : public QDialog
{
	Q_OBJECT
	
public:
	explicit QWaitDlg(QWidget* parent,bool trans=true);
	~QWaitDlg();

	void wait(int interval,QString waitMsg,QString endMsg);
	void wait(QObject* endSender,const char* endSignal, QString waitMsg,QString endMsg);
	
	void cancel();

	void endWait(QString msg);
protected:
	virtual void timerEvent(QTimerEvent * event);
	virtual void showEvent(QShowEvent * event);

protected slots:
	void endWait();
	void okBtnClicked();
	
protected:
	void setDlgPos();
	void initDlgCtrl();
	void setDlgFrame();

	QString		m_endMsg;
	QWidget*	m_parent;
	int			m_timerWait;
	bool		m_trans;
private:
	Ui::QWaitDlg ui;
};

#endif // WATDLG_H
