#include "chatwindowtextedit.h"
#include <QMouseEvent>
#include <QTextFrame>
#include <QAbstractTextDocumentLayout>
#include <QKeyEvent>
#include <QDebug>

#include "../define/chatwindowtextblockuserdata.h"
ChatWindowTextEdit::ChatWindowTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    setReadOnly(true);
}

ChatWindowTextEdit::~ChatWindowTextEdit()
{

}

void ChatWindowTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
	if(NULL == e)
	{
		return;
	}

    QTextEdit::ensureCursorVisible();
    QPoint pt = e->pos();
    QTextFrame *frame = document()->rootFrame();
    
    QTextCursor cutsor = cursorForPosition(e->pos());
    QTextBlock block = cutsor.block();
    for (auto it = block.begin(); it != block.end(); it++)
    {
        QTextFragment fragment = it.fragment();
        if (!fragment.isValid())
        {
			continue;
		}
            
		if (!fragment.charFormat().isImageFormat())
		{
			continue;
		}
            
        if (!fragment.contains(cutsor.position()))
        {
			continue;
		}
        
		QTextImageFormat imageFormat = fragment.charFormat().toImageFormat();
        QString imageName = imageFormat.name();        
		ChatWindowTextBlockUserData *userData = (ChatWindowTextBlockUserData*)block.userData();
        if(NULL == userData)
		{
			continue;
		}

		QString str = QString("userData: %1 \n").arg(userData->getData());
        append(str);
    }
}
