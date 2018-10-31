#include "coursewaretools.h"
#include "session/classsession.h"
#include "CourseAndWhitebMsg.h"
#include "CL8_EnumDefs.h"
#include "classroomdialog.h"
#include "../whiteboard/WhileBoardDataMgr.h"
#include "../Courseware/CoursewareTaskMgr.h"
#include "../Courseware/CoursewarePanel.h"

QcoursewareTools::QcoursewareTools(QWidget *parent)
    : QWidget(parent),m_toolData(this)
{
    ui.setupUi(this);

    m_accountListWidget = new CoursewareListWidget();
    
    connect(m_accountListWidget, SIGNAL(removeItem(CoursewareItem*)), this, SLOT(removeAccountItem(CoursewareItem*)));
    connect(m_accountListWidget, SIGNAL(accountListItemClicked(CoursewareItem*, int)), this, SLOT(accountListItemChanged(CoursewareItem*, int)));
        
    connect(ui.comboBox_listcourseware, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(doSelectCourseware(const QString &)));
    connect(ui.pushButton_left, SIGNAL(clicked()), this, SLOT(pageLeftClicked()));
    connect(ui.pushButton_right, SIGNAL(clicked()), this, SLOT(pageRightClicked()));
    
    connect(CoursewareTaskMgr::getInstance(), SIGNAL(add_courseware(QString, long long, int)), this, SLOT(addCoursewareItem(QString, long long, int)));    
    connect(CoursewareDataMgr::GetInstance()->getCoursewarePannel(), SIGNAL(setShowPage(int)), this, SLOT(setTextPage(int)));    
    connect(CoursewareTaskMgr::getInstance(), SIGNAL(set_pos(QString,int)), this, SLOT(setCoursewarePercent(QString, int)));
    connect(WhiteBoardDataMgr::getInstance(), SIGNAL(add_whiteboard(QString)), this, SLOT(addWhiteboardItem(QString)));

    addItemToAccountListWidget();

    QString page = QString(QString::fromWCharArray(L"ตฺ%1าณ")).arg(1);
    ui.label_page->setText(page);
}

void QcoursewareTools::showUI()
{
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        this->setEnabled(true);
    }
    else if (ClassSeeion::GetInst()->IsStudent())
    {
        this->setEnabled(false);
    }
}

void QcoursewareTools::doSelectCourseware(const QString & fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    ui.comboBox_listcourseware->hidePopup();
    CoursewareDataMgr::GetInstance()->setNowFileName(fileName);
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        m_toolData.openCoursewareORWb(fileName);
    }
}

void QcoursewareTools::removeAccountItem(CoursewareItem* item)
{
    if (NULL == item)
    {
        return;
    }

    m_accountListWidget->removeItemWidget(item->getItem());
    QString fileName = item->getItem()->text();
    delete item->getItem();
    ui.comboBox_listcourseware->hidePopup();
    m_toolData.delViewWidget(fileName);
        
    if(ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->reshowMainState(fileName);
        if (!IsHaveCourseware())
        {
            ClassRoomDialog::getInstance()->setClassRoomUI();
            ClassRoomDialog::getInstance()->showMainState("", MAIN_SHOW_TYPE_NO);
        }
    }
}

void QcoursewareTools::accountListItemChanged(CoursewareItem* item, int row)
{
    if (NULL == item | row < 0)
    {
        return;
    }
    ui.comboBox_listcourseware->setCurrentIndex(row);
    m_accountListWidget->setCurrentRow(row);
    ui.comboBox_listcourseware->hidePopup();
}

void QcoursewareTools::addItemToAccountListWidget()
{
    m_accountListWidget->setFixedHeight(200);
    ui.comboBox_listcourseware->setModel(m_accountListWidget->model());
    ui.comboBox_listcourseware->setView(m_accountListWidget);
}

void QcoursewareTools::addListItem(QString fileName,bool bCourseWare)
{
    bool bIsHaveCwAndWb = IsHaveCourseware();
    if (!bIsHaveCwAndWb&& bCourseWare)
    {
        CoursewareDataMgr::GetInstance()->setNowFileName(fileName);
    }

    QListWidgetItem *listItem = new QListWidgetItem(m_accountListWidget);
    CoursewareItem *item = new CoursewareItem(m_accountListWidget, bCourseWare);
    if(listItem && item)
    {
        QMutexLocker  autoLock(&m_mutexAddItem);

        listItem->setText(fileName);
        item->setItem(listItem);
        m_accountListWidget->setItemWidget(listItem, item);
    }

	ui.comboBox_listcourseware->setCurrentText(fileName);
    
    /*
    if(ClassSeeion::GetInst()->IsTeacher())
    {
	    m_toolData.openCoursewareORWb(fileName);
    }
    */

    if(ClassRoomDialog::isValid() && !bIsHaveCwAndWb)
    {
        ClassRoomDialog::getInstance()->setClassRoomUI();
    }    

}

void QcoursewareTools::addWhiteboardItem(QString fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }
	
    m_toolData.openCoursewareORWb(fileName,true);

    addListItem(fileName,false);
    return; 
}

void QcoursewareTools::addCoursewareItem(QString fileName, long long, int)
{
    if (fileName.isEmpty())
    {
        return;
    }

    int nType = GetCoursewareFileType(wstring((wchar_t*)(fileName).unicode()).data());
    if (COURSEWARE_UNKNOWN == nType)
    {
        return;
    }

    bool bFind = false;
    int nsize = m_accountListWidget->count();
    for (int i = 0; i < nsize; i++)
    {
        QListWidgetItem *item = m_accountListWidget->item(i);
        CoursewareItem *itemData = (CoursewareItem*)m_accountListWidget->itemWidget(item);
        if (NULL == itemData)
        {
            continue;
        }
        QString str = itemData->getItem()->text();
        if (fileName == str)
        {
            bFind = true;
            break;
        }
    }
    
    if (bFind)
    {
        return;
    }

    addListItem(fileName,true);

    return;
}

void QcoursewareTools::pageLeftClicked()
{
    if (CoursewareDataMgr::GetInstance())
    {
        CoursewareDataMgr::GetInstance()->GotoPage(CTRL_PREV_PAGE, 0);
        LPWBAndCourseView pView = m_toolData.GetViewWidget(CoursewareDataMgr::GetInstance()->m_NowFileName);
    }
}

void QcoursewareTools::pageRightClicked()
{
    if (CoursewareDataMgr::GetInstance())
    {
        CoursewareDataMgr::GetInstance()->GotoPage(CTRL_NEXT_PAGE, 0);
        LPWBAndCourseView pView = m_toolData.GetViewWidget(CoursewareDataMgr::GetInstance()->m_NowFileName);
    }
}

void QcoursewareTools::setTextPage(int nPage)
{
    if(nPage <= 0)
    {
        return;
    }

    QString page = QString(QString::fromWCharArray(L"ตฺ%1าณ")).arg(nPage);
    ui.label_page->setText(page);
    LPWBAndCourseView pView = m_toolData.GetViewWidget(CoursewareDataMgr::GetInstance()->m_NowFileName);
    if (pView && pView->m_npage != nPage)
    {
        pView->m_npage = nPage;
    }
}

void QcoursewareTools::ShowMainState(QString fileName, int nPos)
{  
    if(ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->showMainState(fileName,nPos);
    }
}

void QcoursewareTools::ShowcsORwb(QWidget* pWidget, bool bNewAdd)
{
    if(ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->showMainView(pWidget, bNewAdd);
    }
}

void QcoursewareTools::ShowCoursePreview(QString filePath, int npage)
{
    if(ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->showPreviewView(filePath,npage);
    }    
}

void QcoursewareTools::SetDownState(QString fileName, bool bisDown)
{
    if (fileName.isEmpty())
    {
        return;       
    }
	
    int nsize = m_accountListWidget->count();
    bool bIsHaveCwAndWb = nsize == 0 ? false : true;

    for (int i = 0; i < nsize; i++)
    {
        QListWidgetItem *item = m_accountListWidget->item(i);
        CoursewareItem *itemData = (CoursewareItem*)m_accountListWidget->itemWidget(item);
        if (NULL == itemData)
        {
            continue;
        }
        
        QString str = itemData->getItem()->text();
        if (fileName == str)
        {
            itemData->setDowning(bisDown);
            return;
        }        
    }

    QListWidgetItem *listItem = new QListWidgetItem(m_accountListWidget);
    CoursewareItem *item = new CoursewareItem(m_accountListWidget);
    if(listItem && item)
    {
        QMutexLocker  autoLock(&m_mutexAddItem);

        listItem->setText(fileName);
        item->setItem(listItem);
        item->setDowning(bisDown);
        m_accountListWidget->setItemWidget(listItem, item);
    }

    if(ClassRoomDialog::isValid() && !bIsHaveCwAndWb)
    {
        ClassRoomDialog::getInstance()->setClassRoomUI();
    } 
}

void QcoursewareTools::ModifyItemName(QString oldName, QString newName)
{
    int nCount = m_accountListWidget->count();
    for (int i = 0; i < nCount; i++)
    {
        QString strItem = ui.comboBox_listcourseware->itemText(i);
        if (strItem == oldName)
        {
            ui.comboBox_listcourseware->setItemText(i, newName);
            break;
        }
    }
}

void QcoursewareTools::DeleteView(QWidget* pWidget)
{
    if(ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->removeMainView(pWidget);
    }
}

void QcoursewareTools::DeletePreviewView(QString filePath)
{
    if(ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->removePreviewView(filePath);
    }
}

void QcoursewareTools::SetShowCoursewareList(QString filename)
{
    int nCount = m_accountListWidget->count();
    for (int i = 0; i < nCount; i++)
    {
        QString strItem = ui.comboBox_listcourseware->itemText(i);
        if (strItem == filename)
        {
            ui.comboBox_listcourseware->setCurrentIndex(i);
            ui.comboBox_listcourseware->update();
            break;
        }
    }
}

void QcoursewareTools::ShowMediaTool(bool isMedia, QString filename, long nowTime, long totleTime, int state)
{
    if(ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->showMediaTool(isMedia, filename, nowTime, totleTime, state);
    }
}

void QcoursewareTools::setMediaUI(bool isMedia)
{
    if (isMedia)
    {
        ui.pushButton_left->hide();
        ui.pushButton_right->hide();
        ui.label_page->hide();
        ui.comboBox_listcourseware->setFixedWidth(150);
    }
    else
    {
        ui.pushButton_left->show();
        ui.pushButton_right->show();
        ui.label_page->show();
        ui.comboBox_listcourseware->setFixedWidth(180);
    }
}

bool QcoursewareTools::IsHaveCourseware()
{
    if (0 == ui.comboBox_listcourseware->count())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool QcoursewareTools::IsCurrentMedia()
{
    int nType = GetCoursewareFileType(wstring((wchar_t*)(CoursewareDataMgr::GetInstance()->m_LastFileName).unicode()).data());
    if (COURSEWARE_AUDIO == nType || COURSEWARE_VIDEO == nType)
    {
        return true;
    }

    return false;
}

void QcoursewareTools::setWhiteBoardEnable(WBCtrl ctlEnable)
{
	WhiteBoardDataMgr::getInstance()->SetAllEnable(ctlEnable);
}

void QcoursewareTools::DeleteListItem(QString fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    int nCount = m_accountListWidget->count();
    for (int i = 0; i < nCount; i++)
    {
        QString strItem = ui.comboBox_listcourseware->itemText(i);
        if (strItem == fileName)
        {
            ui.comboBox_listcourseware->removeItem(i);
            ui.comboBox_listcourseware->update();
            break;
        }
    }

    if (!IsHaveCourseware())
    {
        if(ClassRoomDialog::isValid())
        {
            ClassRoomDialog::getInstance()->setClassRoomUI();
            ClassRoomDialog::getInstance()->showMainState("", MAIN_SHOW_TYPE_NO);
        }
    }
}

void QcoursewareTools::clearData()
{
    this->disconnect(NULL,NULL,NULL,NULL);

    int nCount = m_accountListWidget->count();
    if (nCount <= 0)
    {
        return;
    }
    
    QMutexLocker  autoLock(&m_mutexAddItem);
    ui.comboBox_listcourseware->clear();
}

void QcoursewareTools::setCoursewarePercent(QString fileName, int nPercent)
{
    if (fileName.isEmpty())
    {
        return;
    }
    
    if (fileName == CoursewareDataMgr::GetInstance()->m_NowFileName) //&& NULL == CoursewareDataMgr::GetInstance()->m_CoursewarePanel->GetCoursewareShow())
    {
        ShowMainState(fileName, nPercent);        
    }
}