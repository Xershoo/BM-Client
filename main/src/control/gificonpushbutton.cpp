#include "gificonpushbutton.h"
#include <QMouseEvent>
GifIconPushButton::GifIconPushButton(QWidget *parent)
    : m_pressed(false), QPushButton(parent)
{
    this->setMouseTracking(true);
    this->installEventFilter(this);    
}

GifIconPushButton::~GifIconPushButton()
{
    m_movie->stop();
}

void GifIconPushButton::resetMovie(QString path)
{
    if (m_movie)
    {
        m_movie->stop();
        m_movie->setFileName(path);
        m_movie->start();
    }
}
void GifIconPushButton::setIconPath(QString normal, QString hover, QString pressed)
{
    m_iconNormal = normal;
    m_iconHover = hover;
    m_iconPressed = pressed;

    m_movie = new QMovie(this);
    connect(m_movie, SIGNAL(frameChanged(int)), this, SLOT(updateIcon(int)));
    resetMovie(m_iconNormal);
    QPixmap pix = m_movie->currentPixmap();
    setIconSize(pix.size());
    setIcon(QIcon(pix));
    if (m_movie->loopCount() == -1)
        connect(m_movie, SIGNAL(finished()), m_movie, SLOT(start()));
}

void GifIconPushButton::updateIcon(int)
{
    this->setIcon(QIcon(m_movie->currentPixmap()));
}

bool GifIconPushButton::eventFilter(QObject *o, QEvent *e)
{
    if (o != this)
        return QPushButton::eventFilter(o, e);

    if (e->type() == QEvent::MouseMove)
    {

    }

    if (e->type() == QEvent::Leave)
    {
        resetMovie(m_iconNormal);
    }

    if (e->type() == QEvent::Enter)
    {
        resetMovie(m_iconHover);
    }

    return QPushButton::eventFilter(o, e);
}

void GifIconPushButton::mousePressEvent(QMouseEvent *e)
{
	resetMovie(m_iconPressed);
	QPushButton::mousePressEvent(e);    
}

void GifIconPushButton::mouseReleaseEvent(QMouseEvent *e)
{
	resetMovie(m_iconNormal);
    QPushButton::mouseReleaseEvent(e);
}