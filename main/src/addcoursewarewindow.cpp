#include "addcoursewarewindow.h"
#include "skyfiledialog.h"
#include <QFileDialog>
#include <QDebug>
#include "Courseware/coursewaredatamgr.h"
#include "c8messagebox.h"
#include "addwhiteboarddialog.h"
#include "session/classsession.h"
#include "Courseware/courseware.h"
#include "whiteboard/WhileBoardDataMgr.h"
#include "CourseWare/CourseWareTaskMgr.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QBitmap>
#include "classroomdialog.h"
#include "lang.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////////
AddCoursewareTipDlg::AddCoursewareTipDlg(QWidget* parent)
{    
    if(NULL == parent)
    {
        this->hide();
        return;
    }

    QSize   sizeDlg = parent->size();
    QPoint  posDlg = parent->pos();
    
    this->setGeometry(posDlg.x()-2,posDlg.y()-2,sizeDlg.width()+4,sizeDlg.height()+4);

    this->setWindowFlags(Qt::FramelessWindowHint);    
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowState(Qt::WindowActive);
    this->setWindowTitle(QApplication::translate("ClassRoomDialog", "ClassRoomDialog", 0));

    QHBoxLayout* hboxLayout = new QHBoxLayout(this);
    this->setLayout(hboxLayout);
    this->setContentsMargins(0,0,0,0);
    
    QWidget * backWidget = new QWidget(this);
    hboxLayout->addWidget(backWidget);

    backWidget->setStyleSheet(QString("background-color: none; background-image: url(:/res/res/image/default/coursewareSize_tip.png)"));
    backWidget->show();
    backWidget->installEventFilter(this);
    backWidget->setMouseTracking(true);    
    this->setMouseTracking(true);
    this->activateWindow();
}

AddCoursewareTipDlg::~AddCoursewareTipDlg()
{

}

void AddCoursewareTipDlg::mousePressEvent(QMouseEvent * event)
{
    if(NULL == event)
    {
        return;
    }

    QPointF mousePos = event->screenPos();
    QPoint dlgPos = this->pos();

    QPoint calcPos(mousePos.x()-dlgPos.x(),mousePos.y()-dlgPos.y());
    QRect  rectBtn(380,590,210,30);
    
    if(rectBtn.contains(calcPos))
    {
        event->ignore();
        QWidget::close();

        return;
    }

    event->accept();
}

void AddCoursewareTipDlg::mouseMoveEvent(QMouseEvent * event)
{
    if(NULL == event)
    {
        return;
    }

    QPointF mousePos = event->screenPos();
    QPoint dlgPos = this->pos();

    QPoint calcPos(mousePos.x()-dlgPos.x(),mousePos.y()-dlgPos.y());
    QRect  rectBtn(380,590,210,30);

    if(rectBtn.contains(calcPos))
    {
        setCursor(Qt::PointingHandCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }

    event->accept();
}

bool AddCoursewareTipDlg::eventFilter(QObject * obj, QEvent * event)
{    
    if(event->type() == QEvent::MouseMove)
    {
        mouseMoveEvent((QMouseEvent*)event);
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
///
bool AddCoursewareWindow::m_showAddTipDlg = false;

AddCoursewareWindow::AddCoursewareWindow(QWidget *parent)
    : C8PopupWindow(parent, "AddCoursewareWindow.png")
{
    ui.setupUi(this);

    QString regKey(QObject::tr("product"));
    char    szKey[MAX_PATH] = {0};
    Util::QStringToChar(regKey,szKey,MAX_PATH);

    if( LangSet::loadLangSet(szKey) == LangSet::LANG_ENG)
    {
        ui.pushButton_localFile->setStyleSheet(
            QString("QPushButton{width: 90px;height: 90px;background-color: none;border-image: url(:/res/res/image/default/addLoaclFile_normal_en.png);}"
            "QPushButton:hover{background-color: none;border-image: url(:/res/res/image/default/addLoaclFile_hover.png);}"
            "QPushButton:pressed{background-color: none;border-image: url(:/res/res/image/default/addLoaclFile_pressed.png);}"));

        ui.pushButton_whiteBoard->setStyleSheet(
            QString("QPushButton{width: 90px;height: 90px;background-color: none;border-image: url(:/res/res/image/default/addWhiteBoard_normal_en.png);}"
            "QPushButton:hover{background-color: none;border-image: url(:/res/res/image/default/addWhiteBoard_hover.png);}"
            "QPushButton:pressed{background-color: none;border-image: url(:/res/res/image/default/addWhiteBoard_pressed.png);}"));

        ui.pushButton_skyFile->setStyleSheet(
            QString("QPushButton{width: 90px;height: 90px;background-color: none;border-image: url(:/res/res/image/default/addSkyFile_normal_en.png);}"
            "QPushButton:hover{background-color: none;border-image: url(:/res/res/image/default/addSkyFile_hover.png);}"
            "QPushButton:pressed{background-color: none;border-image: url(:/res/res/image/default/addSkyFile_pressed.png);}"));
    }
}

AddCoursewareWindow::~AddCoursewareWindow()
{

}

void AddCoursewareWindow::addSkyFileBtnClicked()
{
    SkyFileDialog skyFileDlg;
    skyFileDlg.exec();
}

void AddCoursewareWindow::addLoacalFileBtnClicked()
{
	/* xiewb 2018.09.28*/
//     if(!m_showAddTipDlg)
//     {
//         this->hide();
//         
//         AddCoursewareTipDlg* tipDlg = new AddCoursewareTipDlg(ClassRoomDialog::getInstance());
//         if(tipDlg)
//         {
//             m_showAddTipDlg = true;
//             tipDlg->exec();
// 
//             delete tipDlg;
//             tipDlg = NULL;
//         }
//     }

    int nCount = CoursewareTaskMgr::getInstance()->GetCoursewareCount() +
                 WhiteBoardDataMgr::getInstance()->GetWhiteboardCount();
    
    if (nCount >= MAX_COURSEWARE_AND_WB)
    {
        QString infoText = QString(tr("CousewareAndWbOutMaxCount").arg(MAX_COURSEWARE_AND_WB));
        C8MessageBox msgBox(C8MessageBox::Warning, QString(tr("info")), infoText);
        msgBox.exec();
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Add Courseware"), "", 
        tr("All Files(*.*);;\
           Images (*.bmp *.png *.jpg *.jpge);;\
           Microsoft Word (*.doc *.docx);;\
           Microsoft Exce (*.xls *.xlsx);;\
           PowerPoint File (*.ppt *.pptx);;\
           Portable Document Format (*.pdf);;\
           Audio File(*.mp3 *.wav);;\
           Media File(*.avi *.flv *.mp4 *.rmvb)"));
    
    wchar_t wszFile[MAX_PATH] = {0};
    wcscpy(wszFile, wstring((wchar_t*)(fileName).unicode()).data());

    if (!fileName.isEmpty())
    {
        int nType = GetCoursewareFileType(wszFile);
        if (COURSEWARE_UNKNOWN == nType || COURSEWARE_TEXT == nType)
        {
            wchar_t szError[MAX_PATH] = {0};
            GetErrorInfo(szError, COURSEWARE_ERR_TYPE);
            C8MessageBox msgBox(C8MessageBox::Question, QString(tr("info")), QString::fromWCharArray(szError));
            if (msgBox.exec())
            {
                addLoacalFileBtnClicked();
            }
            return;
        }
        int nError = 0;
        if (!CoursewareTaskMgr::getInstance()->AddCourseBySource(wszFile, &nError))
        {
            //Ìí¼Ó¿Î¼þÊ§°Ü
            wchar_t szError[MAX_PATH] = {0};
            GetErrorInfo(szError, nError);
            C8MessageBox msgBox(C8MessageBox::Information, QString(tr("info")), QString::fromWCharArray(szError));
            if (msgBox.exec())
            {
                addLoacalFileBtnClicked();
            }
        }
    }
}

void AddCoursewareWindow::addWhiteBoardBtnClicked()
{
    int nCount = CoursewareTaskMgr::getInstance()->GetCoursewareCount() +
        WhiteBoardDataMgr::getInstance()->GetWhiteboardCount();

    if (nCount >= MAX_COURSEWARE_AND_WB)
    {
        QString infoText = QString(tr("CousewareAndWbOutMaxCount").arg(MAX_COURSEWARE_AND_WB));
        C8MessageBox msgBox(C8MessageBox::Warning, QString(tr("info")), infoText);
        msgBox.exec();
        return;
    }

    if (ClassSeeion::GetInst()->_bBeginedClass)
    {   
        int nId = WhiteBoardDataMgr::getInstance()->getNewId();
        addwhiteboarddialog dlgAdd(nId);
        int retDlg = dlgAdd.exec();
		
		if(retDlg==QDialog::Accepted)
        {
            WhiteBoardDataMgr::getInstance()->AddWhiteboard(nId,dlgAdd.getName());
        }
    }
    else
    {
        C8MessageBox msgBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("NotStartClassNotAddWhiteboard")));
        msgBox.exec();
    }
}