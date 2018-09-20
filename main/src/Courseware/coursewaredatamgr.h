//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：coursewaredatamgr.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.2
//	说明：
//	修改记录：
//  备注：
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
    QString     m_LastFileName; //上一个显示的课件名称（用于音视频课件切换时的暂停和播放）
    int         m_nFontSize;
    int         m_nColorType;

protected:
    static CoursewareDataMgr*  m_instance;
}; 

