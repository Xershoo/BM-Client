#include "skyfiledialog.h"
#include "control/skyfilelistwidget.h"
#include <qDebug>
#include "Courseware/CourseWareTaskMgr.h"
#include "Courseware/CoursewarePanel.h"
#include "Courseware/SkyCursewaveData.h"
#include "Courseware/courseware.h"
#include "whiteboard/WhileBoardDataMgr.h"
#include "c8messagebox.h"
#include "common/Env.h"


SkyFileDialog::SkyFileDialog(QWidget *parent)
    : C8CommonWindow(parent)
{
    ui.setupUi(this);
    setWindowRoundCorner(ui.widget);
    setModal(true);
    addItem();
}

SkyFileDialog::~SkyFileDialog()
{

}

void SkyFileDialog::setTitleBarRect()
{
    QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void SkyFileDialog::closeBtnClicked()
{
    C8CommonWindow::close();
}

void SkyFileDialog::cancelBtnClicked()
{
    C8CommonWindow::close();
}

void SkyFileDialog::downLoadBtnClicked()
{
    C8CommonWindow::close();

    SkyFileListWidget *listWidget = qobject_cast<SkyFileListWidget *>(ui.listWidget_skyFile);
    if (NULL == listWidget)
    {
        return;
    }

    const QVector<SkyFileListItemWidget*> list = listWidget->getDownloadItems();
    for (auto it = list.begin(); it != list.end(); it++)
    {
        int nCount = CoursewareTaskMgr::getInstance()->GetCoursewareCount() +
            WhiteBoardDataMgr::getInstance()->GetWhiteboardCount();

        if (nCount >= MAX_COURSEWARE_AND_WB)
        {
            QString infoText = QString(tr("CousewareAndWbOutMaxCount").arg(MAX_COURSEWARE_AND_WB));
            C8MessageBox msgBox(C8MessageBox::Warning, QString(tr("info")), infoText);
            msgBox.exec();
            break;
        }

        SkyFileListItemWidget * itemList = (*it);
        if(NULL == itemList)
        {
            continue;
        }

        QString strUrl = (*it)->getDownloadURL();
        QString strName = (*it)->getFileName();
        wchar_t szurl[MAX_URL_LENGTH] = {0};
        wchar_t szname[MAX_PATH] = {0};
        wcscpy(szurl, wstring((wchar_t*)(strUrl).unicode()).data());
        wcscpy(szname, wstring((wchar_t*)(strName).unicode()).data());
        if(!CSkyCursewaveData::getInstance()->SetSel(szname, true))
        {
            continue;
        };
           
        int nError = 0;
        if (!CoursewareTaskMgr::getInstance()->AddCourseBySky(szname, szurl, &nError))
        {
            //添加资料库课件失败
            wchar_t szError[MAX_PATH] = {0};
            GetErrorInfo(szError, nError);
            C8MessageBox msgBox(C8MessageBox::Information, "info", QString::fromWCharArray(szError));
            msgBox.exec();
        }
    }

}

void SkyFileDialog::addItem()
{
    SkyCursewaveVector *vec = CSkyCursewaveData::getInstance()->GetSkyCoursewareVector();
    if (NULL == vec || vec->size() == 0)
    {
        return;
    }
    for (size_t i = 0; i < vec->size(); i++)
    {
        LPSKYCURSEWAVE skyCurseWave = vec->at(i);
        if(NULL == skyCurseWave)
        {
            continue;
        }

        addItem(QString::fromWCharArray(skyCurseWave->m_szFileName), 
            QString::fromWCharArray(skyCurseWave->m_szFileUrl),
            skyCurseWave->m_bISSel);
    }
}

void SkyFileDialog::addItem(QString strName, QString strUrl,bool sel)
{
    if (strName.isEmpty() || strUrl.isEmpty())
    {
        return;
    }
    int nType = GetCoursewareFileType(wstring((wchar_t*)(strName).unicode()).data());
    QListWidgetItem *listWidgetItem = new QListWidgetItem;
    SkyFileListItemWidget* itemWidget = new SkyFileListItemWidget(ui.listWidget_skyFile, nType, strName, strUrl,sel);
    ui.listWidget_skyFile->addItem(listWidgetItem);
    ui.listWidget_skyFile->setItemWidget(listWidgetItem, itemWidget);
}

