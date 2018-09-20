#ifndef QPDF_PREVIEW_LIST_H
#define QPDF_PREVIEW_LIST_H

#include <QtWidgets/QListWidget>
#include <QtCore/QMap>
#include <QtGui/QPixmap>

typedef QVector<QImage>                 QImageList;
typedef QMap<QString,QImageList>        QPDFImageList;

class QPDFWnd;

class QPDFPagePreviewItem : public QWidget
{
    Q_OBJECT
public:
    QPDFPagePreviewItem(QWidget* parent,QImage& image,int pageIndex);

    QPixmap&    getItemPixmap();
    int         getPageIndex();
private:
    QPixmap  m_itemPixmap;
    int      m_pageIndex;
};

class QPDFPreviewList : public QListWidget
{
    Q_OBJECT
public:
    QPDFPreviewList(QWidget * parent);

    bool addPDFPreview(QPDFWnd * wndPDF);
    void delPDFPreview(QString& file);

public slots:
    void selPDFPage(const QString& file,int nIndex = 0);

protected slots:
    void addPDFPageImage(const QString& file,const QImage& image,int index);
    void doItemActivated (QListWidgetItem *);

protected:
    void addListItem(QImage image,int nIndex);
    
signals:
    void clickPreviewPage(const QString& file,int pageIndex);

protected:
    QPDFImageList       m_pdfImageList;
    QString             m_curShowFile;

    QMutex              m_mutexImageList;
};

#endif