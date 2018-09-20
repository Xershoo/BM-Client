#ifndef ANIMATEDTEXTEDIT_H
#define ANIMATEDTEXTEDIT_H
#include <QTextEdit>
#include <QMovie>
#include <QHash>
#include <QUrl>
#include <QString>
#include <QPushButton>
#include <QDragEnterEvent>
#include "chatwindowtextedit.h"

class ChatWindowOutPutEdit : public ChatWindowTextEdit
{
        Q_OBJECT

public:
        ChatWindowOutPutEdit(QWidget * p = 0);
        // QTextEdit *textEdit;

        void addAnimation(const QUrl& url, const QString& fileName);
        void clear();

protected:
        //bool canInsertFromMimeData(const QMimeData *source) const override;
        //virtual void insertFromMimeData(const QMimeData *source) override;

private slots:
        void animate(int);
		
        //void dragEnterEvent(QDragEnterEvent *event);

private:
        QList<QUrl>     lstUrl;
        QList<QMovie *> lstMovie;
        QHash<QMovie*, QUrl> urls;
};

#endif	//ANIMATEDTEXTEDIT_H
