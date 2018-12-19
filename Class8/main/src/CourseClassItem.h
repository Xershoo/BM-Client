#ifndef COURSE_CLASS_ITEM_H
#define COURSE_CLASS_ITEM_H

#include <QtWidgets/QWidget>
#include <ui_CourseClassItem.h>
#include "control/c8commonwindow.h"

class CourseClassItem : public C8CommonWindow
{
	Q_OBJECT

public:
	enum{
		class_unknow = 0,
		class_wait,
		class_doing,
		class_end
	};
public:
	CourseClassItem(QWidget* parent);
	~CourseClassItem();

	void setItemParam(__int64 courseId,__int64 classId,int classState,
		QString classImage,bool isTeacher,
		QString className,QString courseTeacher,
		QString timeStart,QString timeEnd);

protected:
	void setItemRound();
	virtual void setTitleBarRect(){};
	virtual void paintEvent(QPaintEvent * event);
	virtual bool eventFilter(QObject *, QEvent *);

	QString getImageFileName();
	void    loadClassImage();
public slots:
	void enterClass();

	void HttpDownImageCallBack(int, unsigned int, bool, unsigned int);
private:
	Ui::CourseClassItem ui;

	__int64	 m_courseId;
	__int64  m_classId;
	int		 m_state;
	QString  m_imageName;
	QPixmap* m_pixmap;

	int      m_idDownClassImage;
};

#endif