#include "skyfilelistwidget.h"
#include <QHBoxLayout>
SkyFileListItemWidget::SkyFileListItemWidget(QWidget *parent, int type, QString fileName, QString downloadURL,bool select)
    : m_type(type), m_fileName(fileName), m_downloadURL(downloadURL), QWidget(parent)
{
    QString iconFileName = QString("skyFileItemIcon_%1").arg(type);
    m_LabelIcon.setObjectName(iconFileName);
    m_LabelIcon.setMinimumSize(20, 20);

    m_labelFileName.setText(fileName);
    m_labelFileName.setObjectName("skyFileItemFileName");
    m_labelFileName.setFixedWidth(470);

    m_labelCheck.setObjectName("skyFileItemCheckIcon");
    m_labelCheck.setMinimumSize(30, 30);

    m_labelCheck.setVisible(select);
    if(select)
    {
        m_labelCheck.setEnabled(false);
    }

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(&m_LabelIcon);
    hLayout->addWidget(&m_labelFileName);
    hLayout->addWidget(&m_labelCheck);
    hLayout->addStretch();
    hLayout->setContentsMargins(15, 5, 5, 5);
    hLayout->setSpacing(15);
    this->setLayout(hLayout);

}

SkyFileListItemWidget::~SkyFileListItemWidget()
{

}

//////////////////////////////////////////////////////////////////////////

SkyFileListWidget::SkyFileListWidget(QWidget *parent)
    : QListWidget(parent)
{
    connect(this, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(itemSelected(QListWidgetItem *)));
}

SkyFileListWidget::~SkyFileListWidget()
{

}

void SkyFileListWidget::itemSelected(QListWidgetItem *item)
{
    SkyFileListItemWidget *listItemWidget = qobject_cast<SkyFileListItemWidget*>(itemWidget(item));
    if (listItemWidget)
    {
        bool find = false;
        for (auto it = m_selectedFiles.begin(); it != m_selectedFiles.end(); it++)
        {
            if ((*it)->getDownloadURL() == listItemWidget->getDownloadURL())
            {
                m_selectedFiles.erase(it);
                listItemWidget->setSelect(false);
                return;
            }
        }
        m_selectedFiles.push_back(listItemWidget);
        listItemWidget->setSelect(true);
    }
}