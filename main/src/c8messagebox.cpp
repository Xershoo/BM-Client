#include "c8messagebox.h"

C8MessageBox::C8MessageBox(IconType icontype, QString title, QString detial)
    : m_iconType(icontype), m_title(title), m_detail(detial)
{
    ui.setupUi(this);

	resetContentsMargin(ui.gridLayout);

	ui.label_msgBoxTitle->setText(title);
    ui.label_detail->setText(detial);
    ui.pushButton_Cancel->setVisible(false);
    setIconType(icontype);
}

C8MessageBox::~C8MessageBox()
{

}

void C8MessageBox::setTitleBarRect()
{
    m_titlRect = ui.verticalLayout_titleBar->geometry();
}

int C8MessageBox::exec()
{
    return QDialog::exec();
}

void C8MessageBox::acceptBtnClicked()
{
    QDialog::accept();
}

void C8MessageBox::rejectBtnClicked()
{
    QDialog::reject();
}

void C8MessageBox::setIconType(IconType type)
{
    switch (type)
    {
    case NoIcon:
        {
            ui.label_icon->setVisible(false);
        }
        break;
    case Information:
        {
            ui.label_icon->setObjectName("C8MSGBOX_ICONINFO");
            ui.label_icon->setVisible(true);            
        }
        break;
    case Warning:
        {
            ui.label_icon->setObjectName("C8MSGBOX_ICONWARNING");
            ui.label_icon->setVisible(true);
        }
        break;
    case Critical:
        {
            ui.label_icon->setObjectName("C8MSGBOX_ICONCRITICAL");
            ui.label_icon->setVisible(true);
        }
        break;
    case Question:
        {
            ui.label_icon->setObjectName("C8MSGBOX_ICONQUESTION");
            ui.label_icon->setVisible(true);
            ui.pushButton_Cancel->setVisible(true);
        }
        break;
    }
    m_iconType = type;
}

C8MessageBox::IconType C8MessageBox::getIconType()
{
    return m_iconType;
}

void C8MessageBox::setTitle(QString title)
{
    ui.label_msgBoxTitle->setText(title);
}

QString C8MessageBox::getTitle()
{
    return ui.label_msgBoxTitle->text();
}

void C8MessageBox::setDetail(QString detail)
{
    ui.label_detail->setText(detail);
}

QString C8MessageBox::getDetail()
{
    return m_detail;
}