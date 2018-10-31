#include "classroomdialog.h"
#include "./courseware/coursewaredatamgr.h"
#include "./courseware/CoursewarePanel.h"
#include "./courseware/coursewaretaskmgr.h"
#include "./session//classsession.h"

void ClassRoomDialog::showCoursewareWindow()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    QSize size = m_addCoursewareWindow.size();
    QPoint ptGlobal = ui.pushButton_addCourseware->mapToGlobal(QPoint(0, 0));
    ptGlobal.setY(ptGlobal.y() - size.height());
    ptGlobal.setX(ptGlobal.x() - (size.width() / 2 )+ ui.pushButton_addCourseware->size().width() / 2);
    m_addCoursewareWindow.setGeometry(QRect(ptGlobal, size));
	m_addCoursewareWindow.show();
	m_addCoursewareWindow.activateWindow();
	qApp->processEvents(); 
/*  
	m_addCoursewareWindow.hide();
    m_addCoursewareWindow.setWindowOpacity(1);
    m_addCoursewareWindow.showNormal();
    m_addCoursewareWindow.activateWindow();
*/
}

void ClassRoomDialog::nextCoursewareClicked()
{
	coursewareShowCtrl(1);
}

void ClassRoomDialog::preCoursewareClicked()
{
	coursewareShowCtrl(-1);
}

void ClassRoomDialog::preFilePageClicked()
{
	CoursewareDataMgr::GetInstance()->GotoPage(CTRL_PREV_PAGE, 0);
}

void ClassRoomDialog::nextFilePageClicked()
{
	CoursewareDataMgr::GetInstance()->GotoPage(CTRL_NEXT_PAGE, 0);
}

void ClassRoomDialog::addMainView(QWidget *pwidget)
{
	if (NULL == pwidget)
	{
		return;
	}

	int index = ui.stackedWidget_main->indexOf(pwidget);
	if(index >= 0){
		return;
	}

	ui.stackedWidget_main->addWidget(pwidget);
	pwidget->setParent(ui.stackedWidget_main);
	QSize sizeView = ui.stackedWidget_main->size();
	pwidget->setFixedSize(sizeView);
	pwidget->update();

	setCoursewareCtrlBtnUI();
}


void ClassRoomDialog::removeMainView(QWidget *pwidget)
{
	if (NULL == pwidget)
	{
		return;
	}

	if(pwidget->parent() != ui.stackedWidget_main)
	{
		return;
	}

	ui.stackedWidget_main->removeWidget(pwidget);
	setCoursewareCtrlBtnUI();
}

void ClassRoomDialog::showMainView(QWidget *pwidget, bool bAddNew /* = false */)
{
	if (NULL == pwidget)
	{
		return;
	}

	if (bAddNew)
	{
		addMainView(pwidget);
	}

	int widgetIndex = ui.stackedWidget_main->indexOf(pwidget);
	if(widgetIndex < 0){
		return;
	}

	ui.stackedWidget_main->setCurrentIndex(widgetIndex);
	LPWBAndCourseView pView = CoursewareDataMgr::GetInstance()->GetViewWidget(pwidget);
	if(pView){
		setCoursewareNameShow(pView->m_name);
	}

	setCoursewareCtrlBtnUI();
}

void ClassRoomDialog::setPageShowText(int curPage,int allPage)
{
	QString pageText = QString("%1/%2").arg(curPage).arg(allPage);
	ui.label_pageNumber->setText(pageText);

	if(allPage <= 1) {
		ui.pushButton_preFilePage->setEnabled(false);
		ui.pushButton_nextFilePage->setEnabled(false);
		return;
	}

	if(curPage <= 1){
		ui.pushButton_preFilePage->setEnabled(false);
		ui.pushButton_nextFilePage->setEnabled(true);
	} else if(curPage >= allPage){
		ui.pushButton_nextFilePage->setEnabled(false);
		ui.pushButton_preFilePage->setEnabled(true);
	}else{
		ui.pushButton_nextFilePage->setEnabled(true);
		ui.pushButton_preFilePage->setEnabled(true);
	}
}


void ClassRoomDialog::setCoursewareToolUI()
{
	int width = 660;
	int height = ui.widget_coursewareToolbar->height();

	/*
	if (ui.widget_coursewareMediaBar->isVisible()){
		width = 660;
		ui.widget_divToolbar->show();
	}else if(ui.widget_coursewarePageBar->isVisible()){
		width = 500;
		ui.widget_divToolbar->show();
	}else{
		width = 325;
		ui.widget_divToolbar->hide();
	}
	*/

	QSize size = ui.widget_classroomLeft->size();
	QRect rectBar;

	rectBar.setY(size.height()- height);
	rectBar.setX((size.width() - width)/2);
	rectBar.setHeight(height);
	rectBar.setWidth(width);
	ui.widget_coursewareToolbar->setGeometry(rectBar);

	QPoint ptBtn;
	ptBtn.setY(10);
	ptBtn.setX(size.width() - ui.pushButton_closeCourseware->width() - 5);
	
	ui.pushButton_closeCourseware->move(ptBtn);
}

void ClassRoomDialog::setCoursewareTool(int type,int cur,int all,int state)
{
	if(!ClassSeeion::GetInst()->IsTeacher()){
		return;
	}

	switch(type)
	{
	case COURSEWARE_IMG:
	case COURSEWARE_UNKNOWN:
	case COURSEWARE_WHITEBOARD:
		{
			ui.widget_coursewareMediaBar->hide();
			ui.widget_coursewarePageBar->show();
			ui.widget_whiteboardToolbar->show();

			ui.pushButton_nextFilePage->setEnabled(false);
			ui.pushButton_preFilePage->setEnabled(false);
			ui.label_pageNumber->setText(QString("0/0"));
		}
		break;
	case COURSEWARE_PDF:
	case COURSEWARE_EXCLE:
	case COURSEWARE_DOC:
	case COURSEWARE_PPT:
		{
			ui.widget_coursewareMediaBar->hide();
			ui.widget_coursewarePageBar->show();
			ui.widget_whiteboardToolbar->show();

			ui.pushButton_nextFilePage->setEnabled(true);
			ui.pushButton_preFilePage->setEnabled(true);

			setPageShowText(cur,all);
		}
		break;
	case COURSEWARE_AUDIO:
		{
			ui.widget_coursewareMediaBar->show();
			ui.widget_coursewarePageBar->hide();
			ui.widget_whiteboardToolbar->hide();

			ui.slider_mediaProgress->setRange(0,all);

			if(!m_mediaSliderPressed) {
				ui.slider_mediaProgress->setValue(cur);
			}

			if(state == CMediaFilePlayer::PLAY){
				ui.pushButton_playMedia->hide();
				ui.pushButton_pauseMedia->show();
			}else{
				ui.pushButton_playMedia->show();
				ui.pushButton_pauseMedia->hide();
			}
		}
		break;
	case COURSEWARE_VIDEO:
		{
			ui.widget_coursewareMediaBar->show();
			ui.widget_coursewarePageBar->hide();
			ui.widget_whiteboardToolbar->show();

			ui.slider_mediaProgress->setRange(0,all);
			if(!m_mediaSliderPressed) {
				ui.slider_mediaProgress->setValue(cur);
			}

			if(state == CMediaFilePlayer::PLAY){
				ui.pushButton_playMedia->hide();
				ui.pushButton_pauseMedia->show();
			}else{
				ui.pushButton_playMedia->show();
				ui.pushButton_pauseMedia->hide();
			}
		}
		break;
	case COURSEWARE_TEXT:
		{
			ui.widget_coursewareMediaBar->hide();
			ui.widget_coursewarePageBar->show();
			ui.widget_whiteboardToolbar->hide();

			ui.pushButton_nextFilePage->setEnabled(false);
			ui.pushButton_preFilePage->setEnabled(false);
			ui.label_pageNumber->setText(QString("0/0"));
		}
		break;
	}
	
	setCoursewareToolUI();
}

void ClassRoomDialog::showWhiteboardTools(bool bIsShow)
{
	if (!ClassSeeion::GetInst()->IsStudent())
	{
		return;
	}
	if (bIsShow)
	{
		ui.widget_whiteboardToolbar->setVisible(true);
	}
	else 
	{
		ui.widget_whiteboardToolbar->setVisible(false);
	}
}

void ClassRoomDialog::coursewareShowCtrl(int showCtrl)
{
	QStackedWidget* showStackedWidget = ui.stackedWidget_main;
	if(NULL==showStackedWidget){
		return;
	}

	int showIndex = showStackedWidget->currentIndex();
	int showCount = showStackedWidget->count();

	if(showCtrl == 1)
	{
		if(showIndex < (showCount-1))
		{
			showIndex++;
		}
		else
		{
			return;	
		}
	}
	else if(showCtrl == -1)
	{		
		if(showIndex > 1)
		{
			showIndex--;
		}
		else
		{
			return;
		}
	}

	QWidget* widgetView = showStackedWidget->widget(showIndex);
	LPWBAndCourseView viewData = CoursewareDataMgr::GetInstance()->GetViewWidget(widgetView);
	if(NULL != viewData)
	{
		CoursewareDataMgr::GetInstance()->setNowFileName(viewData->m_name);
		CoursewareDataMgr::GetInstance()->openCoursewareORWb(viewData->m_name);
	}
}

void ClassRoomDialog::setCoursewareCtrlBtnUI()
{
	if(!ClassSeeion::GetInst()->IsTeacher()){
		return;
	}

	int widgetIndex = ui.stackedWidget_main->currentIndex();
	if(widgetIndex>=1){
		ui.pushButton_closeCourseware->show();
	}else{
		ui.pushButton_closeCourseware->hide();
	}

	int widgetCount = ui.stackedWidget_main->count();
	if(widgetCount <= 2){
		ui.pushButton_nextCourseware->setEnabled(false);
		ui.pushButton_preCourseware->setEnabled(false);

		return;
	}

	if(widgetIndex <= 1){
		ui.pushButton_nextCourseware->setEnabled(true);
		ui.pushButton_preCourseware->setEnabled(false);
	} else if(widgetIndex == widgetCount-1) {
		ui.pushButton_nextCourseware->setEnabled(false);
		ui.pushButton_preCourseware->setEnabled(true);
	} else {
		ui.pushButton_nextCourseware->setEnabled(true);
		ui.pushButton_preCourseware->setEnabled(true);
	}
}

void ClassRoomDialog::mediaPlayBtnClicked()
{
	if (!ClassSeeion::GetInst()->IsTeacher())
	{
		return;
	}

	if (CoursewareDataMgr::GetInstance()->MediaCtrl(CMediaFilePlayer::PLAY))
	{
		ui.pushButton_playMedia->hide();
		ui.pushButton_pauseMedia->show();
	}
}

void ClassRoomDialog::mediaPauseBtnClicked()
{
	if (!ClassSeeion::GetInst()->IsTeacher())
	{
		return;
	}

	if (CoursewareDataMgr::GetInstance()->MediaCtrl(CMediaFilePlayer::PAUSE))
	{
		ui.pushButton_playMedia->show();
		ui.pushButton_pauseMedia->hide();
	}
}

void ClassRoomDialog::mediaSliderPressed()
{
	m_mediaSliderPressed = true;
}

void ClassRoomDialog::mediaSliderReleased()
{
	if (!ClassSeeion::GetInst()->IsTeacher())
	{
		return;
	}

	int value = ui.slider_mediaProgress->value();
	CoursewareDataMgr::GetInstance()->MediaCtrl(CMediaFilePlayer::SEEK,value);

	m_mediaSliderPressed = false;
}

void ClassRoomDialog::mediaSliderValueChanged(int value)
{	
	
}

void ClassRoomDialog::closeCoursewareClicked()
{
	if(!ClassSeeion::GetInst()->IsTeacher()){
		return;
	}

	int count = ui.stackedWidget_main->count();
	int index = ui.stackedWidget_main->currentIndex();
	if(count <= 1|| index <= 0){
		return;
	}

	QWidget* curWidget = ui.stackedWidget_main->currentWidget();
	if(NULL==curWidget){
		return;
	}

	LPWBAndCourseView curView = CoursewareDataMgr::GetInstance()->GetViewWidget(curWidget);
	if(NULL == curView){
		return;
	}

	bool br = CoursewareDataMgr::GetInstance()->delViewWidget(curView->m_name);
	if(!br){
		return;
	}

	count = ui.stackedWidget_main->count();
	if(count<=1){
		this->showMainState("",MAIN_SHOW_TYPE_NO);
		return;
	}

	if(index >= count){
		index = count - 1;
	}

	curWidget = ui.stackedWidget_main->widget(index);
	if(NULL==curWidget){
		return;
	}

	curView = CoursewareDataMgr::GetInstance()->GetViewWidget(curWidget);
	if(NULL == curView){
		return;
	}

	CoursewareDataMgr::GetInstance()->openCoursewareORWb(curView->m_name);
}

void ClassRoomDialog::setCoursewareNameShow(QString qstrName)
{
	QString txtName = qstrName;
	QFontMetrics fontMetrics(ui.label_nameCourseware->font());
	if(fontMetrics.width(txtName) > ui.label_nameCourseware->width())
	{
		txtName = fontMetrics.elidedText(txtName, Qt::ElideRight, ui.label_nameCourseware->width());
		ui.label_nameCourseware->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	}else{
		ui.label_nameCourseware->setAlignment(Qt::AlignCenter);
	}

	ui.label_nameCourseware->setText(txtName);
}
