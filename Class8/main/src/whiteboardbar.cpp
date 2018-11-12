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
	int nType = GetCoursewareFileType(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->getNowFileName()).unicode()).data());
	if (COURSEWARE_AUDIO == nType || COURSEWARE_TEXT == nType || CoursewareDataMgr::GetInstance()->getNowFileName().isEmpty())
	{
		return;
	}
	QSize size = m_selectColor.size();
	QPoint ptGlobal = ui.pushButton_classRoomLeftColorBtn->mapToGlobal(QPoint(0, 0));
	ptGlobal.setY(ptGlobal.y() + 40);
	ptGlobal.setX(ptGlobal.x() + (ui.pushButton_classRoomLeftColorBtn->size().width()-size.width())/2);
	m_selectColor.setGeometry(QRect(ptGlobal, size));
	m_selectColor.SetColor(CoursewareDataMgr::GetInstance()->getColorType());
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
	int nType = GetCoursewareFileType(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->getNowFileName()).unicode()).data());
	if (COURSEWARE_AUDIO == nType || COURSEWARE_TEXT == nType || CoursewareDataMgr::GetInstance()->getNowFileName().isEmpty())
	{
		return;
	}
	QSize size = m_selectText.size();
	QPoint ptGlobal = ui.pushButton_classRoomLeftTextBtn->mapToGlobal(QPoint(0, 0));
	ptGlobal.setY(ptGlobal.y() + 40);
	ptGlobal.setX(ptGlobal.x() + (ui.pushButton_classRoomLeftTextBtn->size().width()-size.width())/2);
	m_selectText.setGeometry(QRect(ptGlobal, size));
	CoursewareDataMgr::GetInstance()->SetMode(WB_MODE_TEXT);
	m_selectText.SetFontSize(CoursewareDataMgr::GetInstance()->getFontSize());
	m_selectText.show();
	m_selectText.activateWindow();
}

void ClassRoomDialog::wbUndoClicked()
{
	CoursewareDataMgr::GetInstance()->UnDo();
}

void ClassRoomDialog::setWhiteboardEnable(int enable)
{
	/*xiewb 2018.11.1 
	WhiteBoardDataMgr::getInstance()->SetAllEnable((WBCtrl)enable);
	CoursewareDataMgr::GetInstance()->SetEnable((WBCtrl)enable);
	if(enable != WB_CTRL_EVERY){
		ui.widget_whiteboardToolbar->hide();
	}else{
		ui.widget_whiteboardToolbar->show();
	}
	*/
}