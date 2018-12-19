#include "CourseClassItem.h"
#include <QPainter>
#include <QBitmap>
#include <QFile>
#include "lobbydialog.h"
#include "common/Env.h"
#include "common/Util.h"
#include "common/macros.h"
#include "./common/HttpSessionMgr.h"

CourseClassItem::CourseClassItem(QWidget* parent):C8CommonWindow(parent,SHADOW_QT)
	,m_classId(0)
	,m_courseId(0)
	,m_state(class_unknow)
	,m_imageName("")
	,m_pixmap(NULL)
	,m_idDownClassImage(0)
{
	ui.setupUi(this);
	setItemRound();

	connect(ui.pushButton_classCtrl,SIGNAL(clicked()),this,SLOT(enterClass()));
	ui.label_classImage->installEventFilter(this);
	ui.label_classImage->setMouseTracking(true);

	connect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,SLOT(HttpDownImageCallBack(int, unsigned int, bool, unsigned int)),Qt::QueuedConnection);
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

	if(classImage.isEmpty()||classImage.isNull()){
		return;
	}

	//load save file
	m_imageName = classImage.right(classImage.length() - classImage.lastIndexOf('/')-1);
	QString imageFileName = getImageFileName();
	loadClassImage();
	
	char szImageUrl[1024]={0};
	char szImageFile[1024]={0};
	Util::QStringToChar(classImage,szImageUrl,sizeof(szImageUrl));
	Util::QStringToChar(imageFileName,szImageFile,sizeof(szImageFile));

	if(!CHttpSessionMgr::isValid()){
		connect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,
			SLOT(HttpDownImageCallBack(int, unsigned int, bool, unsigned int)),Qt::QueuedConnection);
	}

	//refresh image from network
	m_idDownClassImage = CHttpSessionMgr::GetInstance()->HttpDownloadFile(
		szImageUrl,szImageFile,
		300, 0, 
		(LPVOID)NULL, (LPVOID)this);
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

void CourseClassItem::HttpDownImageCallBack(int httpEventType, unsigned int lpUser, bool bIsFirst, unsigned int Param)
{
	CourseClassItem *pThis = (CourseClassItem*)lpUser;
	LPHTTPSESSION pHttpS = (LPHTTPSESSION)Param;
	if (NULL == pThis || NULL == pHttpS ||pHttpS->id != m_idDownClassImage)
	{
		return;
	}

	switch (httpEventType)
	{
	case HTTP_EVENT_BEGIN:
		{

		}
		break;

	case HTTP_EVENT_PROGRESS:
		{

		}
		break;

	case HTTP_EVENT_END:
		{   
			if(pHttpS->id==m_idDownClassImage)
			{
				m_idDownClassImage = 0;
				loadClassImage();

				break;
			}
		}

		break;
	}
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
	imgFile.replace('/','\\');
	return imgFile;
}

bool CourseClassItem::eventFilter(QObject * obj, QEvent * event)
{
	if(NULL == obj || NULL == event){
		return false;
	}

	if(obj == ui.label_classImage){
		if(NULL == m_pixmap||m_pixmap->isNull()){
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

void CourseClassItem::loadClassImage()
{
	QString imageFileName = getImageFileName();
	if(imageFileName.isEmpty()){
		return;
	}

	if(!Util::isFileExists(imageFileName)){
		return;
	}
	
	QFile fileImage(imageFileName);
	if(!fileImage.open(QFile::ReadOnly)){
		return;
	}

	do 
	{
		if(fileImage.size()<=4){
			break;
		}

		SAFE_DELETE(m_pixmap);
		m_pixmap = new QPixmap();
		if(NULL==m_pixmap){
			break;
		}

		if(!m_pixmap->loadFromData(fileImage.readAll())){
			break;
		}

		QSize sizeImage(ui.label_classImage->width(),ui.label_classImage->height());
		ui.label_classImage->setPixmap(m_pixmap->scaled(sizeImage,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
	} while (false);
	
	fileImage.close();
}
