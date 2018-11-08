//**********************************************************************
//	Copyright （c） 2016,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：MediaPlayerShowWnd.cpp
//	版本号：1.0
//	作者： 
//	时间： 
//	说明：音视频课件播放窗口封装的类实现源文件
//  使用说明：
//	修改记录：
//**********************************************************************

#include "MediaPlayerShowWnd.h"
#include "courseware\courseware.h"
#include "common/Util.h"
#include <QtCore/QCoreApplication>
#include "./whiteboard/QWhiteBoard.h"
#include "./common/macros.h"

MediaPlayerShowWnd::MediaPlayerShowWnd(QWidget * parent) : CMediaFilePlayer(parent)
	, m_isVideo(false)
	, m_viewWhiteboard(NULL)
	, m_userId(0)
	, m_courseId(0)	
	, m_wbCtrl(WB_CTRL_NONE)
{
	setAutoShowBack(false);
	setVideoKeepRadio(true);
}

MediaPlayerShowWnd::~MediaPlayerShowWnd()
{

}

void MediaPlayerShowWnd::initializeGL()
{
	makeCurrent();
	initializeOpenGLFunctions();

	glClearColor(0.0f,0.0f,0.0f,1.0f);
}

void MediaPlayerShowWnd::resizeGL(int width,int height)
{
	makeCurrent();
	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, width, -height, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);

	QMutexLocker renderLocker(&m_mutexRenderImage);
	if(m_isVideo) {
		if(NULL != m_renderImage){
			QImage tranImage = m_renderImage->scaled(this->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
			if(!tranImage.isNull()){
				delete m_renderImage;
				m_renderImage = new QImage(tranImage);
			}
		}
	} else {
		SAFE_DELETE(m_renderImage);
	}


	updateScene();
}

void MediaPlayerShowWnd::ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH)
{
    CMediaFilePlayer* userHandle = (CMediaFilePlayer*)ShowHandle;
    if(userHandle != this) {
        return;
    }

	if(!m_isVideo){
		return;
	}

    if(NULL == pData || 0 >= nSize || 0 >= nVideoW || 0 >= nVideoH)
    {
        return;
    }

	unsigned char* decData = pData;
	if ((nVideoW%4) !=0 || (nVideoH%4) !=0){
		//****xie wen bing  2018.11.7 ******
		//后面QT与OpenGL的图像数据处理时数据必须能被4整除，要不然会出现内存崩溃
		//......截取数据内容............
		// 0 0 0 0 0 0 0 0 0 0 0 
		// 0 0 0 0 0 0 0 0 0 0 0 
		// 0 0 0 1 1 1 1 1 1 1 1 
		// 0 0 0 1 1 1 1 1 1 1 1 
		// 0 0 0 1 1 1 1 1 1 1 1 
		// 0 0 0 1 1 1 1 1 1 1 1 
		int vw = nVideoW - (nVideoW%4);
		int vh = nVideoH - (nVideoH%4);
		int vs = vw*vh*3;
		unsigned char* vbuf = (unsigned char*)malloc(vs);

		for(int w = 0;w<vw;w++){
			for(int h=0;h<vh;h++){
				int d_off = (h * vw + w) * 3;
				int s_off = d_off + ((nVideoH%4)* nVideoW + (nVideoW%4)* h)*3;
				memcpy(vbuf+d_off,pData+s_off,3);
			}
		}

		nVideoW = vw;
		nVideoH = vh;

		pData = vbuf;
		nSize = vs;
	}

	bool setWhiteboard = false;
	if(nVideoW!=m_videoWidth||nVideoH !=m_videoHeight){
		setWhiteboard = true;
	}

	showVideoBuffer(nVideoW,nVideoH,true,nSize,pData);

	if(setWhiteboard) {
		setWhiteboardViewBack();
	}

	if(decData != pData){
		free(pData);
		pData = decData;
	}
}

bool MediaPlayerShowWnd::play(const string& file,bool onlyOpen/* = false*/)
{
	WCHAR wszFile[MAX_PATH]={0};
	Util::AnsiToUnicode(file.c_str(),file.length(),wszFile,MAX_PATH);
	int ftype = ::GetCoursewareFileType(wszFile);

	switch(ftype)
	{
	case COURSEWARE_AUDIO:
		m_isVideo = false;
		break;
	case COURSEWARE_VIDEO:
		m_isVideo = true;
		break;
	default:
		return false;		
	}
	
	return CMediaFilePlayer::play(file,onlyOpen);
}

void MediaPlayerShowWnd::createWhiteboardView()
{
	if (!m_isVideo)
	{
		return;
	}

	m_viewWhiteboard = new QWhiteBoardView(this,false);
	if(NULL == m_viewWhiteboard)
	{
		return;
	}

	__int64 idOwner = (__int64)calcFileId(); 

	m_viewWhiteboard->setUserId(m_userId);
	m_viewWhiteboard->setPageId(0);
	m_viewWhiteboard->setCourseId(m_courseId);
	m_viewWhiteboard->setOwnerId(idOwner);
	m_viewWhiteboard->hide();
	m_viewWhiteboard->installEventFilter(this);
	m_viewWhiteboard->setEnable((WBCtrl)m_wbCtrl);
	m_viewWhiteboard->setColor(WB_COLOR_BLUE);
	m_viewWhiteboard->setMode(WB_MODE_CURVE);
	m_viewWhiteboard->setTextSize(WB_SIZE_SMALL);
}

int MediaPlayerShowWnd::calcFileId()
{
	int nId = 0;
	char szFile[512] = { 0 };
	strcpy_s(szFile,getFile().c_str());

	if(strlen(szFile) <= 0)
	{
		return 0;
	}

	char * pszFileName = strrchr(szFile,'\\');
	if(NULL != pszFileName)
	{
		nId = (int)Util::crc32((unsigned char*)pszFileName,strlen(pszFileName));
	}

	return nId;
}

void MediaPlayerShowWnd::setWhiteboardViewBack()
{
	if(NULL == m_viewWhiteboard)
	{
		return;
	}
	
	if(m_videoWidth<=0||m_videoHeight<=0){
		return;
	}

	QSize sizeImage(m_videoWidth,m_videoHeight);
	QRect rectWidget= this->rect();

	sizeImage.scale(rectWidget.width()+1,rectWidget.height()+1,Qt::KeepAspectRatio);

	QRect rectImage = this->calcImagePaintRect(rectWidget,sizeImage.width(),sizeImage.height());
	
	m_viewWhiteboard->setSizeAndSceneRect(rectImage); 	
	m_viewWhiteboard->setBackColor(QColor(255,255,255,0));
	m_viewWhiteboard->show();
	m_viewWhiteboard->update();
}

void MediaPlayerShowWnd::resizeEvent(QResizeEvent *event)
{	
	CMediaFilePlayer::resizeEvent(event);
	setWhiteboardViewBack();
}

bool MediaPlayerShowWnd::eventFilter(QObject * obj, QEvent * event)
{
	if(obj != m_viewWhiteboard)
	{
		return false;
	}

	if(event->type() == QEvent::KeyPress)
	{
		keyPressEvent((QKeyEvent*)event);
		m_viewWhiteboard->setFocus();
	}

	return false;
}

unsigned int MediaPlayerShowWnd::getCurPlayTime(bool video /* = false */)
{
	if(m_fileName.empty())
	{
		return 0;
	}

	if(video && m_isVideo)
	{
		return ::getFileStreamCurTime(m_fileName.c_str(),true);
	}

	return ::getFileCurPlayTime(m_fileName.c_str());
}


bool MediaPlayerShowWnd::seekEx(unsigned int pos)
{
	Util::PrintTrace("MediaPlayerShowWnd::seekEx %d",pos);
	bool br = false;
	unsigned int curPlay = 0;	
	if(m_isVideo)
	{		
		br = ::AVP_SeekFileStream(m_fileName.c_str(),pos,true);
	}
	else
	{
		curPlay = getCurPlayTime();
		if(curPlay == pos)
		{
			return true;
		}

		br = ::AVP_SeekFile(m_fileName.c_str(),pos);
	}
	
	if(br)
	{
		updateScene();
	}

	return br;
}

void MediaPlayerShowWnd::drawNoVideoImage()
{
	if(m_backImage.isNull()){
		return;
	}

	if(NULL != m_renderImage){
		return;
	}

	QRect rectShow = this->rect();

	m_renderImage = new QImage(rectShow.width(),rectShow.height(),QImage::Format_RGBA8888);
	QPainter painter(m_renderImage);

	QRect imageRect = calcImagePaintRect(rectShow,m_backImage.width(),m_backImage.height());
	QBrush backBrush(QColor(255,255,255,255));

	painter.fillRect(rectShow,backBrush);
	painter.drawImage(imageRect,m_backImage);
}