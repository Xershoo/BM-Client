#include <QApplication>
#include <QTranslator>
#include "dialog.h"
#include "env.h"
#include "msgdlg.h"
#include "lang.h"

Dialog * g_mainDlg = NULL;
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
		g_tran->load(":/lang/res/lang/setup_en.qm");
	}
	else
	{
		g_tran->load(":/lang/res/lang/setup_zh.qm");
	}

	qApp->installTranslator(g_tran);
	if(reload&&g_mainDlg)
	{
		g_mainDlg->retranslateUI();
	}
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int langId = LangSet::getSystemLang();
	setAppTranslator(langId);

    QFile qss(":/style/res/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly))
    {
        a.setStyleSheet(qss.readAll());
        qss.close();
    }

    QString exeFile(QObject::tr("PragramFile"));
    if(!ENV::processIsRun(exeFile))
	{
		MsgDlg msgDlg;
		msgDlg.exec();
	}
	else
	{
		g_mainDlg = new Dialog();
		g_mainDlg->exec();
	}
	return 0;
}
