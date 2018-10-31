#include "CourseClassItem.h"
#include <QPainter>
#include <QBitmap>
#include <QFile>
#include "lobbydialog.h"
#include "common/Env.h"
#include "common/Util.h"
#include "common/macros.h"

CourseClassItem::CourseClassItem(QWidget* parent):C8CommonWindow(parent,SHADOW_QT)
	,m_classId(0)
	,m_courseId(0)
	,m_state(class_unknow)
	,m_imageName("")
	,m_pixmap(NULL)
{
	ui.setupUi(this);
	setItemRound();

	connect(ui.pushButton_classCtrl,SIGNAL(clicked()),this,SLOT(enterClass()));
	ui.label_classImage->installEventFilter(this);
	ui.label_classImage->setMouseTracking(true);
}

CourseClassItem::~CourseClassItem()
{
	SAFE_DELETE(m_pixmap);
}

void CourseClassItem::setItemRound()
{
	QPainterPath path;
	QRectF rect = this->rect();
	path.addRoundRect(rect, 5, 5);
	QPolygon polygon= path.toFillPolygon().toPolygon();
	QRegion region(polygon);
	this->setMask(region);
}

void CourseClassItem::enterClass()
{
	if (m_courseId <= 0 || m_classId <=0){
		return;
	}

	if(m_state == class_unknow || m_state == class_end){
		return;
	}

	LobbyDialog* dlgLobby = LobbyDialog::getInstance();
	if(dlgLobby){
		dlgLobby->enterClass(m_courseId,m_classId);
	}
}

void CourseClassItem::setItemParam(__int64 courseId,__int64 classId,int classState, 
	QString classImage,bool isTeacher, QString className,
	QString courseTeacher, QString timeStart,QString timeEnd)
{
	m_courseId = courseId;
	m_classId = classId;

	m_state = classState;

	if(m_state == class_unknow ){
		ui.pushButton_classCtrl->hide();
	}else {
		if(m_state == class_end){
			ui.pushButton_classCtrl->setEnabled(false);
			ui.pushButton_classCtrl->setText(tr("classOver"));
		}else{
			ui.pushButton_classCtrl->setText(tr("enterClass"));
			ui.pushButton_classCtrl->setEnabled(true);
		}

		ui.pushButton_classCtrl->show();
	}

	if(isTeacher){
		ui.label_classFlag->setStyleSheet(QString("background-color: none;image: url(:/res/res/image/default/course_create_flag.png);"));
	}else{
		ui.label_classFlag->setStyleSheet(QString("background-color: none;image: url(:/res/res/image/default/course_pay_flag.png);"));
	}

	ui.label_className->setText(className);
	ui.label_classTeacher->setText(tr("classTeacher")+courseTeacher);
	ui.label_classTime->setText(tr("classTime")+timeStart+QString("-")+timeEnd);

	//load save file
	m_imageName = classImage.right(classImage.length() - classImage.lastIndexOf('/')-1);
	QString imgFile = getImageFileName();
	if(imgFile.isEmpty()){
		return;
	}

	if(QFile::exists(imgFile)){
		SAFE_DELETE(m_pixmap);
		m_pixmap = new QPixmap(imgFile);
		QSize sizeImage(ui.label_classImage->width(),ui.label_classImage->height());
		ui.label_classImage->setPixmap(m_pixmap->scaled(sizeImage,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
	}

	//refresh image from network
	QNetworkRequest request;
	request.setUrl(classImage);
	connect(&m_mgrNetwork, SIGNAL(finished(QNetworkReply *)), this, SLOT(imageDownloadFinished(QNetworkReply *)));
	m_mgrNetwork.get(request);
}

void CourseClassItem::paintEvent(QPaintEvent * event)
{
	if(!this->isVisible()){
		return;
	}

	QPainter painter(this);
	QRect rectWnd = this->rect();

	painter.fillRect(rectWnd,QColor(250,250,250));
}

void CourseClassItem::imageDownloadFinished(QNetworkReply *reply)
{
	if(NULL==reply){
		return;
	}

	if (reply->error() != QNetworkReply::NoError){
		return;
	}

	//set class image
	QByteArray bytes = reply->readAll();
	if(bytes.length()<=0){
		return;
	}

	SAFE_DELETE(m_pixmap);
	m_pixmap = new QPixmap();
	m_pixmap->loadFromData(bytes);

	QSize sizeImage(ui.label_classImage->width(),ui.label_classImage->height());
	ui.label_classImage->setPixmap(m_pixmap->scaled(sizeImage,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
	
	//save class image file
	QString imgFile = getImageFileName();
	if(imgFile.isEmpty()){
		return;
	}

	if(QFile::exists(imgFile)){
		QFile::remove(imgFile);
	}

	m_pixmap->save(imgFile);

	/* set widget background image - no use 
	QPalette palette;
	palette.setBrush(ui.widget_classImage->backgroundRole(),QBrush(pixmap));

	ui.widget_classImage->setPalette(palette);
	ui.widget_classImage->setAutoFillBackground(true);
	ui.widget_classImage->setMask(pixmap.mask());
	*/
}

QString CourseClassItem::getImageFileName()
{
	QString imgPath=Env::getExePath();
	int nPos = imgPath.lastIndexOf('/');
	if(nPos<=0){
		return QString("");
	}

	imgPath = imgPath.left(nPos) + QString("/page/img/");
	if(!Util::IsExistDir(imgPath)){
		return QString("");
	};

	QString imgFile = imgPath + QString("%1_%2").arg(m_courseId).arg(m_imageName);

	return imgFile;
}

bool CourseClassItem::eventFilter(QObject * obj, QEvent * event)
{
	if(NULL == obj || NULL == event){
		return false;
	}

	if(obj == ui.label_classImage){
		if(NULL == m_pixmap){
			return false;
		}
		QSize sizeImage(ui.label_classImage->width(),ui.label_classImage->height());
		
		if(event->type() == QEvent::Enter)
		{
			QPixmap pixmap = m_pixmap->copy(m_pixmap->width()/8,m_pixmap->height()/8,
				m_pixmap->width() * 3/4,m_pixmap->height()*3/4);

			ui.label_classImage->setPixmap(pixmap.scaled(sizeImage,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
		}
		else if(event->type() == QEvent::Leave)
		{
			ui.label_classImage->setPixmap(m_pixmap->scaled(sizeImage,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
		}
	}

	return false;
}