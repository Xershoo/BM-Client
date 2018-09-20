#include "accountitem.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QListWidgetItem>
#include "common/Env.h"

#include "setdebugnew.h"

AccountListWidget::AccountListWidget(QWidget *parent): QListWidget(parent)
{
}

AccountListWidget::~AccountListWidget()
{

}

void AccountListWidget::removeBtnClicked(AccountItem* item)
{
    emit removeItem(item);
}

void AccountListWidget::itemClicked(AccountItem* item, int row)
{
    emit accountListItemClicked(item, row);
}

AccountItem::AccountItem(QWidget *parent)
    : m_listWidget(NULL), m_item(NULL), QWidget(parent)
{
    m_delBtn.setObjectName("delBtn");
    connect(&m_delBtn, SIGNAL(pressed()), this, SLOT(removeAccount()));
    m_label.setText("");
    QHBoxLayout *main_layout = new QHBoxLayout();
    main_layout->addWidget(&m_label);
    main_layout->addStretch();
    main_layout->addWidget(&m_delBtn);
    main_layout->setContentsMargins(5, 5, 5, 5);
    main_layout->setSpacing(5);
    this->setLayout(main_layout);
    m_delBtn.setVisible(false);
}

AccountItem::~AccountItem()
{

}


void AccountItem::setItem(QListWidgetItem *item)
{
    Q_ASSERT(item);
    m_item = item;
    m_listWidget = qobject_cast<AccountListWidget *>(m_item->listWidget());
}

QListWidgetItem* AccountItem::getItem()
{
    return m_item;
}

void AccountItem::enterEvent(QEvent * event)
{   
    /*
    QObject *obj = parent();
    if (m_listWidget && m_item)
    {
        int row = m_listWidget->row(m_item);
        int height = m_listWidget->sizeHintForRow(row);
        int width = m_listWidget->sizeHintForColumn(-1);
        m_item->setSizeHint(QSize(width, height + 10));
    }
    */
    m_delBtn.setVisible(true);
}

void AccountItem::leaveEvent(QEvent * event)
{
    /*
    if (m_listWidget && m_item)
    {
        int row = m_listWidget->row(m_item);
        int height = m_listWidget->sizeHintForRow(row);
        int width = m_listWidget->sizeHintForColumn(-1);
        m_item->setSizeHint(QSize(width, height - 10));
    }
    */
    m_delBtn.setVisible(false);
}

void AccountItem::mousePressEvent(QMouseEvent *event)
{
    if (m_listWidget)
    {
        int row = m_listWidget->row(m_item);
        m_listWidget->accountListItemClicked(this, row);
    }    
}

void AccountItem::removeAccount()
{
    if (m_listWidget)
    {
        m_listWidget->removeBtnClicked(this);
    }
}


CoursewareItem::CoursewareItem(QWidget *parent, bool bisDown)
    : m_listWidget(NULL), m_item(NULL), QWidget(parent)
{
    if (bisDown)
    {
        m_delBtn.setObjectName("downBtn");
    }
    else
    {
        m_delBtn.setObjectName("delBtn");
    }
    m_bisDowning = bisDown;
    connect(&m_delBtn, SIGNAL(pressed()), this, SLOT(removeAccount()));
    m_label.setText("");
    QHBoxLayout *main_layout = new QHBoxLayout();
    main_layout->addWidget(&m_label);
    main_layout->addStretch();
    main_layout->addWidget(&m_delBtn);
    main_layout->setContentsMargins(5, 5, 5, 5);
    main_layout->setSpacing(5);
    this->setLayout(main_layout);
    m_delBtn.setVisible(false);
}

CoursewareItem::~CoursewareItem()
{

}

void CoursewareItem::setDowning(bool bisDowning)
{
    m_bisDowning = bisDowning; 
    if (m_bisDowning)
    {        
        QString iconPath = QString("%1%2").arg(Env::currentThemeResPath()).arg("downing.png");
        m_delBtn.setIcon(QIcon(iconPath));
    }
    else
    {        
        QString iconPath = QString("%1%2").arg(Env::currentThemeResPath()).arg("del_btn.png");
        m_delBtn.setIcon(QIcon(iconPath));
    }
}

void CoursewareItem::setItem(QListWidgetItem *item)
{
    Q_ASSERT(item);
    m_item = item;
    m_listWidget = qobject_cast<CoursewareListWidget *>(m_item->listWidget());
}

QListWidgetItem* CoursewareItem::getItem()
{
    return m_item;
}

void CoursewareItem::enterEvent(QEvent * event)
{   
    QObject *obj = parent();
    if (m_listWidget && m_item)
    {
        int row = m_listWidget->row(m_item);
        int height = m_listWidget->sizeHintForRow(row);
        int width = m_listWidget->sizeHintForColumn(-1);
        m_item->setSizeHint(QSize(width, height + 0));
    }
    //暂时取消课件删除功能
    //m_delBtn.setVisible(true);
}

void CoursewareItem::leaveEvent(QEvent * event)
{
    if (m_listWidget && m_item)
    {
        /*
        int row = m_listWidget->row(m_item);
        int height = m_listWidget->sizeHintForRow(row);
        int width = m_listWidget->sizeHintForColumn(-1);
        m_item->setSizeHint(QSize(width, height - 0));
        */
    }
    m_delBtn.setVisible(false);
}

void CoursewareItem::mousePressEvent(QMouseEvent *event)
{
    if (m_listWidget)
    {
        int row = m_listWidget->row(m_item);
        m_listWidget->accountListItemClicked(this, row);
    }    
}

void CoursewareItem::removeAccount()
{
    if (m_listWidget && !m_bisDowning)
    {
        m_listWidget->removeBtnClicked(this);
    }
}

CoursewareListWidget::CoursewareListWidget()
{
}

CoursewareListWidget::~CoursewareListWidget()
{

}

void CoursewareListWidget::removeBtnClicked(CoursewareItem* item)
{
    emit removeItem(item);
}

void CoursewareListWidget::itemClicked(CoursewareItem* item, int row)
{
    emit accountListItemClicked(item, row);
}