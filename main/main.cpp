#include "src/logindialog.h"
#include <QtWidgets/QApplication>
#include <QtCore/qfile.h>
#include <QTranslator>
#include "BizInterface.h"
#include "runtime.h"
#include <qDebug>
#include <QMessageBox>
#include "lobbydialog.h"
#include "classroomdialog.h"
#include "systemtray/systemtray.h"
#include "pdf/PDFShowUtil.h"
#include "src/define/publicdefine.h"
#include "c8messagebox.h"
#include "src/LoginByToken/LoginByToken.h"
#include "src/LoginByToken/qloginbttokendialog.h"
#include "SingleApp/SingleApp.h"

#include <Windows.h>
#include "common/GTBugReportLib.h"
#include "common/Env.h"
#include "copyUpdate/copyUpdate.h"
#include "common/Log.h"
#include "setdebugnew.h"
#include "media/MediaPublishMgr.h"
#include "session/classsession.h"
#include "token/LoginTokenMgr.h"
#include "token/UploadTokenMgr.h"
#include "common/Config.h"
#include "QClassWebDialog.h"

#include "lang.h"

//#include <vld.h>

QTranslator* g_tran = NULL;

static void setAppTranslator(int langId)
{	
    bool reload = false;
    if (NULL!=g_tran)
    {
        qApp->removeTranslator(g_tran);
        delete g_tran;
        g_tran = NULL;

        reload = true;
    }

    g_tran = new QTranslator(NULL);
    if(NULL==g_tran)
    {
        return;
    }

    if(langId == LangSet::LANG_ENG)
    {
        g_tran->load(":/lang/res/lang/lang_en.qm");
    }
    else
    {
        g_tran->load(":/lang/res/lang/lang_ch.qm");
    }

    qApp->installTranslator(g_tran);    
}
 
static void freeAppTranslator()
{
	if(NULL == g_tran)
	{
		return;
	}

	delete g_tran;
	g_tran = NULL;
}

SystemTray* g_systemTray = NULL;
void loadStyleSheet(const QString &sheetName)
{
    QFile file(":BMClass/qss/" + sheetName.toLower() + ".qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());

    qApp->setStyleSheet(styleSheet);
}

static class CErrHandler
{
public:
    CErrHandler()
    {
        QString qstrProduct(QObject::tr("product"));
        char    szProduct[MAX_PATH] = {0};
        Util::QStringToChar(qstrProduct,szProduct,MAX_PATH);

        Env::GetGTVersion();
        SetExceptionFilter(GT_DEBUG_MODE_AUTO, szProduct, Env::chMainVer,Env::chSubVer,Env::chExVer,Env::chBuildNO);
    }

    ~CErrHandler()
    {
        UnSetExceptionFilter();
    }

private:
    HMODULE handle;
}errHandler;

LoginDialog* g_loginDlg = NULL;
SingleApp *g_singApp = NULL;

int main(int argc, char *argv[])
{
#ifdef _DEBUG 
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    Config::getConfig();

	g_singApp = new SingleApp(argc, argv);
    loadStyleSheet("coffee");
    
    int langId = LangSet::getSystemLang();
    setAppTranslator(langId);

    QString regKey(QObject::tr("product"));
    char    szKey[MAX_PATH] = {0};
    Util::QStringToChar(regKey,szKey,MAX_PATH);
   
    int regId = LangSet::loadLangSet(szKey);
    if(langId != regId)
    {
        setAppTranslator(regId);
    }
	
    if (g_singApp->isRunning())
    {
        C8MessageBox msgBox(C8MessageBox::Warning, QString(("About")), QString(QObject::tr("The application already running")));        
        msgBox.exec();
        return 0;
    }

    //拷贝更新程序
    CCopyUpdateFile copyUpdate;
    copyUpdate.StartCopy(QCoreApplication::applicationDirPath());

    ::initBizLibray();
    run_time::start();
	CMediaPublishMgr::getInstance();
    QPDFThread::createPDFThread();
    g_systemTray = new SystemTray;

	biz::GetBizInterface()->Start(Config::getConfig()->m_loginServIp.c_str(),Config::getConfig()->m_loginServPort.c_str());

	g_loginDlg = new LoginDialog;
    g_singApp->m_widget = g_loginDlg;

    char token[1024] = {0};
    if (argc > 1)
    {
        strcpy_s(token, argv[argc-1]);
    }
    else
    {
        strcpy_s(token, "updater_start");
    }
    WebStartParam webparam;
    if (IsStartByWeb(token, webparam))
    {
        SetLoginByWebInfo(webparam);
        g_loginDlg->hide();

        QLoginBtTokenDialog::getInstance()->show();
        Sleep(1000);    //等待资源加载，防止UI一直卡在进入课堂页面
        g_loginDlg->loginByTokenUid(webparam.acc, webparam.token, webparam.uid);
    }
    else
    {
        g_loginDlg->show();
    }
	
    int ret = g_singApp->exec();
	CMediaPublishMgr::freeInstance();
    ClassRoomDialog::freeInstance();
    CWebDlgMgr::freeInstance();
    ClassSeeion::FreeInst();
    CLoginTokenMgr::FreeInstance();
    CUploadTokenMgr::Release();
    
    LobbyDialog::freeInstance();
	delete g_loginDlg;
	g_loginDlg = NULL;

    ::unitBizLibray();
    run_time::stop();
	QPDFThread::destroyPDFThread();
	delete g_systemTray;
    
	freeAppTranslator();
	Config::freeConfig();

    if (g_singApp)
    {
        delete g_singApp;
        g_singApp = NULL;
    }
        
    freeLog();

    return ret;
}
