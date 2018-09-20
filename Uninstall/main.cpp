#include "MainDlg.h"
#include <QtWidgets/QApplication>
#include <QtCore/QFile>
#include <QtCore/QTranslator>
#include <QTextCodec>
#include "Util.h"
#include "SingleApp.h"
#include "lang.h"

bool g_uninstallFinished = false;
char g_szInstallPath[1024] = { 0 };

QTranslator* g_tran = NULL;

void setAppTranslator(int langId)
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
        g_tran->load(":/lang/res/lang/uninstall_en.qm");
    }
    else
    {
        g_tran->load(":/lang/res/lang/uninstall_zh.qm");
    }

    qApp->installTranslator(g_tran);    
}

int main(int argc, char *argv[])
{
    //≈–∂œ «∑Ò”––∂‘ÿ≥Ã–Ú «∑Ò¥Ê‘⁄
    SingleApp app(argc, argv);
    if(app.isRunning())
    {
        return 0;
    }

    int langId = LangSet::getSystemLang();
    setAppTranslator(langId);
    	
    QString regKey(QObject::tr("product"));
    std::string  strKey=regKey.toStdString();
    int regId = LangSet::loadLangSet(strKey.c_str());
    if(langId != regId)
    {
        setAppTranslator(regId);
    }
    
	QFile qss(":/style/res/style/uninstall.qss");
	if(qss.open(QFile::ReadOnly))
	{
		app.setStyleSheet(qss.readAll());
		qss.close();
	}

    MainDlg dlgMain;
    app.m_widget = &dlgMain;

    dlgMain.show();

    app.exec();
    
    if(g_uninstallFinished)
    {
        QString qstrBatFile(QObject::tr("product"));
        qstrBatFile += QString("Clear.bat");
        
        std::string strBatFile = qstrBatFile.toStdString();
        exitAndDelSel(g_szInstallPath,strBatFile.c_str());
    }

    return 0;
}
