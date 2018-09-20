#ifndef QSHOWCLASSSTATE_H
#define QSHOWCLASSSTATE_H

#include <QWidget>
#include "ui_qshowclassstate.h"

class QShowClassState : public QWidget
{
    Q_OBJECT

public:
    QShowClassState(QWidget *parent = 0);
    ~QShowClassState();

    void updatePic(const QSize &size);
public slots:
    void ShowMainState(bool bISBenginClass, QString fileName, int nPos);
    void ReshowMainState(bool bISBenginClass, QString fileName);

protected:
    void doShowMainState(bool bISBenginClass, QString fileName = "", int nPos = 0);

    void doShowNothing();
    void doShowClassOver();
    void doShowUploadFailed();
    void doShowDownFailed();
    void doShowTranFailed();
    void doShowProgress(bool bISBenginClass, QString fileName, int nPos);

protected:
	virtual void resizeEvent(QResizeEvent *event);
private:
    Ui::QShowClassState ui;

protected:
    QString     m_fileName;
	bool		m_beginClass;
	int			m_pos;
};

#endif // QSHOWCLASSSTATE_H
