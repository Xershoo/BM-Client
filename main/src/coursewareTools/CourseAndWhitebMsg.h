//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�CourseAndWhitebMsg.h
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.12.25
//	˵����
//	�޸ļ�¼��
//  ��ע��
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
