#ifndef QIMAGESHOW_H
#define QIMAGESHOW_H

#include <QWidget>
#include <QImage>

class QImageShow : public QWidget
{
    Q_OBJECT

public:
    QImageShow(QWidget *parent = 0);
    ~QImageShow();

    bool openFile(QString filePath);
    void closeFile();

	void zoomIn();
	void zoomOut();
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

	QRect calcImagePaintRect(QRect& rectImage,int imageW, int imageH);
protected:
    bool        m_fileOpen;
    QString     m_fileName;
	QImage		m_imgFile;
	float		m_zoomRatio;
};

#endif // QIMAGESHOW_H
