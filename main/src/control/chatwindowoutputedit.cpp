#include "chatwindowoutputedit.h"
#include <QFile>
#include <QDebug>
#include <QVBoxLayout>
#include <QTime>
#include <QMimeData>
//#include <QDragEnterEvent>

ChatWindowOutPutEdit::ChatWindowOutPutEdit(QWidget * e)
    :ChatWindowTextEdit(e)
{
	setAcceptDrops(false);
	setContextMenuPolicy (Qt::NoContextMenu);
    setAcceptRichText(false);
}

void ChatWindowOutPutEdit::addAnimation(const QUrl& url, const QString& fileName)
{
        QFile *file =new QFile(fileName);
        if(!file->open(QIODevice::ReadOnly)){
            qDebug()<<" open err";
        }

         if(lstUrl.contains(url)){ //同一个gif 使用同一个movie
             return;
         }else{
            lstUrl.append(url);
         }

        QMovie* movie = new QMovie(this);
        movie->setFileName(fileName);
        movie->setCacheMode(QMovie::CacheNone);


        lstMovie.append(movie);   //获取该movie,以便删除
        urls.insert(movie, url);

        //换帧时刷新
        connect(movie, SIGNAL(frameChanged(int)), this, SLOT(animate(int)));
        movie->start();
        file->close();
        delete file;

}
void ChatWindowOutPutEdit::animate(int a)
{
        if (QMovie* movie = qobject_cast<QMovie*>(sender()))
        {
               textCursor().document()->addResource(QTextDocument::ImageResource, urls.value(movie), movie->currentPixmap());
               //document()->addResource(QTextDocument::ImageResource, urls.value(movie), movie->currentPixmap());
               qDebug() << "****************" << lineWrapColumnOrWidth() << "**************";
               QTextEdit::setLineWrapColumnOrWidth(lineWrapColumnOrWidth()); // ..刷新显示		   
        }
}

void ChatWindowOutPutEdit::clear()
{
    for (int i = 0; i < lstMovie.size(); i++)
    {
        lstMovie[i]->stop();
    }
    lstMovie.clear();
    lstUrl.clear();
    urls.clear();
    QTextEdit::clear();
}
/*
bool ChatWindowOutPutEdit::canInsertFromMimeData(const QMimeData *source) const
{
    if (source->hasImage())
    {
        return false;
    }
    if (source->hasUrls())
    {
        return false;
    }
    return true;
}

void ChatWindowOutPutEdit::insertFromMimeData(const QMimeData *source)
{
    QTextEdit::insertFromMimeData(source);
}


void ChatWindowOutPutEdit::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
	{
		//QString url = event->mimeData()->urls();
	}

	return QTextEdit::dragEnterEvent(event);
//	if(event->type())
}
*/