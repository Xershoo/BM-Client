#ifndef COURSEDIALOG_H
#define COURSEDIALOG_H

#include "src/control/c8commonwindow.h"
#include "ui_CourseDialog.h"

class CourseDialog : public C8CommonWindow
{
	Q_OBJECT

public:
	CourseDialog(QWidget *parent = 0);
	void openCoursePage();

public slots:
	void enterClass(QString courseID, QString classId);//��JS�е��øú���

protected:
	void setTitleBarRect();	
	void setWindowRoundCorner(QWidget *widget, int roundX, int roundY);

protected slots:	
	void onClose();

	void addObjectToJs();
private:
	Ui::CourseDialog ui;
};

#endif