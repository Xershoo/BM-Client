#include "qshowclassstate.h"
#include "session/classsession.h"
#include "common/Env.h"
#include "Courseware/courseware.h"

QShowClassState::QShowClassState(QWidget *parent)
    : QWidget(parent)
	,m_beginClass(false)
	,m_pos(0xffff)
	,m_fileName()
{
    ui.setupUi(this);   
}

QShowClassState::~QShowClassState()
{

}

void QShowClassState::ShowMainState(bool bISBenginClass, QString fileName, int nPos)
{    
    doShowMainState(bISBenginClass, fileName, nPos);
}

void QShowClassState::doShowMainState(bool bISBenginClass, QString fileName /* = "" */, int nPos /* = 0 */)
{
	if(nPos == m_pos && m_fileName == fileName && m_beginClass == bISBenginClass)
	{
		return;
	}

    if (m_fileName == fileName && (MAIN_SHOW_TYPE_UPLOAD_FAILED == m_pos || MAIN_SHOW_TYPE_DOWN_FAILED == m_pos))
    {
        return;
    }

    if (nPos >= 0 && nPos <= 100)
    {
        doShowProgress(bISBenginClass, fileName, nPos);
        return;
    }

    m_fileName = fileName;
    m_beginClass = bISBenginClass;
    m_pos = nPos;

    switch (nPos)
    {
    case MAIN_SHOW_TYPE_NO:
        {
            doShowNothing();
        }
        break;

    case MAIN_SHOW_TYPE_CLASS_OVER:
        {
            doShowClassOver();
        }
        break;

    case MAIN_SHOW_TYPE_UPLOAD_FAILED:
        {
            doShowUploadFailed();
        }
        break;

    case MAIN_SHOW_TYPE_DOWN_FAILED:
        {
            doShowDownFailed();
        }
        break;
    case MAIN_SHOW_TYPE_TRANS_FAILED:
        {
            doShowTranFailed();
        }
        break;
    default:
        break;
    }

	update();
}

void QShowClassState::ReshowMainState(bool bISBenginClass, QString filename)
{
    if (filename == m_fileName)
    {
        doShowMainState(bISBenginClass, "", MAIN_SHOW_TYPE_NO);
    }
}

void QShowClassState::updatePic(const QSize &size)
{
    ui.label_picture->setFixedSize(size);
    ui.label_picture->update();

	if(!ui.label_picture->isVisible()){
		return;
	}

	const QPixmap* picPixmap = ui.label_picture->pixmap();
	if(NULL != picPixmap){
		ui.label_picture->setPixmap(picPixmap->scaled(ui.label_picture->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
	}
}

void QShowClassState::resizeEvent(QResizeEvent *event)
{

}

void QShowClassState::doShowNothing()
{
    if (m_beginClass)
    {
		//已上课，无课件
		QPixmap map = Env::currentThemeResPath() + "courseware_state_back.png";;
		ui.label_picture->setPixmap(map.scaled(ui.label_picture->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }
    else
    {
		QPixmap map = Env::currentThemeResPath() + "notclass.png";;
		ui.label_picture->setPixmap(map.scaled(ui.label_picture->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    }

	ui.widget_pic->show();
	ui.widget_info->hide();                
	ui.widget_pro->hide();
}

void QShowClassState::doShowClassOver()
{
    QPixmap map = Env::currentThemeResPath() + "classover.png";;
    ui.label_picture->setPixmap(map);

    ui.widget_pic->show();
    ui.widget_info->hide();                
    ui.widget_pro->hide();
}

void QShowClassState::doShowDownFailed()
{
	ui.widget_pic->hide();
	ui.widget_info->show();                
	ui.widget_pro->hide();

    QString strInfo = QString(tr("CoursewareDownFailed")).arg(m_fileName);
    ui.label_courseware_name->setText(strInfo);
}

void QShowClassState::doShowUploadFailed()
{
	ui.widget_pic->hide();
	ui.widget_info->show();                
	ui.widget_pro->hide();

    QString strInfo = QString(tr("CoursewareUploadFailed")).arg(m_fileName);
    ui.label_courseware_name->setText(strInfo);
}

void QShowClassState::doShowTranFailed()
{
	ui.widget_pic->hide();
	ui.widget_info->show();                
	ui.widget_pro->hide();

    QString strInfo = QString(tr("CoursewareTransFailed")).arg(m_fileName);
    ui.label_courseware_name->setText(strInfo);
}

void QShowClassState::doShowProgress(bool bISBenginClass, QString fileName, int nPos)
{
    if (fileName.isEmpty())
    {
        return;
    }

	ui.widget_pic->hide();
    ui.widget_pro->show();

    if (fileName == m_fileName && nPos == m_pos)
    {
        return;
    }

    QString strInfo;
    if (100 == nPos)
    {
        if (bISBenginClass)
        {
            strInfo = QString(tr("CoursewareLoading")).arg(fileName); 
        }
        else
        {
            strInfo = QString(tr("WaitClass")).arg(fileName);
        }
    }
    else
    {
        strInfo = QString(tr("CoursewareProgress")).arg(fileName).arg(nPos);
    }

    m_fileName = fileName;
    m_beginClass = bISBenginClass;
    m_pos = nPos;

    if (!bISBenginClass && !ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    ui.label_courseware_name->setText(strInfo);
    ui.progressBar_progress->setValue(nPos);  
}
