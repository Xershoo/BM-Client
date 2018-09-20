#ifndef SKYFILELISTWIDGET_H
#define SKYFILELISTWIDGET_H

#include <QListWidget>
#include <QLabel>
#include <QVector>

class SkyFileListItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SkyFileListItemWidget(QWidget *parent, int type, QString fileName, QString downloadURL,bool select);
    ~SkyFileListItemWidget();

    void setDownloadURL(QString downloadURL) {m_downloadURL = downloadURL;}
    QString getDownloadURL() {return m_downloadURL;}

    void setFileName(QString fileName) {m_fileName = fileName;}
    QString getFileName() {return m_fileName;}

    void setType(int type) {m_type = type;}
    int getType() {return m_type;}

    void setSelect(bool select){m_labelCheck.setVisible(select);}
    bool getSelect(){return m_labelCheck.isVisible();}
private:
    int m_type;
    QString m_fileName;
    QString m_downloadURL;
    QLabel m_LabelIcon;
    QLabel m_labelFileName;
    QLabel m_labelCheck;
};

class SkyFileListWidget : public QListWidget
{
    Q_OBJECT

public:
    SkyFileListWidget(QWidget *parent);
    ~SkyFileListWidget();

    const QVector<SkyFileListItemWidget *>& getDownloadItems() {return m_selectedFiles;}
protected slots:
    void itemSelected(QListWidgetItem *);
private:
    QVector<SkyFileListItemWidget *> m_selectedFiles;
};

#endif // SKYFILELISTWIDGET_H
