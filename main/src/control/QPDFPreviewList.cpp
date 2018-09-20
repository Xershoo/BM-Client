#include "QPDFPreviewList.h"
#include "pdfwnd.h"
#include <QtWidgets/QLabel>
#include <QVBoxLayout>
#include "macros.h"

//////////////////////////////////////////////////////////////////////////
//QPDFPagePreviewItem

QPDFPagePreviewItem::QPDFPagePreviewItem(QWidget* parent,QImage& image,int pageIndex) : QWidget(parent),
     m_pageIndex(pageIndex)
{
    QString strText = QString(tr("pageNumberText")).arg(pageIndex);

    QLabel * labelText = new QLabel(this);
    labelText->setStyleSheet("color:#827777;");
    labelText->setText(strText);
    labelText->setMinimumHeight(20);
    labelText->setAlignment(Qt::AlignCenter);
    labelText->setBaseSize(QSize(40, 15));

    QLabel * labelImage = new QLabel(this);
    labelImage->setMinimumHeight(54);
    QPixmap pixmap(72,54);
    pixmap = QPixmap::fromImage(image.scaled(72,54));
    labelImage->setPixmap(pixmap);
    //labelImage->setPixmap(QPixmap::fromImage(image));
    labelImage->setAlignment(Qt::AlignCenter);
    labelImage->setBaseSize(QSize(72,54));

    QVBoxLayout *hLayout = new QVBoxLayout;
    hLayout->addWidget(labelImage);
    hLayout->addWidget(labelText);    
    hLayout->setContentsMargins(2, 2, 2, 2);
    hLayout->setSpacing(5);
    
    this->setLayout(hLayout);
}

QPixmap& QPDFPagePreviewItem::getItemPixmap()
{
    return m_itemPixmap;
}

int QPDFPagePreviewItem::getPageIndex()
{
    return m_pageIndex;
}

//////////////////////////////////////////////////////////////////////////
//QPDFPreviewList

QPDFPreviewList::QPDFPreviewList(QWidget * parent) : QListWidget(parent)
    ,m_curShowFile("")
{
    this->setObjectName(QString("listWidget_PdfPreview"));
}

bool QPDFPreviewList::addPDFPreview(QPDFWnd * wndPDF)
{
    if(NULL == wndPDF)
    {
        return false;
    }

    QImageList imageList;
    QString    filePDF = wndPDF->getFile();
    if(!filePDF.isEmpty())
    {
        QMutexLocker    autoLock(&m_mutexImageList);
        m_pdfImageList.insert(filePDF,imageList);
    }

    connect(wndPDF,SIGNAL(addPdfPreviewPage(const QString&,const QImage&,int)),this,SLOT(addPDFPageImage(const QString&,const QImage&,int)));
    connect(wndPDF,SIGNAL(showPdfPage(const QString&,int)),this,SLOT(selPDFPage(const QString&,int)));
    connect(this,SIGNAL(clickPreviewPage(const QString&,int)),wndPDF,SLOT(gotoPage(const QString&,int)));
    connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(doItemActivated(QListWidgetItem*)));

    return true;
}

void QPDFPreviewList::delPDFPreview(QString& file)
{
    QMutexLocker    autoLock(&m_mutexImageList);

    QPDFImageList::Iterator itr = m_pdfImageList.find(file);
    if(itr == m_pdfImageList.end())
    {
        return;
    }

    m_pdfImageList.erase(itr);

    if(m_curShowFile.compare(file,Qt::CaseInsensitive) == 0)
    {
        this->clear();
        m_curShowFile.clear();
    }
}

void QPDFPreviewList::selPDFPage(const QString& file,int nIndex /* = 0 */)
{
    if(m_curShowFile.compare(file,Qt::CaseInsensitive) == 0)
    {
        if (nIndex > -1)
        {
            this->setCurrentRow(nIndex);
        }
        return;
    }

    m_curShowFile = file;

    QMutexLocker    autoLock(&m_mutexImageList);

    this->clear();

    QPDFImageList::Iterator itr = m_pdfImageList.find(file);
    if(itr == m_pdfImageList.end())
    {
        return;
    }

    QImageList listImage = *itr;    
    if(listImage.empty())
    {
        return;
    }

    for(int i=0;i<listImage.count();i++)
    {
        QImage itemImage = listImage.at(i);
        addListItem(itemImage,i+1);
    }

    if (nIndex > -1)
    {
        this->setCurrentRow(nIndex);
    }
}

void QPDFPreviewList::addPDFPageImage(const QString& file,const QImage& image,int index)
{
    if(file.isEmpty())
    {
        return;
    }

    QMutexLocker    autoLock(&m_mutexImageList);

    QImage imageItem(image);
    QPDFImageList::Iterator itr = m_pdfImageList.find(file);    
    if(itr != m_pdfImageList.end())
    {
        QImageList& listImage = *itr;        
        listImage.push_back(imageItem);        
    }
    else
    {
        QImageList imageList;        
        imageList.push_back(imageItem);
        m_pdfImageList.insert(file,imageList);
    }
    
    if(m_curShowFile.compare(file,Qt::CaseInsensitive) != 0)
    {
        return;
    }

    int nCount = this->count();    
    addListItem(image,nCount+1);

    return;
}

void QPDFPreviewList::doItemActivated(QListWidgetItem * item)
{
    if (item)
    {
        QPDFPagePreviewItem* imageItem = qobject_cast<QPDFPagePreviewItem*>(this->itemWidget(item));
        if (imageItem)
        {
            int npage = imageItem->getPageIndex();
            emit clickPreviewPage(m_curShowFile, npage-1);
        }
    }
}

void QPDFPreviewList::addListItem(QImage image,int nIndex)
{    
    QListWidgetItem *       listItem = NULL;
    QPDFPagePreviewItem*    imageItem = NULL;

    bool    addItem = false;
    do 
    {
        if(this->count() >= nIndex)
        {
            break;
        }

        listItem = new QListWidgetItem;
        if(NULL == listItem)
        {
            break;
        }

        imageItem = new QPDFPagePreviewItem(this,image,nIndex);
        if(NULL == imageItem)
        {
            break;
        }

        this->addItem(listItem);
        this->setItemWidget(listItem, imageItem);
        listItem->setSizeHint(QSize(72, 80));

        addItem = true;
    } while (false);

    if(!addItem)
    {
        SAFE_DELETE(listItem);
        SAFE_DELETE(imageItem);
    }
}