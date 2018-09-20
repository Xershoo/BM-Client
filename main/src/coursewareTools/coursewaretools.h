#ifndef COURSEWARETOOLS_H
#define COURSEWARETOOLS_H

#include <QWidget>
#include "ui_coursewaretools.h"
#include <QListWidget>
#include "control/accountitem.h"
#include "varname.h"
#include "../Courseware/coursewaredatamgr.h"
#include "coursewaretoolsData.h"

class QcoursewareTools : public QWidget
{
    Q_OBJECT

public:
    QcoursewareTools(QWidget *parent = 0);
    ~QcoursewareTools(){}

    void showUI();
    void setMediaUI(bool isMedia);

    bool IsHaveCourseware();
    bool IsCurrentMedia();

	void setWhiteBoardEnable(WBCtrl ctlEnable);
    void clearData();

public slots:
	void ShowMainState(QString, int);
	void ShowcsORwb(QWidget*, bool);
	void SetDownState(QString, bool);
	void ModifyItemName(QString, QString);
	void DeleteView(QWidget*);
	void setTextPage(int nPage);
	void ShowCoursePreview(QString, int);
	void DeletePreviewView(QString);
	void SetShowCoursewareList(QString);

	void ShowMediaTool(bool, QString, long, long, int);
	void DeleteListItem(QString fileName);

public slots:
    void addWhiteboardItem(QString);
    void addCoursewareItem(QString, long long, int);
    void setCoursewarePercent(QString, int);

protected slots:
    void removeAccountItem(CoursewareItem* item);
    void accountListItemChanged(CoursewareItem* item, int row);
    void doSelectCourseware(const QString & fileName);

    void pageLeftClicked();
    void pageRightClicked();

protected:
    void addListItem(QString fileName,bool bCourseWare);

private:
    void addItemToAccountListWidget();

private:
    Ui::QcoursewareTools ui;
    QListWidget *m_accountListWidget;

    QMutex      m_mutexAddItem;
public:
    CoursewareToolData  m_toolData;
};

#endif // COURSEWARETOOLS_H
