#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTimer>
#include <QVector>
class SetUp;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
	 void setWindowRoundCorner(QWidget *widget, int roundX = 2, int roundY = 2);

	 void retranslateUI();
private slots:
    void welcomeFastInstallClicked();
    void customInstallBtnClicked();
    void showFinishWidget();
	void showAgreementWidget();
    void startInstall();
    void agreementCancelBtnClicked();
    void returnClicked();
private:
    Ui::Dialog *ui;
    SetUp *m_setUp;
	int m_picPlayTimer;
	int m_picCount;
	QVector<QWidget*> m_widgetVec; 
	void timerEvent(QTimerEvent *event);
};

#endif // DIALOG_H
