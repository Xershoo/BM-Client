#include "pictureviewer.h"

PictureViewer * PictureViewer::m_instance = NULL;

PictureViewer* PictureViewer::getInstance()
{
	if(NULL==m_instance){
		m_instance = new PictureViewer(NULL);
	}

	return m_instance;
}

void PictureViewer::freeInstance()
{
	if(NULL == m_instance)
	{
		return;
	}

	delete m_instance;
	m_instance = NULL;
}

PictureViewer::PictureViewer(QWidget *parent)
    : C8CommonWindow(parent,SHADOW_QT)
{
    ui.setupUi(this);
    connect(ui.label_picViewerImage, SIGNAL(clicked()), this, SLOT(closeBtnClicked()));

	resetContentsMargin(ui.gridLayout);
}

PictureViewer::~PictureViewer()
{

}

void PictureViewer::setTitleBarRect()
{
    QPoint pt = ui.widget_picViewerTitleBar->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui.widget_picViewerTitleBar->size());
}

void PictureViewer::openImage(QString path)
{
    QImage img(path);
     if (img.isNull())
         return;

    QSize size = img.size();
    this->resize(size + QSize(60, 60));
    
    ui.label_picViewerImage->clear();
    ui.label_picViewerImage->resize(QSize(1, 1));
    ui.label_picViewerImage->resize(size);
    ui.label_picViewerImage->setPixmap(QPixmap(path));
    ui.label_picViewerImage->update();
    this->show();
    qApp->processEvents();
    this->activateWindow();

	centerWindow();
    //qApp->processEvents();
}

void PictureViewer::closeBtnClicked()
{
    C8CommonWindow::close();
}