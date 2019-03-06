#include "ChatMsgShowWidget.h"
#include <QtGui/QResizeEvent>
#include <QtCore/QTimerEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QScrollBar>
#include <QDomDocument>
#include <QtGui/QTextBlock>
#include <QtCore/QDateTime>
#include <./control/PictureViewer.h>
#include "common/macros.h"
#include "common/util.h"

//////////////////////////////////////////////////////////////////////////
const int CHAT_SYS_MSG_HEIGHT = 40;
const int CHAT_USER_NAME_HEIGHT = 30;
const int CHAT_USER_IMAGE_SIZE = 32;
const int CHAT_USER_MSG_HEIGHT = 80;
const int CHAT_USER_MSG_WIDTH = 50;
const int DEF_MSG_ITEM_HEIGHT = 80;
const int MAX_SHOW_MSG_NUM = 200;	//每页显示的最多信息

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#endif

QChatMsgShowItem::QChatMsgShowItem(QWidget* parent,int itemWidth):QWidget(parent)
	,m_labelUserName(NULL)
	,m_labelUserHeader(NULL)
	,m_textUserMsg(NULL)
	,m_msgType(MSG_UNKNOWN)
	,m_refreshTimer(0)
	,m_labelMovie(NULL)
	,m_movieProcess(NULL)
	,m_userName("")
	,m_userHeader("")
	,m_userMsg("")
	,m_itemIndex(0)
{
	this->setFixedSize(itemWidth,DEF_MSG_ITEM_HEIGHT);
}

QChatMsgShowItem::~QChatMsgShowItem()
{
	SAFE_DELETE(m_labelUserHeader);
	SAFE_DELETE(m_labelUserName);
	SAFE_DELETE(m_textUserMsg);
	SAFE_DELETE(m_labelMovie);
	SAFE_DELETE(m_movieProcess);
	freeMovieMap();
	if(m_refreshTimer){
		killTimer(m_refreshTimer);
		m_refreshTimer = 0;
	}
}

void QChatMsgShowItem::addUserName(const QString& userName)
{
	if(userName.isEmpty()){
		return;
	}

	m_labelUserName = new QLabel(userName,this);
	switch(m_msgType){
	case MSG_SYS:
		{
			m_labelUserName->setGeometry(0,0,this->width(),CHAT_SYS_MSG_HEIGHT);
			m_labelUserName->setObjectName(QStringLiteral("label_ChatSysMsg"));
			m_labelUserName->setAlignment(Qt::AlignCenter);
			m_labelUserName->setStyleSheet(QString("QLabel#label_ChatSysMsg{background-color:none;"
				"font-family: \"Microsoft YaHei\";"
				"font-size: 15px;"
				"color: rgb(0, 162, 232);}"));
		}
		break;
	case MSG_TIME:
		{
			m_labelUserName->setGeometry(0,0,this->width(),CHAT_SYS_MSG_HEIGHT);
			m_labelUserName->setObjectName(QStringLiteral("label_ChatTime"));
			m_labelUserName->setAlignment(Qt::AlignCenter);
			m_labelUserName->setStyleSheet(QString("QLabel#label_ChatTime{background-color:none;"
				"font-family: \"Microsoft YaHei\";"
				"font-size: 15px;"
				"color: rgb(168, 168, 168);}"));
		}
		break;
	case MSG_USER:
	case MSG_SELF:
		{
			m_labelUserName->setObjectName(QStringLiteral("label_ChatMsgName"));
			m_labelUserName->setStyleSheet(QString("QLabel#label_ChatMsgName{background-color:none;"
				"font-family: \"Microsoft YaHei\";"
				"font-size: 14px;"
				"color: rgb(168, 168, 168);}"));
			m_labelUserName->setIndent(5);
			m_labelUserName->setGeometry(0,0,this->width()-10,CHAT_USER_NAME_HEIGHT);
			if(m_msgType == MSG_SELF){
				m_labelUserName->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			}else{
				m_labelUserName->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
			}
		}
		break;
	}
	
	m_labelUserName->show();
}

void QChatMsgShowItem::addUserHeader(const QString& imageHead)
{
	if(imageHead.isEmpty()||imageHead.isNull()){
		return;
	}

	m_labelUserHeader = new QLabel(this);
	m_labelUserHeader->setObjectName(QStringLiteral("label_ChatMsgHeader"));
	m_labelUserHeader->setStyleSheet(QString("QLabel#label_ChatMsgHeader{background-color:none;"
		"border-radius: 50%;}"));

	QPixmap labelPixmap = loadHeadImage(imageHead);
	if(labelPixmap.isNull()){
		m_labelUserHeader->setText(QString("):("));
	}else{
		m_labelUserHeader->setPixmap(labelPixmap.scaled(QSize(CHAT_USER_IMAGE_SIZE,CHAT_USER_IMAGE_SIZE),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
	}

	if(m_msgType==MSG_SELF){
		m_labelUserHeader->setGeometry(this->width()-CHAT_USER_IMAGE_SIZE-20,CHAT_USER_NAME_HEIGHT,CHAT_USER_IMAGE_SIZE,CHAT_USER_IMAGE_SIZE);
	}else{
		m_labelUserHeader->setGeometry(5,CHAT_USER_NAME_HEIGHT,CHAT_USER_IMAGE_SIZE,CHAT_USER_IMAGE_SIZE);
	}

	m_labelUserHeader->show();
}

void QChatMsgShowItem::addUserMsg(const QString& qstrMsg)
{
	if(qstrMsg.isEmpty()||qstrMsg.isNull()){
		return;
	}

	QString  msgShow = qstrMsg;
	parseHtml(qstrMsg,true);
	if(!m_loadImageList.isEmpty()){
		msgShow = QString("<p><img src=\"qrc:/res/res/image/default/chat_load.gif\"></p>");
		m_refreshTimer = this->startTimer(2000);
	}

	m_textUserMsg = new QTextEdit(this);
	QPalette palEdit = m_textUserMsg->palette();
	QSize sizeText = getTextSize(msgShow);
	palEdit.setBrush(QPalette::Base,QBrush(QColor(255,255,255,0)));
	m_textUserMsg->setPalette(palEdit);
	m_textUserMsg->setReadOnly(true);
	m_textUserMsg->setLineWrapMode(QTextEdit::WidgetWidth);
	m_textUserMsg->setObjectName(QStringLiteral("textEdit_ChatMsg"));
	
	int left = 0;
	int top = CHAT_USER_NAME_HEIGHT;
	int height = sizeText.height();
	int width = sizeText.width();

	if(m_msgType == MSG_SELF){
		m_textUserMsg->setStyleSheet(QString("QTextEdit#textEdit_ChatMsg{background-color:none;"
			"font-family: \"Microsoft YaHei\";"
			"font-size: 13px;"
			"color: rgb(255, 255, 255);"
			"border: none;}"));
		left = this->width() - (width + CHAT_USER_IMAGE_SIZE + 40);
	}else {
		m_textUserMsg->setStyleSheet(QString("QTextEdit#textEdit_ChatMsg{background-color:none;"
			"font-family: \"Microsoft YaHei\";"
			"font-size: 13px;"
			"color: rgb(255, 255, 255);"
			"border: none;}"));

		left = CHAT_USER_IMAGE_SIZE + 30;
	}

	m_textUserMsg->setGeometry(left,top,width,height);
	m_textUserMsg->setHtml(msgShow);
	m_textUserMsg->show();
	m_textUserMsg->viewport()->installEventFilter(this);
	m_textUserMsg->setContextMenuPolicy(Qt::NoContextMenu);
	parseHtml(msgShow,false);
}

void QChatMsgShowItem::showSysMsg(const QString& strMsg,bool time /* = false */)
{
	if(strMsg.isEmpty()){
		return;
	}

	m_userName = strMsg;
	m_msgType = time ? MSG_TIME:MSG_SYS;
	addUserName(strMsg);
}

void QChatMsgShowItem::showChatMsg(bool isSelf,const QString& qstrUserName,const QString& qstrHeadImage,const QString& qstrMsg)
{
	if(qstrMsg.isNull() || qstrMsg.isEmpty()){
		return;
	}

	m_msgType = isSelf ? MSG_SELF:MSG_USER;
	m_userName = qstrUserName;
	m_userHeader = qstrHeadImage;
	m_userMsg = qstrMsg;
	// User Name
	addUserName(qstrUserName);

	// User Header
	addUserHeader(qstrHeadImage);

	//User Message
	addUserMsg(qstrMsg);

	if(isSelf){
		showProcessMovie();
	}
}

QSize QChatMsgShowItem::getItemSize()
{
	QSize sizeItem(0,0);
	int width = this->width();
	int height = 0;
	if(NULL!=m_labelUserName){
		height += m_labelUserName->height();
	}

	if(NULL!=m_textUserMsg){
		height += m_textUserMsg->height();
	}else if(NULL!= m_labelUserHeader){
		height += m_labelUserHeader->height();
	}

	this->setFixedSize(width,height);
	sizeItem = QSize(width,height);
	return sizeItem;
}

void QChatMsgShowItem::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter    painter(this);
	QRect       rect= this->rect();
	
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);//消锯齿
	painter.fillRect(rect,QColor(250,250,250));

	if(m_msgType == MSG_TIME || m_msgType == MSG_SYS){
		return;
	}

	if(NULL == m_textUserMsg){
		return;
	}

	QRect rectMsg = m_textUserMsg->geometry();
	rectMsg.setTop(rectMsg.top() - 5);
	rectMsg.setBottom(rectMsg.bottom()- 5);
	rectMsg.setLeft(rectMsg.left() - 5);
	rectMsg.setRight(rectMsg.right() + 5);

	QPointF ptTriangle[3];
	if(m_msgType == MSG_SELF){
		QColor colRect(75,164,242);
		painter.setBrush(QBrush(colRect));

		ptTriangle[1]=QPointF(rectMsg.right()+8,rectMsg.top()+ 18);
		ptTriangle[0]=QPointF(rectMsg.right(),ptTriangle[1].y()- 6);
		ptTriangle[2]=QPointF(rectMsg.right(),ptTriangle[1].y()+ 6);
		
	}else{
		QColor colRect(34,177,76);
		painter.setBrush(QBrush(colRect));

		ptTriangle[1]=QPointF(rectMsg.left()-8,rectMsg.top() + 18);
		ptTriangle[0]=QPointF(rectMsg.left(),ptTriangle[1].y()- 6);
		ptTriangle[2]=QPointF(rectMsg.left(),ptTriangle[1].y()+ 6);
	}
	
	painter.setPen(Qt::NoPen);
	painter.drawRoundedRect(rectMsg,4,4);
	painter.drawPolygon(ptTriangle, 3);
}

void QChatMsgShowItem::resetItemWidth(int itemWidth)
{
	this->setFixedWidth(itemWidth);

	if(m_labelUserName){
		switch(m_msgType){
		case MSG_SYS:
		case MSG_TIME:
			{
				m_labelUserName->setGeometry(0,0,this->width(),CHAT_SYS_MSG_HEIGHT);
			}
			break;
		case MSG_SELF:
		case MSG_USER:
			{
				m_labelUserName->setGeometry(0,0,this->width()-10,CHAT_USER_NAME_HEIGHT);
			}
			break;
		}
	}

	if(m_labelUserHeader){
		if(m_msgType==MSG_SELF){
			m_labelUserHeader->setGeometry(this->width()-CHAT_USER_IMAGE_SIZE-20,CHAT_USER_NAME_HEIGHT,CHAT_USER_IMAGE_SIZE,CHAT_USER_IMAGE_SIZE);
		}else{
			m_labelUserHeader->setGeometry(5,CHAT_USER_NAME_HEIGHT,CHAT_USER_IMAGE_SIZE,CHAT_USER_IMAGE_SIZE);
		}
	}

	if(m_textUserMsg){
		QSize sizeText = getTextSize(m_textUserMsg->toHtml());
		
		int left = 0;
		int top = CHAT_USER_NAME_HEIGHT;
		
		if(m_msgType == MSG_SELF){
			left = this->width() - (sizeText.width() + CHAT_USER_IMAGE_SIZE + 40);
		}else {
			left = CHAT_USER_IMAGE_SIZE + 30;
		}

		m_textUserMsg->setGeometry(left,top,sizeText.width(),sizeText.height());
	}
}

QPixmap QChatMsgShowItem::loadHeadImage(const QString& qstrHeadImage)
{
	QPixmap headPixmap;
	if(qstrHeadImage.isNull()){
		return headPixmap;
	}
	
	QString imageFileName;
	if(qstrHeadImage.indexOf(QString("qrc:/"))>=0){
		imageFileName = qstrHeadImage.right(qstrHeadImage.length()-3);
	}else{
		imageFileName = qstrHeadImage;
	}

	QFile fileImage(imageFileName);
	if(!fileImage.open(QFile::ReadOnly)){
		return headPixmap;
	}

	do 
	{
		if(fileImage.size()<=4){
			break;
		}

		if(!headPixmap.loadFromData(fileImage.readAll())){
			break;
		}
	} while (false);

	fileImage.close();
	return headPixmap;
}

QSize QChatMsgShowItem::getTextSize(const QString& qstrMsg)
{
	QSize sizeText(0,0);
	if(qstrMsg.isNull()||qstrMsg.isEmpty()){
		return sizeText;
	}

	int editWidth = this->width() - CHAT_USER_IMAGE_SIZE - 130;
	QTextEdit* textEdit = new QTextEdit(NULL);
	if(NULL==textEdit){
		return sizeText;
	}

	QFont textFont;
	textFont.setPixelSize(13);
	textFont.setFamily("Microsoft YaHei");

	textEdit->setFont(textFont);
	textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
	textEdit->setHtml(qstrMsg);

	QTextDocument *document = textEdit->document();
	if(NULL==document){
		SAFE_DELETE(textEdit);
		return sizeText;
	}

	document->setTextWidth(editWidth);
	
	int docWidth = (int)document->idealWidth();
	int docHeight = document->size().height() + 10 ;

	bool singleLine = (docWidth + 4) >= editWidth ? false : true;
	
	sizeText.setWidth(MAX(docWidth,CHAT_USER_MSG_WIDTH));
	sizeText.setHeight(docHeight);

	SAFE_DELETE(textEdit);
	return sizeText;
}

void QChatMsgShowItem::freeMovieMap()
{
	while(!m_mapMovieToName.isEmpty()) {
		QMovie* imgMovie = (QMovie*)m_mapMovieToName.firstKey();
		m_mapMovieToName.erase(m_mapMovieToName.begin());

		if(imgMovie){
			imgMovie->stop();
			SAFE_DELETE(imgMovie);
		}
	}
}

void QChatMsgShowItem::parseHtml(const QString& qstrHtml,bool judgeImageLoad)
{
	if(qstrHtml.isEmpty()){
		return;
	}

	QString txtXml = QString("<body>%1</body>").arg(qstrHtml);
	QDomDocument domDoc;
	domDoc.setContent(txtXml);

	QDomNodeList elementList = domDoc.elementsByTagName("img");
	int elementCount = elementList.count();
	if (elementCount <= 0)
	{
		return;
	}
	
	for(int i=0;i<elementCount;i++){
		QDomNode node = elementList.at(i);
		if(!node.hasAttributes()){
			continue;
		}

		QDomNamedNodeMap attriMap = node.attributes();
		if(!attriMap.contains(QString("src"))){
			continue;
		}
		
		QDomNode nodeSrc = attriMap.namedItem(QString("src"));
		QString srcFile = nodeSrc.toAttr().value();
		if(srcFile.isEmpty()){
			continue;
		}

		if(judgeImageLoad){
			parseLoadImage(srcFile);
		}else{
			parseImageMovie(srcFile);
		}
	}
}

void QChatMsgShowItem::parseImageMovie(const QString& imageFile)
{
	if(imageFile.isNull()||imageFile.isEmpty()){
		return;
	}

	if(imageFile.left(5).compare(QString("qrc:/"),Qt::CaseInsensitive) !=0){
		return;
	}

	for(QMapIntString::Iterator itr=m_mapMovieToName.begin();itr!=m_mapMovieToName.end();itr++){
		QString qstrFile = itr.value();
		if(qstrFile.compare(imageFile) == 0){
			return;
		}
	}

	QString imageFileName=imageFile.right(imageFile.length()-3);
	QMovie* imageMovie = new QMovie(imageFileName);
	imageMovie->setCacheMode(QMovie::CacheNone);
	m_mapMovieToName.insert((int)imageMovie,imageFile);
	connect(imageMovie,SIGNAL(frameChanged(int)), this, SLOT(imageAnimate(int)));
	imageMovie->start();
}

void QChatMsgShowItem::parseLoadImage(const QString& imageFile)
{
	if(imageFile.isNull()||imageFile.isEmpty()){
		return;
	}

	if(imageFile.left(8).compare(QString("file:///"),Qt::CaseInsensitive) !=0){
		return;
	}

	QString imageFileName = imageFile.right(imageFile.length() - QString("file:///").length());
	QFile fileImage(imageFileName);
	if(!fileImage.open(QIODevice::ReadOnly)||fileImage.size()<1024){
		m_loadImageList.push_back(imageFile);
	}

	fileImage.close();
}

void QChatMsgShowItem::imageAnimate(int frame)
{
	QMovie* movie = qobject_cast<QMovie*>(sender());
	if(NULL==movie){
		return;
	}

	if(NULL==m_textUserMsg){
		return;
	}
	
	showImageMovie(movie);
	
	if(this->isVisible()){
		QMetaObject::invokeMethod(m_textUserMsg,"update",Qt::QueuedConnection); // 刷新显示 
	}
}

void QChatMsgShowItem::showImageMovie(QMovie* movie)
{
	QMapIntString::Iterator itrFind = m_mapMovieToName.find((int)movie);
	if(itrFind->isNull()||itrFind==m_mapMovieToName.end()){
		return;
	}

	QString urlImage = itrFind.value();
	if(urlImage.isNull()||urlImage.isEmpty()){
		return;
	}

	m_textUserMsg->document()->addResource(QTextDocument::ImageResource, //替换图片为当前帧 
		urlImage, movie->currentPixmap());
}

void QChatMsgShowItem::timerEvent(QTimerEvent * event)
{
	if(NULL==event){
		return;
	}

	int tid = event->timerId();
	if(tid==m_refreshTimer){
		judgeImageLoadFinish();
		return;
	}
}

void QChatMsgShowItem::judgeImageLoadFinish()
{
	for(int i=0;i<m_loadImageList.count();){
		QString imageFile = m_loadImageList.at(i);
		if(imageFile.isNull()||imageFile.isEmpty()){
			i++;
			continue;
		}

		QString imageFileName = imageFile.right(imageFile.length() - QString("file:///").length());
		if(!Util::isFileExists(imageFileName)){
			i++;
			continue;
		}

		m_loadImageList.removeAt(i);
	}

	if(!m_loadImageList.isEmpty()){
		return;
	}

	if(NULL!=m_textUserMsg)
	{
		QSize sizeText = getTextSize(m_userMsg);
		int left = 0;
		
		if(m_msgType == MSG_SELF){
			left = this->width() - (sizeText.width() + CHAT_USER_IMAGE_SIZE + 40);
		}else {
			left = CHAT_USER_IMAGE_SIZE + 30;
		}

		m_textUserMsg->clear();
		m_textUserMsg->setGeometry(left,CHAT_USER_NAME_HEIGHT,sizeText.width(),sizeText.height());
		m_textUserMsg->setHtml(m_userMsg);
		m_textUserMsg->show();

		parseHtml(m_userMsg,false);
	}else{
		addUserMsg(m_userMsg);
	}

	killTimer(m_refreshTimer);
	m_refreshTimer = 0;

	emit refreshMsgShow(m_itemIndex);
}

bool QChatMsgShowItem::eventFilter(QObject * object, QEvent * event)
{
	Q_UNUSED(object);

	if(NULL==m_textUserMsg || object != m_textUserMsg->viewport()){
		return false;
	}

	QEvent::Type eType = event->type();
	switch(eType)
	{
	case QEvent::MouseButtonPress:
		{

		}
		return true;
	case QEvent::MouseButtonRelease:
		{

		}
		return true;

	case QEvent::MouseButtonDblClick:
		{	
			QPoint ptCursor = ((QMouseEvent*)event)->pos();
			QTextCursor txtCursor = m_textUserMsg->cursorForPosition(ptCursor);
			int txtPos = txtCursor.position();
			QTextBlock txtBlock = m_textUserMsg->document()->findBlock(txtPos);
			QTextBlock::iterator itr = txtBlock.begin();

			int movPos = 0;
			for (itr = txtBlock.begin(); !(itr.atEnd());itr++)
			{
				QTextFragment currentFragment = itr.fragment();
				QTextImageFormat curImageFormat = currentFragment.charFormat().toImageFormat();
				if(curImageFormat.isValid()){
					QString name = curImageFormat.name();
					if(abs(movPos-txtPos)<=1){		//显示图片
						if(name.length()<QString("file:///").length() || name.indexOf("file:///") != 0)
						{
							break;
						}

						QString filePath = name.right(name.length() - QString("file:///").length());
						PictureViewer::getInstance()->openImage(filePath);
						break;
					}
					movPos++;
				}else{
					QString text = currentFragment.text();
					movPos += text.length();
				}
			}
		}
		return true;
	default:
		break;
	}

	return false;
}

bool QChatMsgShowItem::judgeChatMsg(bool isSelf,const QString& qstrUserName,const QString& qstrHeadImage,const QString& qstrMsg)
{
	if(!isSelf){
		return false;
	}

	if (qstrUserName.compare(m_userName) !=0 || 
		qstrHeadImage.compare(m_userHeader) !=0 || 
		qstrMsg.compare(m_userMsg) !=0 ){
			return false;
	}

	if(NULL== m_labelMovie||NULL==m_movieProcess){
		return false;
	}

	m_movieProcess->stop();
	m_labelMovie->hide();

	SAFE_DELETE(m_movieProcess);
	SAFE_DELETE(m_labelMovie);

	return true;
}

void QChatMsgShowItem::showProcessMovie()
{
	m_labelMovie = new QLabel(this);
	m_movieProcess = new QMovie(QString(":/res/res/image/default/chat_load.gif"));

	if(NULL== m_labelMovie||NULL==m_movieProcess){
		return;
	}

	m_labelMovie->setMovie(m_movieProcess);
	m_movieProcess->start();

	int width = m_movieProcess->frameRect().width();
	int height = m_movieProcess->frameRect().height();

	int top = CHAT_USER_NAME_HEIGHT + 5;
	int left = m_textUserMsg->geometry().left() - width - 30;
	
	m_labelMovie->setGeometry(left,top,width,height);
	m_labelMovie->show();
}


//////////////////////////////////////////////////////////////////////////
QChatMsgShowWidget::QChatMsgShowWidget(QWidget* parent):QListWidget(parent)
	,m_lastChatTime(0)
{
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->startTimer(500);
}

QChatMsgShowWidget::~QChatMsgShowWidget()
{
	clearAllMsg();
}

void QChatMsgShowWidget::addSystemMsg(const QString& qstrSysMsg)
{
	if(qstrSysMsg.isNull()||qstrSysMsg.isEmpty()){
		return ;
	}

	QChatMsgShowItem* msgItem = new QChatMsgShowItem(this->parentWidget(),getItemWidth());
	if(NULL==msgItem){
		return ;
	}

	msgItem->showSysMsg(qstrSysMsg);

	addListItem(msgItem);
}

void QChatMsgShowWidget::addChatMsg(bool isSelf,const QString& qstrUserName,const QString& qstrHeadImage,const QString& qstrMsg)
{
	if(qstrMsg.isNull()||qstrMsg.isEmpty()){
		return ;
	}

	if(hasChatMsg(isSelf,qstrUserName,qstrHeadImage,qstrMsg)){
		return;
	}

	int curTime = QDateTime::currentDateTime().toTime_t();
	if(curTime - m_lastChatTime >= 180){
		addSystemMsg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss "));
		m_lastChatTime = curTime;
	}

	QChatMsgShowItem* msgItem = new QChatMsgShowItem(this->parentWidget(),getItemWidth());
	if(NULL==msgItem){
		return;
	}

	msgItem->showChatMsg(isSelf,qstrUserName,qstrHeadImage,qstrMsg);

	addListItem(msgItem);
}

void QChatMsgShowWidget::clearAllMsg()
{
	popFrontListItem(this->count());
}

void QChatMsgShowWidget::delFrontMsg(int delMsgNum)
{
	popFrontListItem(delMsgNum);
	resetListItem();
}

void QChatMsgShowWidget::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	resetListItem();
}

void QChatMsgShowWidget::timerEvent(QTimerEvent *event)
{
	if(NULL==event){
		return;
	}

	int tid=event->timerId();
	killTimer(tid);
	emit loadFinished();
}

void QChatMsgShowWidget::popFrontListItem(int number)
{
	int count = this->count();
	if(count<=0){
		return;
	}

	number = number < count ? number : count;
	for(int i=0;i<number;i++){
		QListWidgetItem * itemList = this->takeItem(0);
		if(NULL!=itemList){
			QChatMsgShowItem* itemWidget = (QChatMsgShowItem*)this->itemWidget(itemList);
			SAFE_DELETE(itemWidget);
		}

		SAFE_DELETE(itemList);
	}
}

void QChatMsgShowWidget::addListItem(QChatMsgShowItem* msgItem)
{
	if(NULL==msgItem){
		return ;
	}

	QScrollBar * verBar = this->verticalScrollBar();
	bool barVisible = false;
	if(verBar){
		barVisible = verBar->isVisible();
	}

	QListWidgetItem * listItem = new QListWidgetItem(this);
	if(NULL==listItem){
		SAFE_DELETE(msgItem);
		return ;
	}
	
	QSize size = msgItem->getItemSize();
	listItem->setSizeHint(size);
	this->setItemWidget(listItem, msgItem);
	
	connect(msgItem,SIGNAL(refreshMsgShow(int)),this,SLOT(refreshItemShow(int)));
	
	if(this->count()>=MAX_SHOW_MSG_NUM){
		popFrontListItem(10);
	}

	this->setCurrentItem(listItem);
	msgItem->setItemIndex(this->count()-1);

	if(NULL == verBar || !verBar->isVisible()){
		return;
	}

	if(verBar->isVisible() == barVisible){
		return;
	}

	resetListItem();
	return;
}

int QChatMsgShowWidget::getItemWidth()
{
	int itemWidth = this->width();
	QScrollBar * verBar = this->verticalScrollBar();
	if(NULL == verBar || !verBar->isVisible()){
		return itemWidth;
	}

	itemWidth -= verBar->width();
	return itemWidth;
}

void QChatMsgShowWidget::resetListItem()
{
	int count = this->count();
	if(count<=0){
		return;
	}

	for(int i=0;i<count;i++){
		QListWidgetItem * itemList = this->item(i);
		if(NULL==itemList){
			continue;
		}

		QChatMsgShowItem* itemWidget = (QChatMsgShowItem*)this->itemWidget(itemList);
		if(NULL==itemWidget){
			continue;
		}

		itemWidget->resetItemWidth(getItemWidth());
		itemWidget->setItemIndex(i);
		itemList->setSizeHint(itemWidget->getItemSize());
	}
}

void QChatMsgShowWidget::refresh()
{

}

void QChatMsgShowWidget::refreshItemShow(int itemIndex)
{
	int count = this->count();
	if(count<=itemIndex){
		return;
	}

	
	QListWidgetItem * itemList = this->item(itemIndex);
	if(NULL==itemList){
		return;
	}

	QChatMsgShowItem* itemWidget = (QChatMsgShowItem*)this->itemWidget(itemList);
	if(NULL==itemWidget){
		return;
	}

	itemList->setSizeHint(itemWidget->getItemSize());
}

bool QChatMsgShowWidget::hasChatMsg(bool isSelf,const QString& qstrUserName,const QString& qstrHeadImage,const QString& qstrMsg)
{
	int count = this->count();
	for(int i=0;i<count;i++){
		QListWidgetItem * itemList = this->item(i);
		if(NULL==itemList){
			continue;
		}

		QChatMsgShowItem* itemWidget = (QChatMsgShowItem*)this->itemWidget(itemList);
		if(NULL==itemWidget){
			continue;
		}

		if(itemWidget->judgeChatMsg(isSelf,qstrUserName,qstrHeadImage,qstrMsg)){
			return true;
		}
	}

	return false;
}
