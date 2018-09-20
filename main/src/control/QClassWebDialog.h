#ifndef CLASS_WEB_DIALOG_H
#define CLASS_WEB_DIALOG_H

#include "src/control/c8commonwindow.h"
#include "ui_QClassWebDialog.h"

class QClassWebDialog : public C8CommonWindow
{
	Q_OBJECT

public:
	QClassWebDialog(QString url,QString title,QWidget *parent = 0);
    virtual ~QClassWebDialog();

    QClassWebView * getWebView();
public slots:
	void enterClass(QString courseID, QString classId);//在JS中调用该函数

protected:
	void setTitleBarRect();	
	
	void openWebPage(QString url);
protected slots:	
	void onClose();
	void onMinSize();
	void onMaxSize();

	void addObjectToJs();
private:
	Ui::QClassWebDialog ui;
};

class CWebDlgMgr
{
public:
    static CWebDlgMgr* getInstance();
    static void freeInstance();

    void addWebDlg(QClassWebDialog* webDlg);
    void delWebDlg(QClassWebDialog* webDlg);

    void closeAll();
protected:
    CWebDlgMgr(){};
    ~CWebDlgMgr(){
        closeAll();
    };

    QVector<QClassWebDialog*>  m_vecWebDlg;

protected:
    int findWebDlg(QClassWebDialog* webDlg);

protected:
    static CWebDlgMgr* m_instance;
};

#endif