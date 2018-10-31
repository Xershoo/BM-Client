//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：CourseAndWhitebMsg.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.25
//	说明：
//	修改记录：
//  备注：
//**********************************************************************

#pragma once

#include <QObject>
#include "BizInterface.h"
#include "BizCallBack.h"

class QCourseAndWhitebMsg : public QObject
{
    Q_OBJECT
public:
    static QCourseAndWhitebMsg* getInstance();
    static void freeInstance();

    ~QCourseAndWhitebMsg(){}

signals:
    void whiteboard_event(int, QString);
    void courseware_event(int, QString);
    void courseware_ctrl(QString, int, QString);
    void whiteboard_ctrl(QString, int);
public slots:
	void doWhiteBoardEvent(biz::SLWhiteBoardEvent wbEvent);
    void doWhiteBoardOpt(biz::SLWhiteBoardOpt info);
    void doCursewaveListEvent(biz::SLCursewaveListOpt info);
    void doShowTypeChanged(biz::SLClassRoomShowInfo info);

private:
     QCourseAndWhitebMsg();

protected:

    static QCourseAndWhitebMsg  *m_instance;
};
