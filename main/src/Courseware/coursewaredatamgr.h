//**********************************************************************
//	Copyright （c） 2015-2020. All rights reserved.
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
#include "./biz/BizInterface.h"
#include "./biz/BizCallBack.h"
#include "../whiteboard/WhiteBoardData.h"
#include "./whiteboard/WhileBoardDataMgr.h"
#include "./Courseware/coursewaretaskmgr.h"

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

	bool  openCoursewareORWb(QString fileName,bool onlyOpen = false);
	bool  deleteCoursewareORWb(QString fileName);
	bool  closeCourseware(QString fileName);
	bool  delViewWidget(QString viewName);
	
	LPWBAndCourseView  GetViewWidget(QWidget* widgetView);
protected slots:
	void onWhiteBoardEvent(biz::SLWhiteBoardEvent wbEvent);
	void onWhiteBoardOpt(biz::SLWhiteBoardOpt info);
	void onCursewaveListEvent(biz::SLCursewaveListOpt info);
	void onShowTypeChanged(biz::SLClassRoomShowInfo info);
	void onWhiteboardModifyName(QString oldName, QString newName);

	void onCoursewareSetPos(QString fileName, int nPercent);
	void onCoursewareStateChange(QString, int, int);
	void onMediaPlayPosChange(unsigned int nPos,string& fileName);
	void onPDFPageChange(int nPage);
	
protected:
	void doCoursewareEvent(int, QString);
	void doCoursewareCtrl(QString filename, int nPage, QString md5name);
	
	bool doCoursewareShow(PCOURSEWAREDATA pData,bool setNowShow = true);
	bool doWhiteboardShow(LPWHITEBOARDDATA pData,bool setNowShow = true);

	bool setTextPage(QString fileName, int nPage);

	LPWBAndCourseView  GetViewWidget(QString viewName);

	//xiewb 2018.10.29
	void setCoursewareShowBar(LPWBAndCourseView pView,int nType);
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

	//xiewb 2018.10.26
	int getCurShowType();
public:
    QCoursewarePannel   *m_CoursewarePanel;

    QString     m_NowFileName;
    QString     m_LastFileName; //上一个显示的课件名称（用于音视频课件切换时的暂停和播放）
    int         m_nFontSize;
    int         m_nColorType;

protected:
    static CoursewareDataMgr*  m_instance;

	WBAndCourseViewVector		m_vecWBAndCourseView;
	QMutex						m_mutex;
}; 

