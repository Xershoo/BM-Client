//**********************************************************************
//	Copyright （c） 2015-2020. All rights reserved.
//	文件名称：CoursewarePanel.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.18
//	说明：
//	修改记录：
//  备注：
//**********************************************************************

#pragma once

#include <QObject>
#include <QMutex>
#include <vector>
#include "pdfwnd.h"
#include "courseware.h"
#include "../whiteboard/QWhiteBoard.h"
#include "../control/QPDFPreviewList.h"
#include ".././media/FilePlayer.h"
#include "MediaPlayerShowWnd.h"
#include "qimageshow.h"
#include "FlashPlayWidget.h"

#define MAX_CoursewarePannelNO  (8)     //支持最大课件数

typedef struct _curseware_show
{
    _curseware_show()
    {
        _type = 0;
        _id = 0;
        _fileName = "";
		_open = false;
		_show._pdf=NULL;
		_show._media=NULL;
		_show._image=NULL;
		_show._flash = NULL;
    }

    int     _type;
    int     _id;
    union
    {
        QPDFWnd*		        _pdf;
		MediaPlayerShowWnd*		_media;        
        QImageShow*		        _image;
		QFlashPlayWidget*		_flash;
    }_show;
    QString     _fileName;
	bool   _open;
}COURSEWARESHOW, *LPCOURSEWARESHOW;

using namespace std;
typedef vector<LPCOURSEWARESHOW>        CoursewareShowVector;

class QCoursewarePannel : public QObject
{
    Q_OBJECT
public:
    QCoursewarePannel();
    ~QCoursewarePannel();

    void setPreviewList(QListWidget *pwidget);

signals:
    void setShowPage(int nPage);
    void setMediaPlayerPro(unsigned int nPos,string& fileName);

public:
    bool OpenCoursewareFile(QString fileName, bool bCtrl,bool onlyOpen = false);
    void CloseCoursewareFile(QString fileName,bool delCourseware = true);
    void CloseAllCoursewareFile();
	void PauseAllMediaCourseware(QString fileNo);

    bool GetShowCourseware(char &nType, QString &fileName, int &nPage);
    void SetCoursewareShow(char nType, QString fileName, int nPage);

    void PauseCoursewareShow(QString fileName, bool pause);
	
	LPCOURSEWARESHOW GetCurrentCoursewareShow();
    LPCOURSEWARESHOW GetCoursewareShowByFileName(QString& fileName);
	LPCOURSEWARESHOW GetCoursewareShowByShowWidget(QWidget* showWidget);
public:
    void SetMode(QString fileName, WBMode mode);
    void SetTextSize(QString fileName, WBSize tSize);
    void SetColor(QString fileName, WBColor clr);
    void SetEnable(QString fileName, WBCtrl enable);
    void UnDo(QString fileName);
    void Clear(QString fileName);
    void ZoomIn(QString fileName);
    void ZoomOut(QString fileName);
    void GotoPage(QString fileName, int nType, int nPage);
    int  GetTotalPage(QString fileName);

    bool MediaCtrl(QString fileName, int nCtrl, int nSeek);
    unsigned int GetMediaState(QString fileName);

public slots:
    void doCoursewareCtrl(const QString &fileName, int nCtrl);
    void doShowPDFPage(const QString&,int);

    void doMediaPlayProgress(unsigned int nPos,string& fileName);
protected:
    void FreeAllShow();
    void SelCoursewareShow(UINT nIndex);
    void ShowCourseware(LPCOURSEWARESHOW pShow, bool bShow);
    void DelCourseware(LPCOURSEWARESHOW pShow);
	void CloseCourseware(LPCOURSEWARESHOW pShow);
	    
	MediaPlayerShowWnd* OpenMediaFile(QString fileName,bool onlyOpen = false);
	QPDFWnd* OpenPDFFile(QString fileName, int nID, long* lParam = 0, bool bCtrl = true);	
    QImageShow* OpenImageFile(QString fileName);
	QFlashPlayWidget* OpenFlashFile(QString fileName);

protected:
    int         m_nSelIndex;
    QMutex      m_mutex;

    CoursewareShowVector    m_vecCourShow;
    QPDFPreviewList*        m_pdfPreview;

    int         m_nTimer;
private:


};