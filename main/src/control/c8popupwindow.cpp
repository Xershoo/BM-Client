#include "c8popupwindow.h"
#include <QApplication>
#include <QBitmap>
#include <QPainter>
#include <QStyleOption>
#include "Env.h"

C8PopupWindow::C8PopupWindow(QWidget *parent, QString background)
    : QWidget(parent)
{
    m_background = background;    
	
	//setWindowOpacity(0);
    
	if (!m_background.isEmpty())
    {
        QPixmap pix;
        pix.load(Env::currentThemeResPath() + m_background, 0, Qt::AvoidDither| Qt::ThresholdDither | Qt::ThresholdAlphaDither);
        this->setFixedSize(pix.size());
        //resize();
        setMask(pix.mask());

        this->setAttribute(Qt::WA_TranslucentBackground);
        show();
        setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
        show();
        QPalette plt;
        plt.setBrush(QPalette::Window, QBrush(pix));
        setPalette(plt);
    }
    else
    {
        show();
        setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
        show();
    }
    
	m_inWindow = false;
    installEventFilter(this);
    
	//setWindowOpacity(1);
}

C8PopupWindow::~C8PopupWindow()
{

}

void C8PopupWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	
}

bool C8PopupWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this)
    {
        if (event->type() == QEvent::Enter)
        {
            m_inWindow = true;
        }
        if (event->type() == QEvent::Leave)
        {
            m_inWindow = false;
        }

        if (event->type() == QEvent::MouseButtonPress)
        {
            if (!m_inWindow)
            {	
                this->hide();
            }
        }
    }
    
    return QWidget::eventFilter(obj, event);
}
