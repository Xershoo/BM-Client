//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：coursewaretoolsData.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.29
//	说明：
//	修改记录：
//  备注：
//**********************************************************************

#pragma once

#include <QObject>
#include <QMutex>
#include "varname.h"
#include "../Courseware/coursewaredatamgr.h"

class QcoursewareTools;
class CoursewareToolData : public QObject
{
    Q_OBJECT

public:
    explicit CoursewareToolData(QcoursewareTools* tools,QObject *parent = 0);
    ~CoursewareToolData();

    bool  openCoursewareORWb(QString fileName,bool onlyOpen = false);
    bool  deleteCoursewareORWb(QString fileName);
    bool  delViewWidget(QString ItemName);
    
    LPWBAndCourseView  GetViewWidget(QString ItemName);

public slots:    
    void dealCoursewareState(QString, int, int);
    void doWhiteboardEvent(int, QString);
    void doWhiteboardModifyName(QString, QString);
    void doCoursewareEvent(int, QString);
    void doCoursewareCtrl(QString, int, QString);
    void doWhiteboardCtrl(QString, int);

    void MainShowChanged(int nID, int nType, int nState);   //改变主显示区的显示内容

protected slots:
    bool  showCourseware(int nID, int nState);
    bool  showWhiteBoard(int nID, int nState);

    bool  uploadEventBengin(QString fileName, int nError);
    bool  uploadEventEnd(QString fileName, int nError);
    bool  uploadEventFailed(QString fileName, int nError);
    bool  downEventBengin(QString fileName, int nError);
    bool  downEventEnd(QString fileName, int nError);
    bool  downEventFailed(QString fileName, int nError);

    bool  setTextPage(QString fileName, int nPage);

	bool  closeCourseware(QString fileName);
private:


protected:
    WBAndCourseViewVector   m_vecWBAndCourseView;
    QMutex      m_mutex;

	QcoursewareTools*       m_tools;
};