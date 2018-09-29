#include "qloginbttokendialog.h"
#include "setdebugnew.h"

QLoginBtTokenDialog * QLoginBtTokenDialog::m_instance = NULL;

QLoginBtTokenDialog* QLoginBtTokenDialog::getInstance()
{
    if(NULL==m_instance){
		m_instance = new QLoginBtTokenDialog(NULL);
	}

    return m_instance;
}

void QLoginBtTokenDialog::freeInstance()
{
	if(NULL == m_instance)
	{
		return;
	}

	delete m_instance;
	m_instance = NULL;
}

QLoginBtTokenDialog::QLoginBtTokenDialog(QWidget *parent)
    : C8CommonWindow(parent,SHADOW_AERO)
{
    ui.setupUi(this);
	resetContentsMargin(ui.horizontalLayout);
}

QLoginBtTokenDialog::~QLoginBtTokenDialog()
{

}

void QLoginBtTokenDialog::setTitleBarRect()
{
   
}