#ifndef ACCOUNTITEM_H
#define ACCOUNTITEM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

class AccountItem;
class AccountListWidget : public QListWidget
{
    Q_OBJECT

public:

    AccountListWidget(QWidget *parent = 0);
    ~AccountListWidget();

    void removeBtnClicked(AccountItem*);
    void itemClicked(AccountItem*, int);
    void hidePopupWidget();
signals:
    void removeItem(AccountItem*);
    void accountListItemClicked(AccountItem*, int);
};


class AccountItem : public QWidget
{
    Q_OBJECT

public:
    AccountItem(QWidget *parent);
    ~AccountItem();

    void setItem(QListWidgetItem *item);
    QListWidgetItem* getItem();
protected:
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
    void mousePressEvent(QMouseEvent *event);
protected slots:
    void removeAccount();

private:
    QLabel m_label;
    QPushButton m_delBtn;
    AccountListWidget *m_listWidget;
    QListWidgetItem* m_item;
};

class CoursewareListWidget;
class CoursewareItem : public QWidget
{
    Q_OBJECT

public:
    CoursewareItem(QWidget *parent, bool bisDown = true);
    ~CoursewareItem();
   
    bool isDowning() { return m_bisDowning; }
    void setItem(QListWidgetItem *item);
    QListWidgetItem* getItem();
    void setDowning(bool bisDowning);
protected:
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
    void mousePressEvent(QMouseEvent *event);

    protected slots:
        void removeAccount();

private:
    QLabel m_label;
    QPushButton m_delBtn;
    CoursewareListWidget *m_listWidget;
    QListWidgetItem* m_item;
    bool m_bisDowning;
};

class CoursewareListWidget : public QListWidget
{
    Q_OBJECT

public:

    CoursewareListWidget();
    ~CoursewareListWidget();

    void removeBtnClicked(CoursewareItem*);
    void itemClicked(CoursewareItem*, int);
    void hidePopupWidget();
signals:
    void removeItem(CoursewareItem*);
    void accountListItemClicked(CoursewareItem*, int);
};

#endif // ACCOUNTITEM_H
