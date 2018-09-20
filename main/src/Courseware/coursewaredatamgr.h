//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�coursewaredatamgr.h
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.12.2
//	˵����
//	�޸ļ�¼��
//  ��ע��
//**********************************************************************

#pragma once

#include <QObject>
#include <QWidget>
#include <vector>
#include "../whiteboard/WhiteBoardData.h"

typedef struct tagWBAndCourseView 
{
    tagWBAndCourseView()
    {
        m_widget = NULL;
        m_name = "";
        m_npage = 0;
    }

    QWidget     *m_widget;
    QString     m_name;
    int         m_npage;
}WBAndCourseView, *LPWBAndCourseView;

using namespace std;
typedef vector<LPWBAndCourseView> WBAndCourseViewVector;

class QCoursewarePannel;
class CoursewareTaskMgr;

class CoursewareDataMgr : public QObject
{
    Q_OBJECT
public:
    CoursewareDataMgr();
    virtual ~CoursewareDataMgr();

public:
    static CoursewareDataMgr *GetInstance();
    static void freeInstance();
    static bool isValid();

    void ReleaseData();

    void setNowFileName(QString fileName);

    void SetMode(WBMode mode);
    void SetTextSize(WBSize tSize);
    void SetColor(WBColor clr);
    void SetEnable(WBCtrl enable);
    void UnDo();
    void Clear();
    void ZoomIn();
    void ZoomOut();
    void GotoPage(int nType, int nPage);
    int  GetTotalPage();

    bool MediaCtrl(int nCtrl, int nSeek = 0);
    unsigned int  getMediaState();
public:
    QCoursewarePannel   *m_CoursewarePanel;

    QString     m_NowFileName;
    QString     m_LastFileName; //��һ����ʾ�Ŀμ����ƣ���������Ƶ�μ��л�ʱ����ͣ�Ͳ��ţ�
    int         m_nFontSize;
    int         m_nColorType;

protected:
    static CoursewareDataMgr*  m_instance;
}; 

