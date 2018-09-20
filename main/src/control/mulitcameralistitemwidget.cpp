#include "mulitcameralistitemwidget.h"
#include "common/Env.h"
#include <QPixmap>

MulitCameraListItemWidget::MulitCameraListItemWidget(int type, QString name, int seat,int id,QWidget *parent /* = 0 */)
    : QWidget(parent)
	, m_cameraSeat(seat)
	, m_cameraId(id)
	, m_cameraType(type)
{
    ui.setupUi(this);
    connect(ui.label_typeIcon, SIGNAL(clicked()), this, SLOT(labelClicked()));
    connect(ui.label_cameraName, SIGNAL(clicked()), this, SLOT(labelClicked()));
    connect(ui.label_number, SIGNAL(clicked()), this, SLOT(labelClicked()));
		
    if (type == 0)
    {
		//ÉãÏñÍ·
		ui.label_typeIcon->setPixmap(QPixmap(Env::currentThemeResPath() + "web_CrmeraIcon.png"));        
    }
    else
    {
		//ÊÖ»ú
		ui.label_typeIcon->setPixmap(QPixmap(Env::currentThemeResPath() + "web_PhoneIcon.png"));
    }

    ui.label_cameraName->setText(name);

    setCameraSeat(seat);
}

MulitCameraListItemWidget::~MulitCameraListItemWidget()
{

}

void MulitCameraListItemWidget::labelClicked()
{
    emit itemClicked(this);
}

void MulitCameraListItemWidget::setCameraSeat(int seat)
{
	seat ++ ;
    if (seat > 0)
    {
        QString fileName = Env::currentThemeResPath() + QString("web_crmera%1.png").arg(seat);
        ui.label_number->setPixmap(QPixmap(fileName));
        ui.label_number->show();
    }
    else
    {
        ui.label_number->hide();
    }
}