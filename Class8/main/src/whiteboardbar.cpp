#include "classroomdialog.h"
#include "qselectcolor.h"
#include "common/Env.h"
#include "common/macros.h"
#include "Courseware/coursewaredatamgr.h"
#include "session/classsession.h"
#include "Courseware/CoursewareTaskMgr.h"
#include "Courseware/CoursewarePanel.h"
#include "whiteboard/WhileBoardDataMgr.h"

void ClassRoomDialog::wbClearClicked()
{
	CoursewareDataMgr::GetInstance()->Clear();
}

void ClassRoomDialog::wbColorClicked()
{
	int nType = GetCoursewareFileType(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->m_NowFileName).unicode()).data());
	if (COURSEWARE_AUDIO == nType || COURSEWARE_TEXT == nType || CoursewareDataMgr::GetInstance()->m_NowFileName.isEmpty())
	{
		return;
	}
	QSize size = m_selectColor.size();
	QPoint ptGlobal = ui.pushButton_classRoomLeftColorBtn->mapToGlobal(QPoint(0, 0));
	ptGlobal.setY(ptGlobal.y() + 40);
	ptGlobal.setX(ptGlobal.x() + (ui.pushButton_classRoomLeftColorBtn->size().width()-size.width())/2);
	m_selectColor.setGeometry(QRect(ptGlobal, size));
	m_selectColor.SetColor(CoursewareDataMgr::GetInstance()->m_nColorType);
	m_selectColor.show();
	m_selectColor.activateWindow();
}

void ClassRoomDialog::wbEraseClicked()
{
	CoursewareDataMgr::GetInstance()->SetMode(WB_MODE_ERASER);
}

void ClassRoomDialog::wbPenClicked()
{
	CoursewareDataMgr::GetInstance()->SetMode(WB_MODE_CURVE);
}

void ClassRoomDialog::wbTextClicked()
{
	int nType = GetCoursewareFileType(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->m_NowFileName).unicode()).data());
	if (COURSEWARE_AUDIO == nType || COURSEWARE_TEXT == nType || CoursewareDataMgr::GetInstance()->m_NowFileName.isEmpty())
	{
		return;
	}
	QSize size = m_selectText.size();
	QPoint ptGlobal = ui.pushButton_classRoomLeftTextBtn->mapToGlobal(QPoint(0, 0));
	ptGlobal.setY(ptGlobal.y() + 40);
	ptGlobal.setX(ptGlobal.x() + (ui.pushButton_classRoomLeftTextBtn->size().width()-size.width())/2);
	m_selectText.setGeometry(QRect(ptGlobal, size));
	CoursewareDataMgr::GetInstance()->SetMode(WB_MODE_TEXT);
	m_selectText.SetFontSize(CoursewareDataMgr::GetInstance()->m_nFontSize);
	m_selectText.show();
	m_selectText.activateWindow();
}

void ClassRoomDialog::wbUndoClicked()
{
	CoursewareDataMgr::GetInstance()->UnDo();
}

void ClassRoomDialog::setWhiteboardEnable(int enable)
{
	WhiteBoardDataMgr::getInstance()->SetAllEnable((WBCtrl)enable);
	if(enable == WB_CTRL_NONE){
		ui.widget_whiteboardToolbar->hide();
	}else{
		ui.widget_whiteboardToolbar->show();
	}
}