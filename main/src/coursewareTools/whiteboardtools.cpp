#include "whiteboardtools.h"
#include <QBitmap>
#include <QDesktopWidget>
#include "common/Env.h"
#include "common/macros.h"
#include "Courseware/coursewaredatamgr.h"
#include "session/classsession.h"
#include "Courseware/CoursewareTaskMgr.h"
#include "Courseware/CoursewarePanel.h"

whiteboardtools::whiteboardtools(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.pushButton_classRoomLeftClearAllBtn, SIGNAL(clicked()), this, SLOT(clickBtnClearAll()));
    connect(ui.pushButton_classRoomLeftColorBtn, SIGNAL(clicked()), this, SLOT(clickBtnColor()));
    connect(ui.pushButton_classRoomLeftEraserBtn, SIGNAL(clicked()), this, SLOT(clickBtnEraser()));
    connect(ui.pushButton_classRoomLeftMoveBtn, SIGNAL(clicked()), this, SLOT(clickBtnMove()));
    connect(ui.pushButton_classRoomLeftPenBtn, SIGNAL(clicked()), this, SLOT(clickBtnPen()));
    connect(ui.pushButton_classRoomLeftShowPreviewBtn, SIGNAL(clicked()), this, SLOT(clickBtnPreview()));
    connect(ui.pushButton_classRoomLeftTextBtn, SIGNAL(clicked()), this, SLOT(clickBtnText()));
    connect(ui.pushButton_classRoomLeftUndoBtn, SIGNAL(clicked()), this, SLOT(clickBtnUndo()));
    connect(ui.label_classRoomLeftHide, SIGNAL(clicked()), this, SLOT(leftSlideHideLabelClicked()));
    if (CoursewareDataMgr::GetInstance()->m_CoursewarePanel)
    {
        CoursewareDataMgr::GetInstance()->m_CoursewarePanel->setPreviewList(m_coursewarePreview.GetListWidget());
    }
    connect(&m_selectColor, SIGNAL(set_now_color(int)), this, SLOT(ShowNowColor(int)));
    
    connect(ui.pushButton_showStuVideoListWnd, SIGNAL(clicked()), this, SLOT(showStuVideoListWndBtnClicked()));

    int nHeight = ui.widget_leftSildBar->height()/2 - 66/2;
    QRect rect = ui.label_classRoomLeftHide->rect(); 

	//xiewb 2018.09.29
	ui.pushButton_showStuVideoListWnd->hide();
}

whiteboardtools::~whiteboardtools()
{

}

void whiteboardtools::showStuVideoListWndBtnClicked()
{
    emit show_stu_video_list();
}

void whiteboardtools::leftSlideHideLabelClicked()
{
    int nHeight = (ui.widget_leftSildBar->height() - 66)/2;
    QRect re = ui.verticalSpacer_2->geometry();
    re.setHeight(nHeight);
    ui.verticalSpacer_2->setGeometry(re);
    if (ui.widget_leftSildeToolBtns_bk->isVisible())
    {
        QString pixPath = Env::currentThemeResPath() + "widget_leftSildBar_mask2.png";
        QPixmap pix(pixPath);
        QRegion rgn(0, nHeight, 50, 66);
        ui.widget_leftSildBar->setMask(rgn);
        ui.widget_leftSildeToolBtns_bk->hide();
    }
    else
    {
        QString pixPath = Env::currentThemeResPath() + "widget_leftSildBar_mask.png";
        QPixmap pix(pixPath);
        QRegion rgn1(0, 0, 41, QApplication::desktop()->height());
        QRegion rgn2(0, nHeight, 50, 66);
        rgn1 = rgn1.united(rgn2);
        ui.widget_leftSildBar->setMask(rgn1/*pix.mask()*/);
        ui.widget_leftSildeToolBtns_bk->show();
    }    
}

void whiteboardtools::setRegion()
{
    int nHeight = (ui.widget_leftSildBar->height() - 66)/2;
    QRect re = ui.verticalSpacer_2->geometry();
    re.setHeight(nHeight);
    ui.verticalSpacer_2->setGeometry(re);
    QRegion rgn1(0, 0, 41, QApplication::desktop()->height());
    QRegion rgn2(0, nHeight, 50, 70);
    rgn1 = rgn1.united(rgn2);
    ui.widget_leftSildBar->setMask(rgn1/*pix.mask()*/);
}

void whiteboardtools::clickBtnClearAll()
{
    CoursewareDataMgr::GetInstance()->Clear();
}

void whiteboardtools::clickBtnColor()
{
    int nType = GetCoursewareFileType(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->m_NowFileName).unicode()).data());
    if (COURSEWARE_AUDIO == nType || COURSEWARE_VIDEO == nType ||
        COURSEWARE_IMG == nType || CoursewareDataMgr::GetInstance()->m_NowFileName.isEmpty())
    {
        return;
    }
    QSize size = m_selectColor.size();
    QPoint ptGlobal = ui.pushButton_classRoomLeftColorBtn->mapToGlobal(QPoint(0, 0));
    ptGlobal.setY(ptGlobal.y());
    ptGlobal.setX(ptGlobal.x() + ui.pushButton_classRoomLeftColorBtn->size().width()+5);
    m_selectColor.setGeometry(QRect(ptGlobal, size));
    m_selectColor.SetColor(CoursewareDataMgr::GetInstance()->m_nColorType);
    m_selectColor.show();
    m_selectColor.activateWindow();
}

void whiteboardtools::clickBtnEraser()
{
    CoursewareDataMgr::GetInstance()->SetMode(WB_MODE_ERASER);
}

void whiteboardtools::clickBtnMove()
{
    CoursewareDataMgr::GetInstance()->SetMode(WB_MODE_NONE);
}

void whiteboardtools::clickBtnPen()
{
    CoursewareDataMgr::GetInstance()->SetMode(WB_MODE_CURVE);
}

void whiteboardtools::clickBtnPreview()
{
    PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->m_NowFileName).unicode()).data());
    if (pData && COURSEWARE_STATE_OK == pData->m_nState)
    {
        int nType = GetCoursewareFileType(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->m_NowFileName).unicode()).data());
        if (COURSEWARE_PDF == nType || COURSEWARE_EXCLE == nType || 
            COURSEWARE_DOC == nType || COURSEWARE_PPT == nType)
        {
            QSize size = m_coursewarePreview.size();
            QPoint ptGlobal = this->mapToGlobal(QPoint(0, 0));
            ptGlobal.setY(ptGlobal.y());
            ptGlobal.setX(ptGlobal.x() + ui.widget_leftSildeToolBtns_bk->size().width());
            m_coursewarePreview.setGeometry(QRect(ptGlobal, size));
            m_coursewarePreview.show();
            m_coursewarePreview.activateWindow();
        }

        SAFE_DELETE(pData);
    }
}

void whiteboardtools::clickBtnText()
{
    int nType = GetCoursewareFileType(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->m_NowFileName).unicode()).data());
    if (COURSEWARE_AUDIO == nType || COURSEWARE_VIDEO == nType ||
        COURSEWARE_IMG == nType || CoursewareDataMgr::GetInstance()->m_NowFileName.isEmpty())
    {
        return;
    }
    QSize size = m_selectText.size();
    QPoint ptGlobal = ui.pushButton_classRoomLeftTextBtn->mapToGlobal(QPoint(0, 0));
    ptGlobal.setY(ptGlobal.y());
    ptGlobal.setX(ptGlobal.x() + ui.pushButton_classRoomLeftTextBtn->size().width()+5);
    m_selectText.setGeometry(QRect(ptGlobal, size));
    CoursewareDataMgr::GetInstance()->SetMode(WB_MODE_TEXT);
    m_selectText.SetFontSize(CoursewareDataMgr::GetInstance()->m_nFontSize);
    m_selectText.show();
    m_selectText.activateWindow();
}

void whiteboardtools::clickBtnUndo()
{
    CoursewareDataMgr::GetInstance()->UnDo();
}

void whiteboardtools::ShowPreview(QString filePath, int npage)
{
    m_coursewarePreview.ShowPreview(filePath, npage);
}

void whiteboardtools::RemovePreview(QString filePath)
{
   m_coursewarePreview.RemovePreview(filePath);
}

bool whiteboardtools::releaseViewData()
{
    
    return true;
}

void whiteboardtools::showUI(bool bISShow)
{
    if (ClassSeeion::GetInst()->IsTeacher())
    {	
        ui.pushButton_showStuVideoListWnd->show();
        if (bISShow)
        {   
            ui.pushButton_classRoomLeftMoveBtn->setDisabled(false);
            ui.pushButton_classRoomLeftShowPreviewBtn->setDisabled(false);
            ui.pushButton_classRoomLeftPenBtn->setDisabled(false);
            ui.pushButton_classRoomLeftTextBtn->setDisabled(false);
            ui.pushButton_classRoomLeftColorBtn->setDisabled(false);
            ui.pushButton_classRoomLeftEraserBtn->setDisabled(false);
            ui.pushButton_classRoomLeftUndoBtn->setDisabled(false);
            ui.pushButton_classRoomLeftClearAllBtn->setDisabled(false);
        }
        else
        {   
            ui.pushButton_classRoomLeftMoveBtn->setDisabled(true);
            ui.pushButton_classRoomLeftShowPreviewBtn->setDisabled(true);
            ui.pushButton_classRoomLeftPenBtn->setDisabled(true);
            ui.pushButton_classRoomLeftTextBtn->setDisabled(true);
            ui.pushButton_classRoomLeftColorBtn->setDisabled(true);
            ui.pushButton_classRoomLeftEraserBtn->setDisabled(true);
            ui.pushButton_classRoomLeftUndoBtn->setDisabled(true);
            ui.pushButton_classRoomLeftClearAllBtn->setDisabled(true);
        }
    }
    else if (ClassSeeion::GetInst()->IsStudent())
    {
#ifdef STUDENT_NO_SHOW_VIDEO_LIST
		ui.pushButton_showStuVideoListWnd->hide();
		ui.line_left->hide();
#endif
        if (bISShow)
        {            
            ui.pushButton_classRoomLeftMoveBtn->setDisabled(true);
            ui.pushButton_classRoomLeftPenBtn->setDisabled(false);
            ui.pushButton_classRoomLeftTextBtn->setDisabled(false);
            ui.pushButton_classRoomLeftColorBtn->setDisabled(false);
            ui.pushButton_classRoomLeftEraserBtn->setDisabled(false);
            ui.pushButton_classRoomLeftUndoBtn->setDisabled(false);
            ui.pushButton_classRoomLeftClearAllBtn->setDisabled(false);
            ui.pushButton_classRoomLeftShowPreviewBtn->hide();
        }
        else
        {            
            ui.pushButton_classRoomLeftMoveBtn->setDisabled(true);
            ui.pushButton_classRoomLeftPenBtn->setDisabled(true);
            ui.pushButton_classRoomLeftTextBtn->setDisabled(true);
            ui.pushButton_classRoomLeftColorBtn->setDisabled(true);
            ui.pushButton_classRoomLeftEraserBtn->setDisabled(true);
            ui.pushButton_classRoomLeftUndoBtn->setDisabled(true);
            ui.pushButton_classRoomLeftClearAllBtn->setDisabled(true);
            ui.pushButton_classRoomLeftShowPreviewBtn->hide();
        }
    }
}

void whiteboardtools::ShowNowColor(int nColor)
{
    QIcon colorSettingIcon;
    switch (nColor)
    {
    case WB_COLOR_RED:
        {
            colorSettingIcon.addFile(Env::currentThemeResPath() + "leftSildeColorBtn_normal.png", QSize(), QIcon::Normal, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "leftSildeColorBtn_hover.png", QSize(), QIcon::Active, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "leftSildeColorBtn_pressed.png", QSize(), QIcon::Selected, QIcon::On);
        }
        break;

    case WB_COLOR_YELLOW:
        {
            colorSettingIcon.addFile(Env::currentThemeResPath() + "yellow_normal.png", QSize(), QIcon::Normal, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "yellow_hover.png", QSize(), QIcon::Active, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "yellow_pressed.png", QSize(), QIcon::Selected, QIcon::On);
        }
        break;

    case WB_COLOR_BLUE:
        {
            colorSettingIcon.addFile(Env::currentThemeResPath() + "blue_normal.png", QSize(), QIcon::Normal, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "blue_hover.png", QSize(), QIcon::Active, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "blue_pressed.png", QSize(), QIcon::Selected, QIcon::On);
        }
        break;

    case WB_COLOR_GREEN:
        {
            colorSettingIcon.addFile(Env::currentThemeResPath() + "green_normal.png", QSize(), QIcon::Normal, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "green_hover.png", QSize(), QIcon::Active, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "green_pressed.png", QSize(), QIcon::Selected, QIcon::On);
        }
        break;

    case WB_COLOR_WHITE:
        {
            colorSettingIcon.addFile(Env::currentThemeResPath() + "white_normal.png", QSize(), QIcon::Normal, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "white_hover.png", QSize(), QIcon::Active, QIcon::On);
            colorSettingIcon.addFile(Env::currentThemeResPath() + "white_pressed.png", QSize(), QIcon::Selected, QIcon::On);
        }
        break;
    }
    ui.pushButton_classRoomLeftColorBtn->setIcon(colorSettingIcon);
    ui.pushButton_classRoomLeftColorBtn->setIconSize(QSize(28,28));
}
